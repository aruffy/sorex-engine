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

#include "GLRenderDevice.h"

using namespace Sorex::Graphics;
#if defined(GL_DEBUG_OUTPUT)
namespace
{
  Sorex::ELogLevel GetLogLevel(const GLenum type) SRX_NOEXCEPT
  {
    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      return Sorex::ELogLevel::Error;

    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    case GL_DEBUG_TYPE_PORTABILITY:
    case GL_DEBUG_TYPE_PERFORMANCE:
      return Sorex::ELogLevel::Warning;

    case GL_DEBUG_TYPE_PUSH_GROUP:
    case GL_DEBUG_TYPE_POP_GROUP:
      return Sorex::ELogLevel::Info;

    case GL_DEBUG_TYPE_OTHER:
      return Sorex::ELogLevel::Debug;

    default:
      return Sorex::ELogLevel::Error;
    }
  }

  const char* TranslateMessageType(const GLenum type) SRX_NOEXCEPT
  {
    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
      return "error";
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      return "deprecated behavior";
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      return "undefined behavior";
    case GL_DEBUG_TYPE_PORTABILITY:
      return "portability issue";
    case GL_DEBUG_TYPE_PERFORMANCE:
      return "performance issue";
    case GL_DEBUG_TYPE_PUSH_GROUP:
      return "group {";
    case GL_DEBUG_TYPE_POP_GROUP:
      return "}";
    case GL_DEBUG_TYPE_OTHER:
      [[fallthrough]];
    default:
      return "other";
    }
  }

  const char* TranslateMessageSeverity(const GLenum severity)
  {
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
      return "serious";
    case GL_DEBUG_SEVERITY_MEDIUM:
      return "medium";
    case GL_DEBUG_SEVERITY_LOW:
      return "minor";
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      return "notification";
    default:
      return "error";
    }
  }

  const char* TranslateMessageSource(const GLenum source)
  {
    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
      return "API";
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      return "window system";
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      return "shader compiler";
    case GL_DEBUG_SOURCE_THIRD_PARTY:
      return "third-party";
    case GL_DEBUG_SOURCE_APPLICATION:
      return "application";
    case GL_DEBUG_SOURCE_OTHER:
      [[fallthrough]];
    default:
      return "other";
    }
  }

  void OnGraphicsDebugOutput(GLenum        source,
                             GLenum        type,
                             GLuint        id,
                             GLenum        severity,
                             GLsizei       length,
                             const GLchar* message,
                             const void*   userParam)
  {
    constexpr Sorex::TArray<GLuint, 4> kIgnoredCallbacks{ 131169,
                                                          131185,
                                                          131218,
                                                          131204 };

    if (std::find(kIgnoredCallbacks.cbegin(), kIgnoredCallbacks.cend(), id)
        != kIgnoredCallbacks.cend())
      return;

    SRX_ENGINE_LOG(GetLogLevel(type),
                   "[OpenGL @{}] {} from {}: \"{}\"\n",
                   TranslateMessageSeverity(severity),
                   TranslateMessageType(type),
                   TranslateMessageSource(source),
                   message);

    SRX_CHECK(severity == GL_DEBUG_SEVERITY_LOW
              || severity == GL_DEBUG_SEVERITY_NOTIFICATION);
  }
}

bool GLRenderDevice::EnableDebugOutput(GLRenderDevice& glRenderDevice)
  SRX_NOEXCEPT
{
  bool bHasDebugOutputExtension = false;
  if (const GLExtensions* extensions = glRenderDevice.GetExtensions())
    bHasDebugOutputExtension = extensions->HasExtension("GL_KHR_debug");

  const bool bDebugOutput = bHasDebugOutputExtension && GLAD_GL_KHR_debug;
  if (!bDebugOutput)
  {
    SRX_NOENTRY("[OpenGL] Debug output isn't supported");
    return false;
  }

  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

  glDebugMessageCallback(OnGraphicsDebugOutput, &glRenderDevice);
  glDebugMessageControl(GL_DONT_CARE,
                        GL_DONT_CARE,
                        GL_DEBUG_SEVERITY_NOTIFICATION,
                        0,
                        nullptr,
                        GL_TRUE);

  // Show test message
  constexpr GLchar helloMessage[] = "debug callback";
  constexpr GLuint messageId      = 0xfeed;
  glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION,
                       GL_DEBUG_TYPE_OTHER,
                       messageId,
                       GL_DEBUG_SEVERITY_NOTIFICATION,
                       sizeof(helloMessage) / sizeof(GLchar),
                       helloMessage);

  return true;
}

#else
bool GLRenderDevice::EnableDebugOutput(GLRenderDevice& glRenderDevice)
  SRX_NOEXCEPT
{
  return false;
}
#endif
