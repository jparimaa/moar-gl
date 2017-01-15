layout(location = 0) out vec3 outColor;

layout (location = 30) uniform sampler2D positionTex;
const int KERNEL_SIZE = 64;
layout (location = 45) uniform mat4 projection;
layout (location = 46) uniform vec3 kernel[KERNEL_SIZE];

in vec2 texCoord;

const float RADIUS = 0.1;
const float BIAS = 0.02;

void main()
{
  vec3 pos = texture(positionTex, texCoord).xyz;
  float occlusion = 0.0;

  for (int i = 0; i < KERNEL_SIZE; i++) {
    vec3 samplePos = pos + kernel[i];
    vec4 offset = vec4(samplePos, 1.0);
    offset = projection * offset;
    offset.xy /= offset.w;
    offset.xy = offset.xy * 0.5 + vec2(0.5);

    float sampleDepth = texture(positionTex, offset.xy).z;
     if (abs(pos.z - sampleDepth) < RADIUS) {
    //   occlusion += sampleDepth > samplePos.z + BIAS ? 1.0 : 0.0;
       occlusion += step(samplePos.z, sampleDepth);
    }

    // float rangeCheck = smoothstep(0.0, 1.0, RADIUS / abs(pos.z - sampleDepth));
    // occlusion += (sampleDepth >= samplePos.z + BIAS ? 1.0 : 0.0) * rangeCheck;
  }

  occlusion = 1.0 - (occlusion / KERNEL_SIZE);
  outColor = vec3(occlusion);
}