#version 330 core
in vec2 Texcoord;
in vec3 Normal;
in vec3 Color;

out vec4 color;

void main(){
	color = vec4(Color,1.0f);
}