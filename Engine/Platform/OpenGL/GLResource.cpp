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

#include "GLResource.h"

namespace Sorex::Graphics
{

  class GLRenderDevice
  {
public:
    GLResourceToken Allocate(GLResourceType) { return nullptr; }
    void            Deallocate(GLResourceReference* ref) {}
  };

  GLResourceReference::GLResourceReference(GLRenderDevice* glRenderDevice,
                                           size_t          rid)
    : _id(rid)
    , _glDevice(glRenderDevice)
  {}

  GLResourceReference::~GLResourceReference()
  {
    if (IsValid())
      _glDevice->Deallocate(this);
  }

  bool GLResourceReference::IsValid() const
  {
    return (_glDevice && _id != kInvalidReferenceId);
  }

  void GLResourceReference::OnExpired()
  {
    _glDevice = nullptr;
    _id       = kInvalidReferenceId;
  }

  void GLResource::Reset()
  {
    id       = kInvalidResourceId;
    token    = nullptr;
    value    = 0u;
    target   = 0u;
    type     = GLResourceType::Idle;
    isInited = false;
  }

  RFY_NODISCARD GLResourceToken AllocateResource(GLRenderDevice* glRenderDevice,
                                                 GLResourceType  type)
  {
    return glRenderDevice ? glRenderDevice->Allocate(type) : nullptr;
  }
}
