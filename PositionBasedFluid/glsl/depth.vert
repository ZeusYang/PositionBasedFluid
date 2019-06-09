#version 330 core
layout (location = 0) in vec3 position;
layout (location = 4) in mat4 instanceMatrix;

uniform bool instance;
uniform mat4 lightSpaceMatrix;
uniform mat4 modelMatrix;

void main()
{
	if(!instance)
    		gl_Position = lightSpaceMatrix * modelMatrix * vec4(position, 1.0f);
	else
 		gl_Position = lightSpaceMatrix * modelMatrix * instanceMatrix * vec4(position, 1.0f);
}