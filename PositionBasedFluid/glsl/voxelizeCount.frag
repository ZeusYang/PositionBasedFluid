#version 430 core

in vec3 FragPos;

layout (std430, binding = 0) buffer CountBuffer{
    int cnts[];
};

uniform float step;
uniform vec3 boxMin;
uniform vec3 resolution;

out vec4 color;

void main(){
	int x = int((FragPos.x - boxMin.x)/step);
	int y = int((FragPos.y - boxMin.y)/step);
	int z = int((FragPos.z - boxMin.z)/step);
	int index = int(y * (resolution.z * resolution.x) + z * resolution.x + x);
	atomicAdd(cnts[index], 1);
	//cnts[index] = 1;
	color = vec4(0.0,1.0,0.0,1.0);
}