#version 450 core

in vec3 lightDir_Tan;
in vec2 texCoord;

layout(location = 0) out vec4 outColor;

layout (location = 20) uniform sampler2D DiffuseTex;
layout (location = 21) uniform sampler2D NormalTex;
layout (location = 23) uniform sampler2D DepthTex;

layout (std140) uniform LightBlock {
    vec4 lightColor;
    vec3 lightPos;
    vec3 lightForward;
};

void main()
{
    vec3 normal_Tan = normalize(texture(NormalTex, texCoord).rgb * 2.0 - vec3(1.0));

    float diff = clamp(dot(normal_Tan, lightDir_Tan), 0, 1);

    outColor = vec4(lightColor.xyz * lightColor.w * diff, 1.0) * texture(DiffuseTex, texCoord);
}
