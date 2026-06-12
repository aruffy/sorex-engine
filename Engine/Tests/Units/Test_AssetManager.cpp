#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <sstream>
#include <thread>
#include <chrono>

#include <Sorex/Asset/SxAsset.h>
#include <Sorex/Asset/SxAssetCreator.h>
#include <Sorex/Asset/SxAssetHandler.h>
#include <Sorex/Asset/SxAssetManager.h>
#include <Sorex/FileSystem/SxPathUtils.h>

#include <Asset/SxAssetLoadingTask.h>

using namespace Sorex;

namespace
{
  uint64_t s_GetThreadId()
  {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    return static_cast<uint64_t>(std::atoll(ss.str().c_str()));
  }
}

enum class ELoadingStage
{
  None,
  Preload,
  Load,
  Finalizing,
  Done
};

struct TestAssetOptions
{
  ELoadingStage failStage        = ELoadingStage::None;
  bool          bMakeMissingFile = false;
};

// ========== TEST ASSET ========== //
class TestAsset final: public Resource::Asset
{
  SRX_RTTI(TestAsset, Resource::Asset);

  public:
  TestAsset(Path path, const TestAssetOptions& options)
    : Asset(std::move(path))
    , _ctid(s_GetThreadId())
    , _ltid(0ull)
    , _stage(ELoadingStage::None)
    , _options(options)
  {}

  void ToStage(const ELoadingStage stage)
  {
    SRX_ASSERT(_stage < stage);
    _stage = stage;
  }

  inline ELoadingStage GetStage() const { return _stage; }
  inline bool IsLoaded() const { return _stage == ELoadingStage::Done; }

  inline const TestAssetOptions& GetOptions() const { return _options; }

  inline uint64 GetCreationThread() const { return _ctid; }
  inline uint64 GetLoadingThread() const { return _ltid; }
  inline void   SetLoadingThread(uint64 val) { _ltid = val; }

  private:
  uint64           _ctid;  ///< Creation thread id
  uint64           _ltid;  ///< Loading thread id
  ELoadingStage    _stage;
  TestAssetOptions _options;
};
// ========== TEST ASSET LOADER ========== //
class TestAssetLoader: public Resource::AssetLoader
{
  public:
  virtual ~TestAssetLoader() override = default;
  explicit TestAssetLoader(TSharedPointer<TestAsset> asset)
    : AssetLoader(asset)
  {
    _asset = asset.get();
  }

  virtual Status Preload(Resource::AssetStorage&  storage,
                         Resource::AssetRegistry* registry,
                         TVector<Path>&           missingFiles) override
  {
    SRX_ASSERT(_asset->GetStage() == ELoadingStage::None);
    _asset->ToStage(ELoadingStage::Preload);

    if (_asset->GetOptions().bMakeMissingFile && _preload == 0)
    {
      _preload++;
      missingFiles.push_back(_asset->GetPath());
      return SRX_OK;
    }

    if (_asset->GetOptions().failStage == ELoadingStage::Preload)
      return SRX_STATUS_MSG(EStatusCode::Not_Permitted, "preload failed");

    return SRX_OK;
  }

  virtual Status Load(Resource::AssetStorage&       storage,
                      const Resource::AssetOptions* options,
                      Resource::AssetDependencies&  dependencies) override
  {
    SRX_ASSERT(_asset->GetStage() == ELoadingStage::Preload);

    _asset->SetLoadingThread(s_GetThreadId());
    _asset->ToStage(ELoadingStage::Load);

    if (_asset->GetOptions().failStage == ELoadingStage::Load)
      return SRX_STATUS_MSG(EStatusCode::Not_Permitted, "loading failed");

    return SRX_OK;
  }

  virtual Status Finalize(
    Resource::AssetRegistry*           registry,
    const Resource::AssetDependencies& dependencies) override
  {
    SRX_ASSERT(_asset->GetStage() == ELoadingStage::Load);

    _asset->ToStage(ELoadingStage::Finalizing);
    if (_asset->GetOptions().failStage == ELoadingStage::Finalizing)
      return SRX_STATUS_MSG(EStatusCode::Not_Permitted, "finalizing failed");

    _asset->ToStage(ELoadingStage::Done);
    return SRX_OK;
  }

  private:
  int        _preload = 0;
  TestAsset* _asset;
};

class MockAssetLoader final: public TestAssetLoader
{
  public:
  MockAssetLoader(TSharedPointer<TestAsset> a)
    : TestAssetLoader(std::move(a))
  {}

  MOCK_METHOD(Status,
              Preload,
              (Resource::AssetStorage & storage,
               Resource::AssetRegistry* registry,
               TVector<Path>&           missingFiles),
              (override));
  MOCK_METHOD(Status,
              Load,
              (Resource::AssetStorage & storage,
               const Resource::AssetOptions* options,
               Resource::AssetDependencies&  dependencies),
              (override));
  MOCK_METHOD(Status,
              Finalize,
              (Resource::AssetRegistry * registry,
               const Resource::AssetDependencies& dependencies),
              (override));
};
// ========== TEST ASSET CREATOR ========== //
class TestAssetCreator final: public Resource::AssetCreator
{
  bool _bMock = false;

