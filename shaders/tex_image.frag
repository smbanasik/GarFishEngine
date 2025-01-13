//glsl version 4.5
#version 450

//shader input
layout (location = 0) in vec3 inColor;
layout (location = 1) in vec2 inUV;
//output write
layout (location = 0) out vec4 outFragColor;

// Not needed, but necessary for draw pipeline right now.
layout(set = 0, binding = 0) uniform  SceneData{

	mat4 view;
	mat4 proj;
	mat4 viewproj;
	vec4 ambientColor;
	vec4 sunlightDirection; //w for sun power
	vec4 sunlightColor;
} sceneData;
//texture to access
layout(set = 1, binding = 0) uniform sampler2D displayTexture;

void main() 
{
	outFragColor = texture(displayTexture,inUV);
    outFragColor = outFragColor * vec4(inColor, 1.0f);
}