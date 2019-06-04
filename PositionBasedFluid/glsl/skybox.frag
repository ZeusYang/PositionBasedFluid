#version 330 core
in vec3 Texcoord;
in vec3 Normal;
in vec3 Color;

uniform samplerCube image;

out vec4 color;

void main(){
	color = texture(image, Texcoord);
}