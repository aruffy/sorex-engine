/**************************************************************************/
/*                         This file is part of:                          */
/*                                SOREX                                   */
/*                 Simple OpenGL Rendering Engine eXtended                */
/**************************************************************************/
/* Copyright (c) 2022 Aleksandr Ershov (Ruffy).                           */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#pragma once

#include "SxCoreMinimal.h"
#include "Director.h"
#include "Thread.h"

namespace Sorex
{
  /**
   * DirectorLauncher - make preparation and control the start of an
   * director on different platform.
   */
  class DirectorLauncher
  {
public:
    virtual ~DirectorLauncher() = default;

    DirectorLauncher(const DirectorLauncher& other)            = delete;
    DirectorLauncher& operator=(const DirectorLauncher& other) = delete;

    /**
     * @brief Create instance of the Director and start applications loop.
     *
     * @return Null if application was run successfully, else valid error unique
     * pointer.
     */
    template<typename App, typename... Args>
    SRX_TYPENAME std::enable_if_t<std::is_base_of_v<Director, App>, Status> Run(
      Args&&... args);

protected:
    DirectorLauncher();

    /**
     * @brief OnStartup is called when the sorex have entered to start point
     *
     * The role of this function prepare platform (system) before engine
     * start. All system initialization must be here. If returned status isn't
     * ok the launcher breaks.
     *
     * @return Status indicating the result of the startup process.
     */
    virtual Status OnStartup() = 0;

    /**
     * @brief OnShutdown is called to finish all work.
     *
     * The role of this function to free and destroy all resources that have
     * been allocated/inited on startup. This function will be invoked even the
     * Deactivate function return error;
     */
    virtual void OnShutdown() = 0;

    /**
     * @brief Called before Director start. This function must prepare
     * the director instance to run.
     *
     * @note: There is not need to call Director::Initialize(); It will be done
     * AFTER this function call: see DirectorLauncher::Initialize.
     *
     * @param director - Pointer to the Director instance that needs to be
     * initialized.
     *
     * @return Status - If initialization was successful, returns a success
     * status; otherwise, returns an error status indicating failure.
     */
    virtual Status OnInitialize(Director& director) = 0;

    /**
     * @brief Called after the director exit main loop. This function must
     * free resources that were allocated during the initialization.
     *
     * @note There is no need to call Director::Deactivate(); It was already
     * done BEFORE the call to this function: see DirectorLauncher::Deactivate.
     *
     * @param director Pointer to the deactivating application.
     * @return Status indicating success if the application was deactivated
     * successfully and can be destroyed; otherwise, an error status.
     */
    virtual Status OnDeactivate(Director& director) = 0;

private:
    Status Initialize(Director& director);
    Status Deactivate(Director& director);
  };

  template<typename T, typename... Args>
  SRX_TYPENAME std::enable_if_t<std::is_base_of_v<Director, T>, Status>
               DirectorLauncher::Run(Args&&... args)
  {
    if (!Thread::IsMainThread())
      return SRX_STATUS_MSG(EStatusCode::Not_Permitted,
                            "Application must be run on main thread");

    try
    {
      Status status;
      if (status = OnStartup(); !status.Ok())
        return status;

      TUniquePointer<Director> director =
        MakeUnique<T>(std::forward<Args>(args)...);

      if (status = Initialize(*director); !status.Ok())
      {
        SRX_ERROR("[DirectorLauncher] Initilization failed: {}",
                  status.ToString());
        return status;
      }

      director->MainLoop();

      status = Deactivate(*director);
      director.reset();

      OnShutdown();

      return status;
    }
    catch (std::exception& ex)
    {
      SRX_ERROR("Applicatoin critical error: {}", ex.what());
      return SRX_STATUS_MSG(EStatusCode::Interrupted, ex.what());
    }
  }
}
