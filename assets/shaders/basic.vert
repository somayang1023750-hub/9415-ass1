#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in float aBrightness;
layout(location = 3) in vec3 aColorModifier;

uniform mat4 uViewProj;
uniform mat4 uModel;

out vec3 vWorldPosition;
out vec2 vTexCoord;
out float vBrightness;
out vec3 vColorModifier;

void main() {
    vTexCoord = aTexCoord;
    vBrightness = aBrightness;
    vColorModifier = aColorModifier;
    vec4 worldPosition = uModel * vec4(aPosition, 1.0);
    vWorldPosition = worldPosition.xyz;
    gl_Position = uViewProj * worldPosition;
}
