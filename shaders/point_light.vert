#version 450

const vec2 OFFSETS[6] = vec2[](
  vec2(-1.0, -1.0),
  vec2(-1.0, 1.0),
  vec2(1.0, -1.0),
  vec2(1.0, -1.0),
  vec2(-1.0, 1.0),
  vec2(1.0, 1.0)
);

layout(location = 0) out vec2 fragOffset;
layout(location = 3) out float visibility;

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
	vec4 position;
	vec4 color;
	float radius;
} push;

const float LIGHT_RADIUS = 0.05;

void main() {
	
	fragOffset = OFFSETS[gl_VertexIndex];
	
	vec4 lightInCameraSpace = ubo.view * vec4(push.position.xyz, 1.0);
	vec4 positionInCameraSpace = lightInCameraSpace + push.radius * vec4(fragOffset, 0.0, 0.0);

	gl_Position = ubo.projection * positionInCameraSpace;

	float dist = length(positionInCameraSpace.xyz);
	visibility = exp(-pow((dist * ubo.fog.densityGradient.x), ubo.fog.densityGradient.y));
	//visibility = mix(dist, ubo.fog.dist.x, ubo.fog.dist.y);
	visibility = clamp(visibility, 0, 1);

}