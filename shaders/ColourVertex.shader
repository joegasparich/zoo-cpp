#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 color;

out vec4 v_Color;

uniform mat4 u_MVP;

void main(){
    gl_Position = u_MVP * vec4(position.x, position.y, 0.0, 1.0);
    v_Color = color;
}
