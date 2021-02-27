#version 430 core

uniform vec3 objectColor;
//uniform vec3 lightDir;
uniform vec3 lightPos;
uniform vec3 lightPos2;
uniform vec3 cameraPos;

in vec3 interpNormal;
in vec3 fragPos;

void main()
{
	vec3 lightDir = normalize(lightPos-fragPos);
	vec3 lightDir2 = normalize(lightPos2-fragPos);
	vec3 V = normalize(cameraPos - fragPos);
	vec3 normal = normalize(interpNormal);
	vec3 R = reflect(-lightDir, normal);
	vec3 R2 = reflect(-lightDir2, normal);

	
	float specular = pow(max(0,dot(R,V)),10);
	float specular2 = pow(max(0,dot(R2,V)),10);
	float diffuse = max(0,dot(normal,lightDir));
	float diffuse2 = max(0,dot(normal,lightDir2));

	float ambient = 0.01;

	vec3 lightColor = vec3(1);
	vec3 lightColor2 = vec3(1.0f, 0.0f, 0.0f);

	vec3 shadedColor = objectColor * (diffuse + diffuse2) + (lightColor * specular) + (lightColor2 * specular2);

	gl_FragColor.rgb = mix(objectColor, shadedColor, 1.0 - ambient);
	gl_FragColor.a = 1.0;
}
