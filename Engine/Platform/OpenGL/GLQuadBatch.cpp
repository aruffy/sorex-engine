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

#include "GLQuadBatch.h"
#include "GLRenderDevice.h"

namespace Sorex::Graphics
{
  GLTexBatch::GLTexBatch(GLRenderDevice& glDevice, size_t capacity) SRX_NOEXCEPT
    : GLQuadBatch<VertexType>(glDevice, capacity)
  {}

  Status GLTexBatch::Flush()
  {
    if (!IsEmpty())
    {
      Status status = SRX_STATUS(EStatusCode::Invalid_State);
      if (auto glDevice = GetRenderDevice())
        status = glDevice->Draw(GetVertexArray());

      Clear();
      return status;
    }

    return SRX_OK;
  }

  void GLTexBatch::Draw(const TArray<Point, 4>& texcoord,
                        const TArray<Point, 4>& screenPoints,
                        Color                   color)
  {
    if (GetSize() >= GetCapacity())
      Flush();

    Quad quad;
    SRX_VERIFY(Allocate(quad).Ok());

    quad.tl->position[0] = screenPoints[0].x;
    quad.tl->position[1] = screenPoints[0].y;
    quad.tl->texCoord[0] = texcoord[0].x;
    quad.tl->texCoord[1] = texcoord[0].y;
    quad.tl->color       = color.value;

    quad.bl->position[0] = screenPoints[1].x;
    quad.bl->position[1] = screenPoints[1].y;
    quad.bl->texCoord[0] = texcoord[1].x;
    quad.bl->texCoord[1] = texcoord[1].y;
    quad.bl->color       = color.value;

    quad.br->position[0] = screenPoints[2].x;
    quad.br->position[1] = screenPoints[2].y;
    quad.br->texCoord[0] = texcoord[2].x;
    quad.br->texCoord[1] = texcoord[2].y;
    quad.br->color       = color.value;

    quad.tr->position[0] = screenPoints[3].x;
    quad.tr->position[1] = screenPoints[3].y;
    quad.tr->texCoord[0] = texcoord[3].x;
    quad.tr->texCoord[1] = texcoord[3].y;
    quad.tr->color       = color.value;
  }
}  // namespace
