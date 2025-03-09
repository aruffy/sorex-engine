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

#include <Sorex/Graphics/SxRenderDevice.h>
#include <Sorex/Graphics/SxVertex.h>

#include "GLTypes.h"
#include "GLResourceToken.h"
#include "GLShader.h"
#include "GLShaderProgram.h"
#include "GLUniform.h"
#include "GLRenderContext.h"
#include "GLVertexArray.h"
#include "GLExtensions.h"
#include "GLTexture2D.h"

namespace Sorex::Graphics
{
  class GLRenderDevice final: public RenderDevice
  {
    SRX_RTTI(Graphics::GLRenderDevice, Graphics::RenderDevice);

public:
    GLRenderDevice() SRX_NOEXCEPT;
    virtual ~GLRenderDevice() override;

    GLRenderDevice(const GLRenderDevice& other)            = delete;
    GLRenderDevice& operator=(const GLRenderDevice& other) = delete;

    // Interface Director::Component
    virtual Status Initialize() override;
    virtual void   Shutdown() override;

    // Interface RenderDevice
    virtual void                      Cleanup() override;
    virtual TUniquePointer<Texture2D> CreateTexture2D(StringView name) override;
    Status InitializeTexture(const GLTexture2D& texture, bool bMinmaps = false);

    /**
     * @brief Allocate new OpenGL resource.
     *
     * @param type - type of resource
     * @return resource token
     */
    GLResourceToken Allocate(GLResourceType type) SRX_NOEXCEPT;

    /**
     * @brief Deallocate OpenGL resource.
     *
     * @param glResource - resource reference
     */
    void Deallocate(GLResourceReference* glResource) SRX_NOEXCEPT;

    GLExtensions* GetExtensions() const { return mExtensions.get(); }

    template<typename VertexType, typename IndexType>
    Status Draw(const GLVertexArray<VertexType, IndexType>& vtxArray)
      SRX_NOEXCEPT;

    GLShaderPtr GetOrCreateShader(const GLShaderSource& shaderSource)
      SRX_NOEXCEPT;

    Status BuildShaderProgram(const GLShaderProgram& shaderProgram,
                              TVector<GLUniform>&    uniforms) SRX_NOEXCEPT;
    Status ApplyRenderTechnique(const GLRenderTechnique& technique)
      SRX_NOEXCEPT;

protected:
    virtual Renderer* CreateRenderer(const RuntimeClass& cls,
                                     ssize_t capacity) SRX_NOEXCEPT override;

private:
    Status LoadUniforms(GLResource&         program,
                        TVector<GLUniform>& uniforms) SRX_NOEXCEPT;

    GLResource* GetResource(
      const GLResourceReference* glResourceReference) const SRX_NOEXCEPT;
    bool IsValidResourceReference(const GLResourceReference* glResource) const
      SRX_NOEXCEPT;
    void DeallocateResource(GLResource& resource) SRX_NOEXCEPT;

    Status CompileShader(const GLShaderPtr& shader,
                         GLuint&            shaderId) const SRX_NOEXCEPT;

    template<typename VertexType, typename IndexType>
    Status BindVertexArray(
      const GLVertexArray<VertexType, IndexType>& vtxArray) const SRX_NOEXCEPT;

    Status ActivateShaderProgram(GLenum& mode) SRX_NOEXCEPT;

    static Status InitializeBuffer(GLResource&         resource,
                                   const GLBufferData& buffer) SRX_NOEXCEPT;
    static Status UpdateBufferData(const GLResource&   resource,
                                   const GLBufferData& data) SRX_NOEXCEPT;
    static void   EnableVertexAttributes(const VertexLayout& vtxLayout)
      SRX_NOEXCEPT;

#ifdef SOREX_OPENGL_DEBUG_OUTPUT
    static bool EnableDebugOutput(GLRenderDevice& glRenderDevice) SRX_NOEXCEPT;
#endif

private:
    TLinkedList<GLResource>       mResources;
    THashMap<hash_t, GLShaderPtr> mShaders;

    TUniquePointer<GLRenderContext> mRenderContext;
    GLShaderProgram*                mActiveShaderProgram;

