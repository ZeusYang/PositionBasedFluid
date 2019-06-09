#version 430 core
layout (location = 0) in vec3 position;

out vec3 FragPos;

uniform mat4 viewMatrix;
uniform mat4 projectMatrix;

void main(){
	FragPos = position;
	gl_Position = vec4(position, 1.0f);
	//gl_Position = projectMatrix * viewMatrix * vec4(position,1.0f);
}