  public:
  TestAssetCreator(bool bMock = false)
    : _bMock(bMock)
  {}
  TestAssetOptions      options;
  virtual AssetInstance CreateAssetInstance(Path                     path,
                                            Resource::AssetRegistry* registry,
                                            Status* status) override
  {
    auto asset = std::make_shared<TestAsset>(std::move(path), options);
    TUniquePointer<Resource::AssetLoader> loader;
    if (_bMock)
      loader = std::make_unique<MockAssetLoader>(asset);
    else
      loader = std::make_unique<TestAssetLoader>(asset);
    return std::make_pair(std::move(asset), std::move(loader));
  }
};
// ========== TEST ASSET AWAITER ========== //
class TestAwaiter final: public Resource::IAssetAwaiter
{
  public:
  explicit TestAwaiter(String* str, int n = 1)
    : _waitingAssetName(str)
    , _attempts(n)
    , _counter(0)
  {}

  virtual ETaskAction GetAssetStatus(const Resource::Asset& asset,
                                     Status*                status) override
  {
    if (_waitingAssetName && _waitingAssetName->empty())
      _waitingAssetName->assign(asset.GetName());

    _counter++;
    return _counter >= _attempts ? ETaskAction::Continue : ETaskAction::Await;
  }

  private:
  String* _waitingAssetName;
  int     _attempts;
  int     _counter = 0;
};
// ========== TEST ASSET LOADING HANDLER ========== //
class TestAssetLoadingHandler final: public Resource::IAssetLoadingHandler
{
  public:
  explicit TestAssetLoadingHandler(const bool bAwait = false)
    : _bAwait(bAwait)
    , _bFailed(false)
    , _bSuccess(false)
  {}

  virtual void OnAssetLoadingFailed(const Path&      path,
                                    Resource::Asset* asset,
                                    Status&          reason) override
  {
    _bFailed = true;
  }
  virtual void OnAssetLoaded(Resource::AssetRegistry* registry,
                             Resource::Asset*         asset) override
  {
    _bSuccess = true;
  }

  virtual TPair<ETaskAction, TUniquePointer<Resource::IAssetAwaiter>>
  HandleMissingFiles(Resource::AssetStorage&  storage,
                     Resource::AssetRegistry* registy,
                     const Resource::Asset*   asset,
                     const TSpan<Path>&       files) override
  {
    if (_bAwait)
      return std::make_pair(ETaskAction::Await,
                            MakeUnique<TestAwaiter>(&_waitingAssetName));

    return std::make_pair(ETaskAction::Cancel, nullptr);
  }

  inline bool          IsSuccess() const { return _bSuccess; }
  inline bool          IsFailed() const { return _bFailed; }
  inline void          SetAwait(bool value) { _bAwait = value; }
  inline const String& GetWaitingAssetName() const { return _waitingAssetName; }

  inline void Reset()
  {
    _bFailed = _bSuccess = false;
    _waitingAssetName.clear();
  }

  private:
  String _waitingAssetName;
  bool   _bAwait   = false;
  bool   _bFailed  = false;
  bool   _bSuccess = false;
};
// ========== TEST ASSET STORAGE ========== //
class TestStorage final: public Resource::AssetStorage
{
  public:
  virtual TUniquePointer<Stream> Read(const Path& path, Status* status) override
  {
    return nullptr;
  }
};
// ========== TEST ========== //
TEST(GTestAssetLoadingTask, LoadSimpleAsset)
{
  auto createAssetInstance = [](const RuntimeClass& cls, Path path, Status* st)
    -> Resource::AssetCreator::AssetInstance {
    TestAssetOptions          testAssetOptions;
    TSharedPointer<TestAsset> testAsset =
      std::make_shared<TestAsset>(std::move(path), testAssetOptions);
    TUniquePointer<MockAssetLoader> mockAssetLoader =
      std::make_unique<MockAssetLoader>(testAsset);

    EXPECT_CALL(*mockAssetLoader, Preload).WillOnce(testing::Return(SRX_OK));
    EXPECT_CALL(*mockAssetLoader, Load).WillOnce(testing::Return(SRX_OK));
    EXPECT_CALL(*mockAssetLoader, Finalize).WillOnce(testing::Return(SRX_OK));

    return std::make_pair(std::move(testAsset), std::move(mockAssetLoader));
  };

  Resource::AssetLoadingTask::Parameters params;
  TestStorage                            testStorage;
  params.path    = SRX_PATH("/test/asset");
  params.type    = &Sorex::GetRuntimeType<TestAsset>();
  params.storage = &testStorage;

  auto task = Resource::AssetLoadingTask::Create(params, createAssetInstance);
  task->Execute();
  task->Finalize();
}