    TUniquePointer<GLExtensions> mExtensions;
  };

  template<typename IndexType>
  constexpr GLenum ConvertIndexType() SRX_NOEXCEPT
  {
    if constexpr (std::is_same_v<IndexType, GLbyte>)
      return GL_UNSIGNED_BYTE;
    else if constexpr (std::is_same_v<IndexType, GLushort>)
      return GL_UNSIGNED_SHORT;
    else if constexpr (std::is_same_v<IndexType, GLuint>)
      return GL_UNSIGNED_INT;

    SRX_NOENTRY("Invalid Index Type");
    return GL_NONE;
  }

  template<typename VertexType, typename IndexType>
  Status GLRenderDevice::Draw(
    const GLVertexArray<VertexType, IndexType>& vtxArray) SRX_NOEXCEPT
  {
    GLenum mode;
    Status status = ActivateShaderProgram(mode);
    if (!status.Ok())
      return status;

    status = BindVertexArray(vtxArray);
    if (!status.Ok())
      return status;

    if (const GLIndexBuffer<IndexType>* indexBuffer = vtxArray.GetIndexBuffer())
    {
      SRX_OPENGL_CALL(
        glDrawElements(mode,
                       static_cast<GLsizei>(indexBuffer->GetSize()),
                       ConvertIndexType<IndexType>(),
                       (const GLvoid*)0));
    }
    else if (const GLVertexBuffer<VertexType>* vtxBuffer =
               vtxArray.GetVertexBuffer())
    {
      SRX_OPENGL_CALL(
        glDrawArrays(mode, 0, static_cast<GLsizei>(vtxBuffer->GetSize())));
    }

    // @FIXME:
#ifdef RUFFY_GAME_DEVELOPMENT
    _stats.drawCalls.Increase();
#endif

    SRX_OPENGL_CALL(glBindVertexArray(0));
    return status;
  }

  template<typename VertexType, typename IndexType>
  Status GLRenderDevice::BindVertexArray(
    const GLVertexArray<VertexType, IndexType>& vtxArray) const SRX_NOEXCEPT
  {
    GLResource* vao = GetResource(vtxArray.GetResourceToken());
    const GLVertexBuffer<VertexType>* vtxBuffer = vtxArray.GetVertexBuffer();
    if (vao == nullptr || vtxBuffer == nullptr)
      return SRX_STATUS_MSG(
        EStatusCode::Invalid_Argument,
        "{} has invalid state or expired resource reference",
        ToString(GLResourceType::VertexArray));

    SRX_OPENGL_CALL(glBindVertexArray(vao->id));
    GLResource* vbo = GetResource(vtxBuffer->GetResourceToken());
    if (vbo == nullptr)
      return SRX_STATUS_MSG(EStatusCode::Invalid_State,
                            "{} has expired resource reference",
                            ToString(GLResourceType::VertexBuffer));

    // TODO: if not vao->inited
    Status status;
    SRX_OPENGL_CALL(glBindBuffer(vbo->target, vbo->id));
    if (vbo->inited == false)
    {
      status = InitializeBuffer(*vbo, vtxBuffer->GetData());
      if (!status.Ok())
        return status;

      EnableVertexAttributes(vtxBuffer->GetVertexLayout());
    }

    UpdateBufferData(*vbo, vtxBuffer->GetData());

    if (const GLIndexBuffer<IndexType>* indxBuffer = vtxArray.GetIndexBuffer())
    {
      GLResource* ebo = GetResource(indxBuffer->GetResourceToken());
      if (ebo == nullptr)
        return SRX_STATUS_MSG(EStatusCode::Invalid_State,
                              "Index buffer has expired resource reference");

      SRX_OPENGL_CALL(glBindBuffer(ebo->target, ebo->id));
      if (ebo->inited == false)
      {
        status = InitializeBuffer(*ebo, indxBuffer->GetData());
        if (!status.Ok())
          return status;
      }

      UpdateBufferData(*ebo, indxBuffer->GetData());
    }

    return status;
  }
}  // namespace
