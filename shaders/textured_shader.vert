#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragPosWorld;
layout(location = 2) out vec3 fragNormalWorld;
layout(location = 3) out vec2 texCoord;
layout(location = 4) out float visibility;

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

layout(set = 0, binding = 1) uniform sampler2D tex;

layout(push_constant) uniform Push {
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

void main() {

	vec4 positionWorld = push.modelMatrix * vec4(position, 1.0);
	vec4 positionRelativeToCamera = ubo.view * positionWorld;

	gl_Position = ubo.projection * (positionRelativeToCamera);

	fragNormalWorld = normalize(mat3(push.normalMatrix) * normal);
	fragPosWorld = positionWorld.xyz;
	fragColor = color;

	texCoord = uv;

	float dist = length(positionRelativeToCamera.xyz);
	visibility = exp(-pow((dist * ubo.fog.densityGradient.x), ubo.fog.densityGradient.y));
	//visibility = mix(dist, ubo.fog.dist.x, ubo.fog.dist.y);
	visibility = clamp(visibility, 0, 1);

	//visibility = dist;

}