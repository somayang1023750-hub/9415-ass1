#version 330 core

// The rasterizer stores this output color in the framebuffer.
out vec4 fragColor;

in vec2 vTexCoord;
in float vBrightness;
in vec3 vColorModifier;

uniform sampler2D uTexture;
uniform vec3 uTint;

void main() {
    vec4 texColor = texture(uTexture, vTexCoord);
    fragColor = vec4(texColor.rgb * vColorModifier * vBrightness * uTint, 1.0);
}