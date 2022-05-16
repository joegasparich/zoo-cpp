#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_TexCoord;
in vec3 v_Colour;

uniform sampler2DArray u_Textures;

void main(){
    vec4 texColor = texture(u_Textures, vec3(v_TexCoord));

    if (texColor.a <= 0.0) {
         discard;
    }

    color = vec4(v_Colour, 1.0);
}
