#version 420


layout(std140, binding = 2) uniform LightScene
{
	uniform vec3 uSceneAmbient;
};

layout(std140, binding = 3) uniform Light
{
	uniform vec3 uLightPosition;
	uniform vec3 uLightColor;
};

uniform float uMaterialSpecularExponent = 16.0;
layout(std140, binding = 1) uniform Time
{
	uniform float uTime;
};

layout(binding = 0) uniform sampler2D uTexAlbedo;
layout(binding = 1) uniform sampler2D uTexEmissive;
layout(binding = 2) uniform sampler2D uTexSpecular;

in vec2 texcoord;
in vec3 norm;
in vec3 pos;

out vec4 outColor;

void main()
{
	vec2 texOffset = texcoord;
	texOffset.x += sin(texOffset.y * 16 + uTime) * 0.1 + uTime * 0.1;

	vec4 albedoColor = texture(uTexAlbedo, texOffset);
	outColor.rgb = albedoColor.rgb * uSceneAmbient; 
	outColor.a = albedoColor.a;

	// Fix length after rasterizer interpolates
	vec3 normal = normalize(norm);

	vec3 lightVec = uLightPosition - pos;
	float dist = length(lightVec);
	vec3 lightDir = lightVec / dist;

	float NdotL = dot(normal, lightDir);

	// If the normal is facing the light
	if (NdotL > 0.0)
	{
		// Normalized vector pointing towards the camera
		vec3 eye = normalize(-pos);
		
		// Calculate attenuation (falloff)
		// Add a small number to avoid divide by zero.
		float attenuation = 1.0 / (1.0 + dist * 0.01 + dist * dist * 0.001);

		// Calculate the diffuse contribution
		outColor.rgb += albedoColor.rgb * uLightColor * NdotL * attenuation;
		
		vec3 reflection = reflect(-lightDir, normal);
		
		float specularStrength = dot(reflection, eye);
		specularStrength = max(specularStrength, 0.0f); // don't let it fall before zero

		// Calculate the specular contribution
		outColor.rgb += texture(uTexSpecular, texOffset).rgb * uLightColor * pow(specularStrength, uMaterialSpecularExponent) * attenuation;
	}

	outColor.rgb += texture(uTexEmissive, texOffset).rgb;
}
