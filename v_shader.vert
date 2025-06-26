#version 430 core
layout(location = 0) in vec3 position;
layout(location = 1) in int v_id;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec3 normal;

layout(std430,Binding=0) buffer MeshMatrix
{
	mat4 subMeshModelMat[];
};

out vec2 textureCoords;
out float fragcolor;
out vec3 Normal;
out vec3 fragPos;

uniform float color;
uniform mat4 ModelMat;
uniform mat4 projection;
uniform mat4 view;

void main()
{
	textureCoords=texCoords;
	fragcolor =  color;
	Normal = normal;
	fragPos = vec3(ModelMat * subMeshModelMat[v_id] * vec4(position,1));
	gl_Position = projection * view * ModelMat * subMeshModelMat[v_id] * vec4(position,1);
}