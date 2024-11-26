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

#include <Sorex/CoreMinimal.h>
#include <Sorex/Director.h>

namespace Sorex::Platform
{
  /**
   * Class that based on Graphics Library Framework (glfw).
   * Create window and context for OpenGL.
   */
  class DesktopGraphicsFramework final: private Director::Component
  // , private Director::IListener
  {
    SRX_RTTI(Platform::DesktopGraphicsFramework, Director::Component)

public:
    static DesktopGraphicsFramework& GetInstance() SRX_NOEXCEPT;
    virtual ~DesktopGraphicsFramework() override {}

    DesktopGraphicsFramework(const DesktopGraphicsFramework& other) = delete;
    DesktopGraphicsFramework& operator=(const DesktopGraphicsFramework& other) =
      delete;

    virtual Status Initialize() override;
    // InputEventDispatcher Interface
    // virtual bool AddEventHandler(InputEventHandler* handler) override;
    // virtual void RemoveEventHandler(InputEventHandler* handler) override;

    /**
     * @brief Check if graphics framework is already initialized.
     *
     * @return True if Initialization has already been successfully done.
     */
    // inline bool IsInitialized() const { return _isInitialized; }

    /**
     * @brief Create new glfw window object.
     *
     * @param title - title of the window
     * @param size - desired window size
     * @param owner - real holder of glfw window object, will be associated with
     * window.
     * @param error - store error description if creation isn't done
     * successfully
     * @return The pointer to glfw window object or NULL if creation was't
     * successfully
     */
    /* GLFWwindow* CreateWindow(const WString& title,
                             const SizeInt& size,
                             Window*        owner,
                             Error*         error); */

    /**
     * @brief Destroy glfw window object.
     *
     * @param window - the window object pointer that will be destroyed
     */
    // void DestroyWindow(GLFWwindow* window);

    /**
     * @brief Retrieve actual window size.
     *
     * @param window - the pointer to glfw window object.
     * @param outSize - out paremeter where to store the size of window
     * @return Error::Ok if size saved in outSize parameter, elese error code
     */
    // int GetWindowSize(GLFWwindow* window, SizeInt& outSize) const;

    /**
     * @brief Set the size of window.
     *
     * @param window - the pointer to glfw window object
     * @param size - desired window size
     * @return Error::Ok success, else error code
     */
    // int SetWindowSize(GLFWwindow* window, const SizeInt& size);

    /**
     * @brief Set close flag to the window state.
     *
     * @param window - pointer to target glfw windows object
     * @param bClose - value of close flag.
     */
    // void SetCloseWindonFlag(GLFWwindow* window, bool bClose = true);

    /**
     * @brief Check if button are being pressed. Return the cached value of
     * button state.
     *
     * @note: checking uses main application window;
     *
     * @return True if key is being pressed;
     */
    // bool IsKeyDown(int key) const;

protected:
    // static void OnErrorCallback(int errorId, const char* errorDesc);

    /* Mouse Events */
    /* static void OnMouseClickCallback(GLFWwindow* window,
                                     int         button,
                                     int         action,
                                     int         modify); */
    // static void OnMouseMoveCallback(GLFWwindow* window, double x, double y);
    // static void OnMouseScrollCallback(GLFWwindow* window, double x, double
    // y);

    /* Keyboard Events */
    /* static void OnKeyCallback(GLFWwindow* window,
                              int         key,
                              int         scancode,
                              int         action,
                              int         mods); */
    // static void OnCharCallback(GLFWwindow* window, unsigned int character);

    /* Window Events */
    // static void OnWindowFocusCallback(GLFWwindow* window, int focused);
    // static void OnWindowPositionChanged(GLFWwindow* windows, int x, int y);
    // static void OnWindowSizeChanged(GLFWwindow* window, int width, int
    // height); static void OnWindowIconifyCallback(GLFWwindow* window, int
    // iconified);

    /* Framebuffer Events */
    // static void OnFramebufferSizeChanged(GLFWwindow* window, int w, int h);

private:
    // Singleton
    DesktopGraphicsFramework() = default;

    /**
     * @brief Start listen app main loop.
     *
     * Can listen only one app loop.
     * When is listening events from the loop perform pulling and emiting user
     * input events; If the main application window will be closed the graphics
     * framework send exit request;
     *
     * @param loop - new app loop to listen.
     */
    // void ListenApplicationLoop(ApplicationLoop* loop);

    // ApplicationLoop Listener Interface
    // virtual void OnUpdate(float deltaTime) override;
    // virtual void OnFinishFrame() override;
    // virtual void OnExit() override;

    // void RequestApplicationExit() const;

    // Events
    // void EmitMouseEvent(GLFWwindow* window, const MouseEvent& event);
    // void EmitKeyboardEvent(GLFWwindow* window, const KeyboardEvent& event);

private:
    // Graphics::GLContextAttributes _glCtxAttribs;
    bool mIsInitialized = false;
    // GLFWwindow*                   _appMainWindow = nullptr;

    // ApplicationLoop* _appLoop = nullptr;

    // Event Dispatcher
    // using HandlerList = TListenerContainer<InputEventHandler>;
    // HandlerList _handlers;

    // Input events
    // TUniquePointer<MouseEvent>    _mouseEvent;
    // TUniquePointer<KeyboardEvent> _keyboardEvent;
  };
}
