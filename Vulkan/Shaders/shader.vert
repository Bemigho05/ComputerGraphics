
#version 450

layout(set = 0, binding = 0) uniform UBO {
	mat4 view;
	mat4 projection;
	mat4 viewProjection;
 } cameraData;

 layout(std140, set = 0, binding = 1) readonly buffer storageBuffer {
	mat4 model[];
 } ObjectData;


layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec2 vertexTexCoord;


layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
	gl_Position = cameraData.projection * cameraData.view * ObjectData.model[gl_InstanceIndex] * vec4(vertexPosition, 0.0, 1.0);
	fragColor = vertexColor;
	fragTexCoord = vertexTexCoord;
}