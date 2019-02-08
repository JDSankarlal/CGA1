#version 420

layout(std140, binding = 1) uniform Time
{
	uniform float uTime;
};

layout(std140, binding = 2) uniform LightScene
{
	uniform vec3 uSceneAmbient;
};

layout(std140, binding = 3) uniform Light
{
	uniform vec3 uLightPosition;
	uniform vec3 uLightColor;
	uniform vec3 uLightDirection;
	uniform vec4 uLightAttenuation;
};

#define uLightAttenConst	uLightAttenuation.r;
#define uLightAttenLinear	uLightAttenuation.g;
#define uLightAttenQuad		uLightAttenuation.b;
#define uLightRadius		uLightAttenuation.a;


layout(std140, binding = 5) uniform ToonActive
{
	uniform bool uToonActive;
};

layout(std140, binding = 6) uniform RimActive
{
	uniform bool uRimActive;
};

layout(std140, binding = 7) uniform AmbientActive
{
	uniform bool uAmbientActive;
};

layout(std140, binding = 8) uniform SpecularActive
{
	uniform bool uSpecularActive;
};

layout(binding = 31) uniform sampler2D uTexToonRamp;



layout(binding = 0) uniform sampler2D uTexAlbedo;
layout(binding = 1) uniform sampler2D uTexEmissive;
layout(binding = 2) uniform sampler2D uTexSpecular;

uniform vec3 DiffuseLight = vec3(0.0, 0.0, 1.0);
uniform vec3 RimColor  = vec3(0.0, 0.0, 0.0);
uniform float gamma = 1/0.01;
uniform float uMaterialSpecularExponent = 64.0;

in vec2 texcoord;
in vec3 norm;
in vec3 pos;

out vec4 outColor;

void main()
{
	vec2 texOffset = texcoord;

	vec4 albedoColor = texture(uTexAlbedo, texOffset);
	if(uAmbientActive)
	{
		outColor.rgb = albedoColor.rgb * uSceneAmbient; 
		outColor.a = albedoColor.a;
	}
	else
	{
		outColor.rgb = albedoColor.rgb;
		outColor.a = albedoColor.a;
	}

	// Fix length after rasterizer interpolates
	vec3 normal = normalize(norm);

	vec3 lightVec = uLightPosition - pos;
	float dist = length(lightVec);
	vec3 lightDir = lightVec / dist;

	float NdotL = dot(normal, lightDir);

	// If the normal is facing the light
	{
		// Normalized vector pointing towards the camera
		vec3 eye = normalize(-pos);
		
		// Calculate attenuation (falloff)
		// Add a small number to avoid divide by zero.
		float attenuation = 1.0 / (1.0 + dist * 0.01 + dist * dist * 0.001);
		if((!uAmbientActive) && (!uSpecularActive))
			attenuation = 0;

		if(uToonActive)
		{
			NdotL = NdotL * 0.5 + 0.5;
			outColor.rgb += albedoColor.rgb * uLightColor * texture(uTexToonRamp, vec2(NdotL, 0.5)).rgb * uLightAttenuation.rgb;
		}
		else
		{
			NdotL = max(NdotL, 0.0);
			// Calculate the diffuse contribution
			outColor.rgb += albedoColor.rgb * uLightColor * NdotL * attenuation;
		}

		if (uSpecularActive)
		{
			vec3 reflection = reflect(-lightDir, normal);
		
			float specularStrength = dot(reflection, eye);
			specularStrength = max(specularStrength, 0.0f); // don't let it fall before zero

			// Calculate the specular contribution
			outColor.rgb += texture(uTexSpecular, texOffset).rgb * uLightColor * pow(specularStrength, uMaterialSpecularExponent) * attenuation;
		}
		else
		{
			outColor.rgb = outColor.rgb;
		}
		
		if (uRimActive)
		{
			vec3 tex = texture(uTexAlbedo, texcoord).rgb;

			//Diffuse Light
			vec3 diffuse = DiffuseLight * max(0, dot(pos, norm));

			//RIM LIGHTING
			float rim = 1 - max(dot(pos, norm), 0.0);
			rim = smoothstep(0.6, 1.0, rim);
			vec3 finalRim = RimColor * vec3(rim, rim, rim);

			vec3 finalColor = diffuse + finalRim + tex;

			vec3 finalColorGamma = vec3(pow(finalColor.r, gamma),
								pow(finalColor.g, gamma),
								pow(finalColor.b, gamma));
		
			outColor += vec4(finalColorGamma, 1);
		}
		else
		{
			outColor.rgb = outColor.rgb;
		}
	}
	outColor.rgb += texture(uTexEmissive, texOffset).rgb;
}