static bool WaitAssetLoading(Resource::AssetManager&      mng,
                             const Resource::Asset* const asset,
                             const float                  step = 25.f)
{
  if (!asset)
    return false;

  unsigned ms = 0;
  while (asset->GetState() <= Resource::EAssetState::Loading)
  {
    constexpr unsigned timeout = 5 * 1000;
    if (ms > timeout)
      return false;

    mng.Update(step);
    std::this_thread::sleep_for(std::chrono::milliseconds((unsigned)step));
    ms += (unsigned)step;
  }

  return true;
}

TEST(GTestAssetManager, LoadSync)
{
  TestStorage    storage;
  SxAssetManager manager(storage, nullptr);

  TestAssetCreator* assetCreator = nullptr;
  {
    auto ac      = MakeUnique<TestAssetCreator>();
    assetCreator = ac.get();
    manager.Register<TestAsset>(std::move(ac));
  }

  auto asset = manager.Load<TestAsset>(SRX_PATH("/test/asset"));
  ASSERT_NE(asset, nullptr);
  EXPECT_EQ(asset->GetCreationThread(), asset->GetLoadingThread());
  EXPECT_TRUE(asset->IsLoaded());
  EXPECT_EQ(asset->GetState(), Resource::EAssetState::Loaded);

  asset.reset();
  ASSERT_EQ(asset, nullptr);

  TestAssetLoadingHandler handler;
  asset = manager.Load<TestAsset>(SRX_PATH("/test/asset/load/sync"), &handler);
  ASSERT_NE(asset, nullptr);
  EXPECT_EQ(asset->GetCreationThread(), asset->GetLoadingThread());
  EXPECT_TRUE(asset->IsLoaded());
  EXPECT_EQ(asset->GetState(), Resource::EAssetState::Loaded);
  EXPECT_FALSE(handler.IsFailed());
  EXPECT_TRUE(handler.IsSuccess());
  handler.Reset();

  assetCreator->options.failStage = ELoadingStage::Preload;
  asset =
    manager.Load<TestAsset>(SRX_PATH("/test/asset/fail/preload"), &handler);
  ASSERT_EQ(asset, nullptr);
  EXPECT_TRUE(handler.IsFailed());
  EXPECT_FALSE(handler.IsSuccess());
  handler.Reset();

  assetCreator->options.failStage = ELoadingStage::Load;
  asset = manager.Load<TestAsset>(SRX_PATH("/test/asset/fail/load"), &handler);
  ASSERT_EQ(asset, nullptr);
  EXPECT_TRUE(handler.IsFailed());
  EXPECT_FALSE(handler.IsSuccess());
  handler.Reset();

  assetCreator->options.failStage = ELoadingStage::Finalizing;
  asset =
    manager.Load<TestAsset>(SRX_PATH("/test/asset/fail/finalizing"), &handler);
  ASSERT_EQ(asset, nullptr);
  EXPECT_TRUE(handler.IsFailed());
  EXPECT_FALSE(handler.IsSuccess());
  handler.Reset();
}

TEST(GTestAssetManager, LoadAsync)
{
  TestStorage            storage;
  Resource::AssetManager manager(storage, nullptr);
  manager.Register<TestAsset>(MakeUnique<TestAssetCreator>());

  ASSERT_EQ(manager.Initialize(), SRX_OK);

  auto asset = manager.LoadAsync<TestAsset>(SRX_PATH("/test/asset/load/async"));
  ASSERT_NE(asset, nullptr);

  WaitAssetLoading(manager, asset.get());

  EXPECT_NE(asset->GetCreationThread(), asset->GetLoadingThread());
  EXPECT_TRUE(asset->IsLoaded());
  EXPECT_EQ(asset->GetState(), Resource::EAssetState::Loaded);
  EXPECT_EQ(asset->GetStage(), ELoadingStage::Done);

  manager.Shutdown();
}

TEST(GTestAssetManager, LoadAsyncWithAwaiter)
{
  TestStorage            storage;
  Resource::AssetManager manager(storage, nullptr);
  {
    auto ac                      = MakeUnique<TestAssetCreator>();
    ac->options.bMakeMissingFile = true;
    manager.Register<TestAsset>(std::move(ac));
  }

  ASSERT_EQ(manager.Initialize(), SRX_OK);

  TestAssetLoadingHandler handler;
  handler.SetAwait(true);
  auto asset =
    manager.LoadAsync<TestAsset>(SRX_PATH("/test/asset/async/await"), &handler);

  WaitAssetLoading(manager, asset.get());

  EXPECT_NE(asset->GetCreationThread(), asset->GetLoadingThread());
  EXPECT_FALSE(handler.IsFailed());
  EXPECT_TRUE(handler.IsSuccess());
  EXPECT_EQ(asset->GetState(), Resource::EAssetState::Loaded);
  EXPECT_EQ(asset->GetStage(), ELoadingStage::Done);
  EXPECT_EQ(handler.GetWaitingAssetName(), asset->GetName());

  manager.Shutdown();
}
