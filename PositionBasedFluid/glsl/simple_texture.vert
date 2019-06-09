#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;
layout (location = 3) in vec3 color;
layout (location = 4) in mat4 instanceMatrix;

out vec2 Texcoord;
out vec3 Normal;
out vec3 Color;

uniform bool instance;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectMatrix;

void main(){
	Texcoord = texcoord;
	Normal = normal;
	Color = color;
	vec3 pos;
	if(!instance)
		pos = vec3(modelMatrix * vec4(position,1.0f));
	else 
		pos = vec3(modelMatrix * instanceMatrix * vec4(position,1.0f));
	gl_Position = projectMatrix * viewMatrix * vec4(pos,1.0f);
}