/* #include <Graphics/OpenGL/GLShaderSource.h>

namespace
{
    const Ruffy::String __kTextureVertexShaderSource = R"(
        #version 330 core

        in mediump vec2 a_position;
        in mediump vec2 a_texcoord;
        in lowp vec4 a_color;

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


    const Ruffy::String __kTextureFragmentShaderSource = R"(
        #version 330 core

        in lowp vec4 v_color;
        in mediump vec2 v_texcoord;

        uniform sampler2D u_sampler_0;

        out lowp vec4 out_color;

        void main()
        {
            out_color = texture2D(u_sampler_0, v_texcoord) * v_color;
        }
    )";
}

namespace Ruffy::Graphics
{
    const GLShaderSource::Instance GLShaderSource::kTextureVertexShaderSource{
        GLShaderSource::GetHash("kTextureVertexShaderSource"),
        EShaderType::Vertex_Shader,
        __kTextureVertexShaderSource
    };


    const GLShaderSource::Instance GLShaderSource::kTextureFragmentShaderSource{
GLShaderSource::GetHash("kTextureFragmentShaderSource"),
  EShaderType::Fragment_Shader, __kTextureFragmentShaderSource
}
;
}  // namespace
*/
