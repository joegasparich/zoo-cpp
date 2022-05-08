#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 texCoord;

out vec3 v_TexCoord;

uniform mat4 u_MVP;

void main(){
    gl_Position = u_MVP * vec4(position.xyz, 1.0);
    v_TexCoord = texCoord;
}