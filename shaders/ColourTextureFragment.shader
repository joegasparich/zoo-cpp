#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

uniform sampler2D u_Texture;
uniform vec3 u_Colour;

void main(){
    vec4 texColor = texture(u_Texture, v_TexCoord);

    if (texColor.a <= 0.0) {
         discard;
    }

    color = texColor * vec4(u_Colour, 1.0);
}
