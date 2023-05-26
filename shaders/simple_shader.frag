#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWorld;
layout(location = 3) in float visibility;

layout(location = 0) out vec4 outColor;

struct Fog {
	vec4 color;
	vec4 dist;
	vec4 densityGradient;
};

struct Sun {
	vec4 dir;
	vec4 color;
};

struct PointLight {
	vec4 position;
	vec4 color;
};

layout(set = 0, binding = 0) uniform GlobalUbo {
	mat4 projection;
	mat4 view;
	mat4 inverseView;
	vec4 ambientLightColor;
	Fog fog;
	Sun sun;
	PointLight pointLights[10];
	int numLights;
} ubo;

layout(push_constant) uniform Push {
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

void main() {

	vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
	vec3 specularLight = vec3(0.0);
	vec3 surfaceNormal = normalize(fragNormalWorld);

	vec3 cameraPosWorld = ubo.inverseView[3].xyz;
	vec3 viewDirection = normalize(cameraPosWorld - fragPosWorld);

	// ================ SUN LIGHT ================
	vec3 directionToSun = ubo.sun.dir.xyz - fragPosWorld;
	directionToSun = normalize(directionToSun);

	// ======== DIFFUSE ========
	float sunCosAngIncidence = max(dot(surfaceNormal, directionToSun), 0);
	vec3 sunIntensity = ubo.sun.color.xyz * ubo.sun.color.w;
	diffuseLight += sunIntensity * sunCosAngIncidence;

	// ======== SPECULAR ========
	vec3 sunHalfAngle = normalize(directionToSun + viewDirection);
	float sunBlinnTerm = dot(surfaceNormal, sunHalfAngle);
	sunBlinnTerm = clamp(sunBlinnTerm, 0, 1);
	sunBlinnTerm = pow(sunBlinnTerm, 256.0);
	specularLight += sunIntensity * sunBlinnTerm;

	// ======== POINT LIGHTS ========
	for (int i = 0; i < ubo.numLights; i++) {
		PointLight light = ubo.pointLights[i];
		vec3 directionToLight = light.position.xyz - fragPosWorld;
		float attenuation = 1.0 / dot(directionToLight, directionToLight); // distance squared
		directionToLight = normalize(directionToLight);

		// ======== DIFFUSE ========
		float cosAngIncidence = max(dot(surfaceNormal, directionToLight), 0);
		vec3 intensity = light.color.xyz * light.color.w * attenuation;

		diffuseLight += intensity * cosAngIncidence;

		// ======== SPECULAR ========
		vec3 halfAngle = normalize(directionToLight + viewDirection);
		float blinnTerm = dot(surfaceNormal, halfAngle);
		blinnTerm = clamp(blinnTerm, 0, 1);
		blinnTerm = pow(blinnTerm, 512.0); // higher values = sharper highlights
		specularLight += intensity * blinnTerm;
	}

	outColor = vec4(diffuseLight, 1.0f) * vec4(fragColor, 1.0) + vec4(specularLight, 1.0);

	outColor = mix(ubo.fog.color, outColor, visibility);

	//outColor = vec4(visibility);

}