#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 FragPos;
out vec3 ProjectPos;
out vec4 BoundingBox;

uniform vec2 halfPixel[3];
uniform mat4 viewProject[3];
uniform mat4 viewProjectInverse[3];

uint selectViewProject()
{
	vec3 p1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 p2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 faceNormal = cross(p1, p2);
	float nDX = abs(faceNormal.x);
	float nDY = abs(faceNormal.y);
	float nDZ = abs(faceNormal.z);

	if( nDX > nDY && nDX > nDZ )
    {
		return 0;
	}
	else if( nDY > nDX && nDY > nDZ  )
    {
	    return 1;
    }
	else
    {
	    return 2;
	}
} 

vec4 calcAABB(vec4 pos[3], vec2 pixelDiagonal)
{
	vec4 aabb;
	aabb.xy = min(pos[2].xy, min(pos[1].xy, pos[0].xy));
	aabb.zw = max(pos[2].xy, max(pos[1].xy, pos[0].xy));
	// enlarge by half-pixel
	aabb.xy -= pixelDiagonal;
	aabb.zw += pixelDiagonal;
	return aabb;
}

void main() {
	uint projectIndex = selectViewProject();
	vec4 pos[3] = vec4[3]
	(
		viewProject[projectIndex] * gl_in[0].gl_Position,
		viewProject[projectIndex] * gl_in[1].gl_Position,
		viewProject[projectIndex] * gl_in[2].gl_Position
	);

	vec4 trianglePlane;
	trianglePlane.xyz = normalize(cross(pos[1].xyz - pos[0].xyz, pos[2].xyz - pos[0].xyz));
	trianglePlane.w   = -dot(pos[0].xyz, trianglePlane.xyz);

    // change winding, otherwise there are artifacts for the back faces.
    if (dot(trianglePlane.xyz, vec3(0.0, 0.0, 1.0)) < 0.0)
    {
        vec4 vertexTemp = pos[2];
        pos[2] = pos[1];
        pos[1] = vertexTemp;
    }

	if(trianglePlane.z == 0.0f)
		return;

	BoundingBox = calcAABB(pos, halfPixel[projectIndex]);

	vec3 edgePlanes[3];
	edgePlanes[0] = cross(pos[0].xyw - pos[2].xyw, pos[2].xyw);
	edgePlanes[1] = cross(pos[1].xyw - pos[0].xyw, pos[0].xyw);
	edgePlanes[2] = cross(pos[2].xyw - pos[1].xyw, pos[1].xyw);
	edgePlanes[0].z -= dot(halfPixel[projectIndex], abs(edgePlanes[0].xy));
	edgePlanes[1].z -= dot(halfPixel[projectIndex], abs(edgePlanes[1].xy)); 
	edgePlanes[2].z -= dot(halfPixel[projectIndex], abs(edgePlanes[2].xy));

	vec3 intersection[3];
	intersection[0] = cross(edgePlanes[0], edgePlanes[1]);
	intersection[1] = cross(edgePlanes[1], edgePlanes[2]);
	intersection[2] = cross(edgePlanes[2], edgePlanes[0]);
	intersection[0] /= intersection[0].z;
	intersection[1] /= intersection[1].z;
	intersection[2] /= intersection[2].z;

	// calculate dilated triangle vertices
	float z[3];
	z[0] = -(intersection[0].x * trianglePlane.x + intersection[0].y * trianglePlane.y + trianglePlane.w) / trianglePlane.z;
	z[1] = -(intersection[1].x * trianglePlane.x + intersection[1].y * trianglePlane.y + trianglePlane.w) / trianglePlane.z;
	z[2] = -(intersection[2].x * trianglePlane.x + intersection[2].y * trianglePlane.y + trianglePlane.w) / trianglePlane.z;
	pos[0].xyz = vec3(intersection[0].xy, z[0]);
	pos[1].xyz = vec3(intersection[1].xy, z[1]);
	pos[2].xyz = vec3(intersection[2].xy, z[2]);
	
	vec4 voxelPos;

	ProjectPos = pos[0].xyz;
	gl_Position = pos[0];
	voxelPos = viewProjectInverse[projectIndex] * gl_Position;
	FragPos = voxelPos.xyz;
    EmitVertex();

	ProjectPos = pos[1].xyz;
	gl_Position = pos[1];
	voxelPos = viewProjectInverse[projectIndex] * gl_Position;
	FragPos = voxelPos.xyz;
    EmitVertex();

	ProjectPos = pos[2].xyz;
	gl_Position = pos[2];
	voxelPos = viewProjectInverse[projectIndex] * gl_Position;
	FragPos = voxelPos.xyz;
    EmitVertex();

    EndPrimitive();
}