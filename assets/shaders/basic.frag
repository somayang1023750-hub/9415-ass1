#version 330 core

// The rasterizer stores this output color in the framebuffer.
out vec4 fragColor;

in vec3 vWorldPosition;
in vec2 vTexCoord;
in float vBrightness;
in vec3 vColorModifier;

uniform sampler2D uTexture;
uniform vec3 uTint;
uniform int uFogEnabled;
uniform vec3 uCameraPosition;
uniform vec3 uFogColor;
uniform float uFogStart;
uniform float uFogEnd;

void main() {
    vec4 texColor = texture(uTexture, vTexCoord);
    // Alpha testing keeps leaves/flowers sharp and writes depth only for solid
    // texels, so these cutouts need no transparent-object sorting or blending.
    if (texColor.a < 0.5) discard;
    vec3 surface = texColor.rgb * vColorModifier * vBrightness * uTint;
    float distanceToEye = length(vWorldPosition - uCameraPosition);
    float fog = uFogEnabled != 0 ? smoothstep(uFogStart, uFogEnd, distanceToEye) : 0.0;
    fragColor = vec4(mix(surface, uFogColor, fog), 1.0);
}
