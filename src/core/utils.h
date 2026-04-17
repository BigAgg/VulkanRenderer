#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <array>

// Make my own definitions of glm::vec
#define Vector2 glm::vec2
#define Vector3 glm::vec3
#define Vector4 glm::vec4
#define Color glm::vec4

struct Vertex {
	Vector3 pos;
	Vector3 color;
	Vector2 texCoord;

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		/*
		* VK_VERTEX_INPUT_RATE_VERTEX: Move to the next data entry after each vertex
		* VK_VERTEX_INPUT_RATE_INSTANCE: Move to the next data entry after each instance
		*/
		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
		// Setting one attributeDescription per variable in Vertex
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		/* Following shader types and formats are commonly used together:
		* float: VK_FORMAT_R32_SFLOAT
		* vec2: VK_FORMAT_R32G32_SFLOAT
		* vec3:	VK_FORMAT_R32G32B32_SFLOAT
		* vec4: VK_FORMAT_R32G32B32A32_SFLOAT
		*/
		attributeDescriptions[0].offset = offsetof(Vertex, pos);
		
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

		return attributeDescriptions;
	}
};

struct UniformBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

/* Example for nested as you have to define alignas by yourself with nested structures
struct Foo {
	glm::vec3 v;
};

struct UniformBufferObject {
	Foo f1;
	alignas(16) Foo f2;
}
*/

#undef Rectangle	// Undef for windows.h
struct Rectangle {
	float x, y, width, height;
};

struct Circle {
	float x, y, rad;
};

// Image storing
struct Image {
	int width, height, channels;
	void* data;
};
