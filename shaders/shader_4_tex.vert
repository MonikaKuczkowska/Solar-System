#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBitangent;

uniform mat4 transformation;
uniform mat4 modelMatrix;
uniform vec3 lightPos;
uniform vec3 lightPos2;
uniform vec3 cameraPos;

out vec2 interpTexCoord;
out vec3 lightDirTS;
out vec3 lightDir2TS;
out vec3 viewDirTS;

void main()
{
	gl_Position = transformation * vec4(vertexPosition, 1.0);
	vec3 vertPos = (modelMatrix * vec4(vertexPosition, 1.0)).xyz;

	vec3 normal = (modelMatrix * vec4(vertexNormal, 0.0)).xyz;
	vec3 tangent = (modelMatrix * vec4(vertexTangent, 0.0)).xyz;
	vec3 bitangent = (modelMatrix * vec4(vertexBitangent, 0.0)).xyz;

	mat3 TBN = transpose(mat3(tangent, bitangent, normal));

	vec3 viewDir = normalize(cameraPos - vertPos);

	vec3 fragPos = (modelMatrix * vec4(vertexPosition,1)).xyz;
	vec3 lightDir = normalize(vertPos - lightPos);
	vec3 lightDir2 = normalize(vertPos - lightPos2);

	lightDirTS = TBN * lightDir;
	lightDir2TS = TBN * lightDir2;
	viewDirTS = TBN * viewDir;

	interpTexCoord = vertexTexCoord;
	interpTexCoord[1] = -interpTexCoord[1];
}
