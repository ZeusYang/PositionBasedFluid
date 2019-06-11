#version 430 core

in vec3 FragPos;
in vec3 ProjectPos;
in vec4 BoundingBox;

layout (std430, binding = 0) buffer CountBuffer{
    int cnts[];
};

uniform bool conservate;
uniform float step;
uniform vec3 boxMin;
uniform vec3 resolution;

out vec4 color;

void main(){
	if(ProjectPos.x < BoundingBox.x || ProjectPos.y < BoundingBox.y || ProjectPos.x > BoundingBox.z || ProjectPos.y > BoundingBox.w)
		discard;

	uint x = uint((FragPos.x - boxMin.x)/step);
	uint y = uint((FragPos.y - boxMin.y)/step);
	uint z = uint((FragPos.z - boxMin.z)/step);
	uint index = uint(y * (resolution.z * resolution.x) + z * resolution.x + x);
	atomicAdd(cnts[index], 1);
	color = vec4(0.0,1.0,0.0,1.0);
}