#version 450 core

in vec2 texCoord;

layout(location = 0) out vec4 outColor;

layout (location = 10) uniform vec3 ambient;
layout (location = 11) uniform float Specularity;
layout (location = 20) uniform sampler2D DiffuseTex;
layout (location = 21) uniform sampler2D NormalTex;

void main()
{
    outColor = vec4(ambient, 1.0) * texture(DiffuseTex, texCoord);
}
