#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;
layout (location = 3) in vec3 color;

out vec2 Texcoord;
out vec3 Normal;
out vec3 Color;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectMatrix;

void main(){
	Texcoord = texcoord;
	Normal = normal;
	Color = color;
	gl_Position = projectMatrix * viewMatrix * modelMatrix * vec4(position,1.0f);
}