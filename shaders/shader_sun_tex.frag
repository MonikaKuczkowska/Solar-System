#version 430 core

uniform vec3 objectColor;
//uniform vec3 lightDir;
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform sampler2D colorTexture;

in vec3 interpNormal;
in vec3 fragPos;
in vec2 vertexCoord;

void main()
{
	vec3 lightDir = normalize(lightPos-fragPos);
	vec3 V = normalize(cameraPos-fragPos);
	vec3 normal = normalize(interpNormal);
	vec3 R = reflect(-lightDir,normal);
	
	float specular = pow(max(0,dot(R,V)),10);
	float diffuse = max(0,dot(normal,lightDir));
	float ambient = 0.1;

	vec4 textureColor = texture2D(colorTexture, vertexCoord);

	//gl_FragColor.rgb = mix(textureColor.xyz, textureColor.xyz * diffuse + vec3(1) * specular, 1.0 - ambient);
	gl_FragColor.rgb = textureColor.xyz;
	gl_FragColor.a = 1.0;
}
