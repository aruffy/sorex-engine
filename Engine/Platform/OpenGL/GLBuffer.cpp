#include "GLBuffer.h"

#include "GLRenderDevice.h"

namespace Ruffy::Graphics
{
  namespace OpenGL
  {
    Buffer::Buffer(GLRenderDevice* glDevice, GLResourceType type)
      : _type(type)
    {
      RFY_CHECK(type == GLResourceType::VertexBuffer
                || type == GLResourceType::IndexBuffer);
      _glToken = glDevice ? glDevice->Allocate(type) : nullptr;

      RFY_CHECK_MSG(_glToken, "allocation buffer failed");
    }
  };
}
