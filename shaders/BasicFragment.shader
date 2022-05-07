#version 330 core

layout(location = 0) out vec4 color;

uniform vec4 u_Color;

void main(){
    if (u_Color.a <= 0.0) {
         discard;
    }

    color = vec4(u_Color);
}
