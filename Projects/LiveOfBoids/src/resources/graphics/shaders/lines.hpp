#pragma once

namespace lines {
    static const char* const vertex_shader_text = R"#(
#version 330 core
uniform mat3 transform;
in vec2 vPos;
in vec3 vCol; // Specify a vertex attribute for color
out vec3 color;
void main()
{
    gl_Position = vec4(transform * vec3(vPos, 1.0), 1.0);
	color = vCol; // pass the color along to the fragment shader
}
)#";

    static const char* const fragment_shader_text = R"#(
#version 330 core

in vec3 color;
out vec4 fragColor;
void main() {
    // Set the fragment color to the color passed from the vertex shader
    fragColor = vec4(color, 1.0);
}
)#";
}  // namespace points
