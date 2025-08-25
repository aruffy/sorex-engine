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
  const Sorex::String __kBitmapFontFragmentShaderSource = R"(
        #version 330

        in lowp vec4 v_color;
        in mediump vec2 v_texcoord;

        uniform sampler2D u_sampler_0;

        out lowp vec4 out_color;

        void main()
        {
            vec4 color_alpha = vec4(1.f, 1.f, 1.f, texture(u_sampler_0, v_texcoord).r);
            out_color = v_color * color_alpha;
        }
    )";

  const Sorex::String __kSignedDistanceFieldFragmentShaderSource = R"(
        #version 330

        in lowp vec4 v_color;
        in mediump vec2 v_texcoord;

        uniform sampler2D u_sampler_0;

        uniform lowp float u_onedge = 0.5f;
        uniform mediump float u_smoothing = 0.0f;

        uniform float u_outline;
        uniform vec4  u_outline_color;

        out lowp vec4 out_color;

        void main()
        {
            float distance = texture(u_sampler_0, v_texcoord).r;
            float width = u_smoothing == 0.f ? fwidth(distance) : u_smoothing;

            float alpha = smoothstep(u_onedge - width, u_onedge + width, distance);
            vec4 color = v_color;

            if (u_outline >= 0.f) {
                color = mix(u_outline_color, v_color, alpha);
                alpha = smoothstep(u_outline - width, u_outline + width, distance);
            }

            out_color = vec4(color.rgb, color.a * alpha);
        }
    )";
}

namespace Sorex::Graphics::OpenGL
{
  const GLShaderSource Shader::kFontBitmapFragmentShaderSource =
    GLShaderSource{ EShaderType::Fragment, __kBitmapFontFragmentShaderSource };

  const GLShaderSource Shader::kSignedDistanceFieldFragmentShaderSource =
    GLShaderSource{ EShaderType::Fragment,
                    __kSignedDistanceFieldFragmentShaderSource };
}  // namespace
