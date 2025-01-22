/* #include <Graphics/OpenGL/GLShaderSource.h>

namespace
{
    const Ruffy::String __kBitmapTextFragmentShaderSource = R"(
        #version 330

        in lowp vec4 v_color;
        in mediump vec2 v_texcoord;

        uniform sampler2D u_sampler_0;

        out lowp vec4 out_color;

        void main()
        {
            vec4 color_alpha = vec4(1.f, 1.f, 1.f, texture(u_sampler_0,
v_texcoord).r); out_color = v_color * color_alpha;
        }
    )";

    const Ruffy::String __kSignedDistanceFieldFragmentShaderSource = R"(
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

            float alpha = smoothstep(u_onedge - width, u_onedge + width,
distance); vec4 color = v_color;

            if (u_outline >= 0.f) {
                color = mix(u_outline_color, v_color, alpha);
                alpha = smoothstep(u_outline - width, u_outline + width,
distance);
            }

            out_color = vec4(color.rgb, color.a * alpha);
        }
    )";
}
*/
namespace Ruffy::Graphics
{
  const GLShaderSource::Instance
    GLShaderSource::kBitmapTextFragmentShaderSource{
      GLShaderSource::GetHash("kBitmapTextFragmentShaderSource"),
      EShaderType::Fragment_Shader,
      __kBitmapTextFragmentShaderSource
    };

  const GLShaderSource::Instance
    GLShaderSource::kSignedDistanceFieldFragmentShaderSource{
      GLShaderSource::GetHash("kSignedDistanceFieldFragmentShaderSource"),
      EShaderType::Fragment_Shader,
      __kSignedDistanceFieldFragmentShaderSource
    };
}
