#include "GLPrimitiveRenderer.h"

#include <Sorex/Math/SxMatrix3x3.h>

#include "GLRenderDevice.h"

namespace Sorex::Graphics
{
  GLPrimitiveRenderer::GLPrimitiveRenderer(GLRenderDevice* glRenderDevice,
                                           size_t vtxCapacity /*= 1024*/)
    : mRenderDevice(glRenderDevice)
    // , _canvasState(nullptr)
    , mVtxArray(glRenderDevice)
    , mVtxCapacity(std::max<size_t>(vtxCapacity, 8u))
  {}

  Status GLPrimitiveRenderer::Initialize() SRX_NOEXCEPT
  {
    if (mRenderDevice == nullptr)
      return SRX_STATUS_MSG(EStatusCode::Invalid_State,
                            "Invalid render device");

    Status status = mVtxArray.Initialize(mVtxCapacity);
    if (!status.Ok())
      return status;

    /*
        mShaderProgram = GLShaderProgram::CreateFromSource(
          _glDevice,
          GLShaderSource::kColorVertexShaderSource,
          GLShaderSource::kColorFragmentShaderSource,
          error);
     */
    // if (!_shaderProgram)
    // return false;
    mShaderProgram->SetRenderingMode(ERenderingMode::Lines);
    return status;
  }

  Status GLPrimitiveRenderer::Activate() SRX_NOEXCEPT
  {
    SRX_CHECK(mShaderProgram && mRenderDevice);
    if (!mShaderProgram || !mRenderDevice)
      return SRX_STATUS(EStatusCode::Invalid_State);

    Flush();

    // _canvasState = &state;
    // return mRenderDevice->ApplyRenderTechnique(_technique, error);
    return SRX_OK;
  }

  void GLPrimitiveRenderer::Flush() SRX_NOEXCEPT
  {
    if (IsEmpty())
      return;
    /*
        Error error;
        if (_glDevice->Draw(_vtxArray, &error) == false)
          RFY_WARN("[GLPrimitiveRenderer] Draw call failed: {}",
                   error.ToString().c_str());
     */

    Reset();
  }

  void GLPrimitiveRenderer::Reset() SRX_NOEXCEPT
  {
    mVtxArray.Clear();
  }

  void GLPrimitiveRenderer::DrawLine(Point        begin,
                                     Point        end,
                                     const Color* color,
                                     size_t       colorNumber) SRX_NOEXCEPT
  {
    SwitchRenderingMode(ERenderingMode::Lines);

    /* if (_canvasState && _canvasState->bUseTransform)
    {
      begin = Matrix3x3::Transform(_canvasState->transform, begin);
      end   = Matrix3x3::Transform(_canvasState->transform, end);
    } */

    if (!color || !colorNumber)
    {
      color       = &Color::White;
      colorNumber = 1;
    }

    Vertex::V2F_C4B* vertices = AllocateVertices(2);
    SRX_CHECK_MSG(vertices, "[GLPrimitiveRenderer] Vertices allocation failed");

    if (vertices)
    {
      vertices[0].position[0] = begin.x;
      vertices[0].position[1] = begin.y;
      vertices[0].color       = color[0].value;

      vertices[1].position[0] = end.x;
      vertices[1].position[1] = end.y;
      vertices[1].color = (colorNumber > 1) ? color[1].value : color[0].value;
    }
  }

  void GLPrimitiveRenderer::DrawBorder(const Rectangle& rect,
                                       const Color*     color,
                                       size_t colorNumber) SRX_NOEXCEPT
  {
    if (!rect.GetSize().IsValid())
      return;

    rect.ToArray(mPoints);
    DrawBorder(color, colorNumber);
  }

  void GLPrimitiveRenderer::DrawBorder(const Color* color, size_t colorNumber)
  {
    SwitchRenderingMode(ERenderingMode::Lines);

    constexpr size_t kVtxNumber = 8;
    Vertex::V2F_C4B* vertices   = AllocateVertices(kVtxNumber);

    SRX_CHECK_MSG(vertices, "[GLPrimitiveRenderer] Vertices allocation failed");
    if (vertices == nullptr)
      return;

    /* if (_canvasState && _canvasState->bUseTransform)
    {
      for (Point& p : mPoints)  // cppcheck-suppress useStlAlgorithm
        p = Matrix3x3::Transform(_canvasState->transform, p);
    } */

    if (!color || !colorNumber)
    {
      color       = &Color::White;
      colorNumber = 1;
    }

    for (size_t i = 0; i < mPoints.size(); ++i)
    {
      const size_t vtxIndex          = 2 * i;
      vertices[vtxIndex].position[0] = mPoints[i].x;
      vertices[vtxIndex].position[1] = mPoints[i].y;
      vertices[vtxIndex].color       = color[(i % colorNumber)].value;

      const size_t next                  = i + 1;
      const size_t nextPointIndex        = (next % mPoints.size());
      vertices[vtxIndex + 1].position[0] = mPoints[nextPointIndex].x;
      vertices[vtxIndex + 1].position[1] = mPoints[nextPointIndex].y;
      vertices[vtxIndex + 1].color       = color[(next % colorNumber)].value;
    }
  }

