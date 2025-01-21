#pragma once

#include <Sorex/SxCoreMinimal.h>
#include <Sorex/Graphics/SxRenderer.h>
#include <Sorex/Graphics/SxVertex.h>

#include "GLVertexArray.h"
#include "GLShaderProgram.h"

namespace Sorex::Graphics
{
  class GLRenderDevice;
  class GLPrimitiveRenderer: public PrimitiveRenderer
  {
    using VertexType   = Vertex::V2F_C4B;
    using VertexArray  = GLVertexArray<VertexType>;
    using VertexBuffer = typename VertexArray::VertexBuffer;

public:
    explicit GLPrimitiveRenderer(GLRenderDevice* glRenderDevice,
                                 size_t          vtxCapacity = 1024u);

    // API Render
    virtual Status Initialize() SRX_NOEXCEPT override;
    virtual Status Activate() SRX_NOEXCEPT override;
    virtual void   Flush() SRX_NOEXCEPT override;
    virtual void   Reset() SRX_NOEXCEPT override;
    virtual bool   IsEmpty() const SRX_NOEXCEPT override
    {
      return mVtxArray.IsEmpty();
    }

    // API PrimitiveRenderer
    virtual void DrawLine(Point        begin,
                          Point        end,
                          const Color* color,
                          size_t       colorNumber) SRX_NOEXCEPT override;
    virtual void DrawBorder(const Rectangle& rectangle,
                            const Color*     color,
                            size_t           colorNumber) SRX_NOEXCEPT override;
    virtual void DrawRectangle(const Rectangle& rectangle,
                               const Color*     color,
                               size_t colorNumber) SRX_NOEXCEPT override;
    virtual void DrawCircle(Point center,
                            float radius,
                            int32 segments,
                            Color color) SRX_NOEXCEPT override;

private:
    Vertex::V2F_C4B* AllocateVertices(size_t vtxNumber) SRX_NOEXCEPT;
    void             SwitchRenderingMode(ERenderingMode mode);

    void DrawBorder(const Color* color, size_t colorNumber);
    void DrawRectangle(const Color* color, size_t colorNumber);

private:
    GLRenderDevice* mRenderDevice;
    // const CanvasState* mCanvasState;

    TArray<Point, 4> mPoints;
    // GLRenderTechnique mTechnique;

    VertexArray mVtxArray;
    size_t      mVtxCapacity;

    TUniquePointer<GLShaderProgram> mShaderProgram;
  };
}
