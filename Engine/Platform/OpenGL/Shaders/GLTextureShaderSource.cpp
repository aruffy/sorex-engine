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

#include <GLShader.h>

using namespace Sorex::Graphics;

namespace
{
  const GLString __kTextureVertexShaderSource = R"(
        #version 330 core

        in mediump vec2 a_position;
        in mediump vec2 a_texcoord;

        in mediump vec4 a_color;

        uniform mediump mat4 u_mvp;
        uniform mediump vec2 u_scale_tex_0;

        out lowp vec4 v_color;
        out mediump vec2 v_texcoord;

        void main()
        {
            // convert texture coordinates from ST space to UV.
            v_texcoord = a_texcoord * u_scale_tex_0;

            v_color = a_color;
            gl_Position = u_mvp * vec4(a_position.x, a_position.y, 0.f, 1.f);
        }
    )";


  const GLString __kTextureFragmentShaderSource = R"(
        #version 330 core

        in lowp vec4 v_color;
        in mediump vec2 v_texcoord;

        uniform sampler2D u_sampler_0;

        out lowp vec4 out_color;

        void main()
        {
            out_color = texture(u_sampler_0, v_texcoord) * v_color;
        }
    )";
}

namespace Sorex::Graphics::OpenGL
{
  const GLShaderSource Shader::kTextureVertexShaderSource =
    GLShaderSource{ EShaderType::Vertex, __kTextureVertexShaderSource };

  const GLShaderSource Shader::kTextureFragmentShaderSource =
    GLShaderSource{ EShaderType::Fragment, __kTextureFragmentShaderSource };
}  // namespace