  void GLPrimitiveRenderer::DrawRectangle(const Rectangle& rect,
                                          const Color*     color,
                                          size_t colorNumber) SRX_NOEXCEPT
  {
    if (!rect.GetSize().IsValid())
      return;

    rect.ToArray(mPoints);
    DrawRectangle(color, colorNumber);
  }

  void GLPrimitiveRenderer::DrawRectangle(const Color* color,
                                          size_t       colorNumber)
  {
    SwitchRenderingMode(ERenderingMode::Triangles);

    /* if (_canvasState && _canvasState->bUseTransform)
    {
      for (Point& p : mPoints)  // cppcheck-suppress useStlAlgorithm
        p = Matrix3x3::Transform(_canvasState->transform, p);
    } */

    if (!color || !colorNumber)
    {
      color       = &Color::White;
      colorNumber = 1;
    }

    if (Vertex::V2F_C4B* vertices = AllocateVertices(6))
    {
      vertices[0].position[0] = vertices[3].position[0] = mPoints[0].x;
      vertices[0].position[1] = vertices[3].position[1] = mPoints[0].y;
      vertices[0].color = vertices[3].color = color->value;

      vertices[1].position[0] = mPoints[1].x;
      vertices[1].position[1] = mPoints[1].y;
      vertices[1].color       = color[(1 % colorNumber)].value;

      vertices[2].position[0] = vertices[4].position[0] = mPoints[2].x;
      vertices[2].position[1] = vertices[4].position[1] = mPoints[2].y;
      vertices[2].color = vertices[4].color = color[(2 % colorNumber)].value;

      vertices[5].position[0] = mPoints[3].x;
      vertices[5].position[1] = mPoints[3].y;
      vertices[5].color       = color[(3 % colorNumber)].value;
    }
  }

  void GLPrimitiveRenderer::DrawCircle(Point center,
                                       float radius,
                                       int32 segments,
                                       Color color) SRX_NOEXCEPT
  {
    if (radius < 0.1f)
    {
      SRX_NOENTRY("[GLPrimitiveRenderer] Invalid cicle radius");
      return;
    }

    SwitchRenderingMode(ERenderingMode::Lines);

    /* if (_canvasState && _canvasState->bUseTransform)
      center = Mat3::Transform(_canvasState->transform, center); */

    const int32 n    = std::max<int32>(segments, 4);
    const float coef = 2.f * (float)M_PI / n;

    const int32      vtxNumber = 2 * n;
    Vertex::V2F_C4B* vertices  = AllocateVertices(vtxNumber);
    SRX_CHECK_MSG(vertices, "[GLPrimitiveRenderer] Vertices allocation failed");

    if (vertices == nullptr)
      return;

    Point point(center.x + radius, center.y);
    for (int32 i = 1; i < n + 1; ++i)
    {
      float a        = i * coef;
      int32 vtxIndex = 2 * (i - 1);

      vertices[vtxIndex].position[0] = point.x;
      vertices[vtxIndex].position[1] = point.y;
      vertices[vtxIndex].color       = color.value;

      ++vtxIndex;
      point.x = center.x + radius * cosf(a);
      point.y = center.y + radius * sinf(a);

      vertices[vtxIndex].position[0] = point.x;
      vertices[vtxIndex].position[1] = point.y;
      vertices[vtxIndex].color       = color.value;
    }
  }

  Vertex::V2F_C4B* GLPrimitiveRenderer::AllocateVertices(size_t vtxNumber)
    SRX_NOEXCEPT
  {
    const size_t capacity = mVtxArray.GetVertexCapacity();
    if (vtxNumber > capacity)
    {
      SRX_NOENTRY(
        "[GLPrimitiveRenderer] Vertices allocation failed: buffer too small");
      return nullptr;
    }

    const size_t avaliable = capacity - mVtxArray.GetVertexNum();
    if (avaliable < vtxNumber)
      Flush();  // @note: free buffers then allocate vertices

    return mVtxArray.AllocateVertex(vtxNumber);
  }

  void GLPrimitiveRenderer::SwitchRenderingMode(ERenderingMode mode)
  {
    if (mShaderProgram->GetRenderingMode() == mode)
      return;

    Flush();
    mShaderProgram->SetRenderingMode(mode);
  }
}
