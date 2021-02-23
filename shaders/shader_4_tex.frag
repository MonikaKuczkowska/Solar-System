#version 430 core

uniform sampler2D colorTexture;
uniform sampler2D normalSampler;

in vec2 interpTexCoord;
in vec3 lightDirTS;
in vec3 lightDir2TS;
in vec3 viewDirTS;

void main()
{
	vec3 L = normalize(-lightDirTS);
	vec3 V = normalize(viewDirTS);
	vec3 N = normalize((texture2D(normalSampler, interpTexCoord).rgb * 2 - 1));
	vec3 R = reflect(-normalize(L), N);

	float diffuse = max(0, dot(N, L));
	
	float specular_pow = 10;
	float specular = pow(max(0, dot(R, V)), specular_pow);

	//Second lightsource
	vec3 L2 = normalize(-lightDir2TS);
	vec3 R2 = reflect(-normalize(L2), N);

	float diffuse2 = max(0, dot(N, L2));

	float specular_pow2 = 20;
	float specular2 = pow(max(0, dot(R2, V)), specular_pow2);

	vec3 color = texture2D(colorTexture, interpTexCoord).rgb;

	vec3 lightColor = vec3(1);
	vec3 lightColor2 = vec3(1.0f, 0.0f, 0.0f);

	vec3 shadedColor = color * (diffuse + diffuse2) + (lightColor * specular) + (lightColor2 * specular2);
	
	float ambient = 0.05;

	gl_FragColor = vec4(mix(color, shadedColor, 1.0 - ambient), 1.0);
}
