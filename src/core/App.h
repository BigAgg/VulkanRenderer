#pragma once

#include <vector>
#include <iostream>
#include <optional>
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR // Tells glfw to specifically use windows
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "utils/logging.h"

/*
* Checking if a struct member has a value when it is of type std::optional
* 
* graphicsFamily.has_value() // false
* grapihcsFamily = 0;
* grapihcsFamily.has_value() // true
*/
struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool isComplete() {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

// Structure to hold information about swap chain for compatibility
struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

class App {
public:
  void run();
  void init(int width, int height, const char* name);
  void beginDrawing();
  void endDrawing();
  void closeWindow();

  bool shouldClose();
private:
  void initWindow(int width, int height, const char* name);
  void initVulkan();
  void mainLoop();
  void drawFrame();
  void cleanup();
  void cleanupSwapChain();

  // Vulkan Initialize functions
  void createInstance();
  void setupDebugMessenger();
  void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
  void pickPhysicalDevice();
  bool isDeviceSuitable(VkPhysicalDevice device);
  bool checkDeviceExtensionSupport(VkPhysicalDevice& device);
  int rateDeviceSuitability(VkPhysicalDevice device);
  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
  void createLogicalDevice();
  void createSurface();
  void createSwapChain();
  void createImageViews();
  void createRenderPass();
  void createGraphicsPipeline();
  void createFramebuffers();
  void createCommandPool();
  void createCommandBuffers();
  void createSyncObjects();
  void recreateSwapChain();
  void createVertexBuffer();
  void createIndexBuffer();
  void createUniformBuffers();
  void createDescriptorPool();
  void createDescriptorSets();
  void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
    VkBuffer& buffer, VkDeviceMemory& bufferMemory);
  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
  void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
  VkCommandBuffer beginSingleTimeCommands();
  void endSingleTimeCommands(VkCommandBuffer commandBuffer);
  void createDescriptorSetLayout();
  void updateUniformBuffer(uint32_t currentImage);
  SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
  VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
  VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
  VkShaderModule createShaderModule(const std::vector<char>& code);
  uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
  // Texture functions
  void createTextureImage();
  void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
  void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
  void createTextureImageView();
  VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
  void createTextureSampler();
  // Depth buffering
  void createDepthResources();
  VkFormat findDepthFormat();
  VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
  bool hasStencilComponent(VkFormat format);

  // Buffer recording
  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

  // callback functions
  static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

  // Validation layers
  bool checkValidationLayerSupport();
  // Callback function for validation layers
  std::vector<const char*> getRequiredExtensions();
  static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData
  ) {
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
      logging::logerror("App::debugCallback Validation layer: \n\t%s", pCallbackData->pMessage);
    }
    else if(messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
      logging::logwarning("App::debugCallback Validation layer: \n\t%s", pCallbackData->pMessage);
    return VK_FALSE;
  }

  // glfw attributes
  GLFWwindow* m_window;
  // Vulkan attributes
  VkInstance m_instance;
  VkDebugUtilsMessengerEXT m_debugMessenger;
  VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
  VkDevice m_device;  // Logical device to interface with the physical device
  VkQueue m_graphicsQueue;  // handle to interface with queues for logical device
  VkSurfaceKHR m_surface; // Holds WSI extensions and sets up our extension requirement by the way we set it up
  VkQueue m_presentQueue;
  VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
  std::vector<VkImage> m_swapChainImages;
  VkFormat m_swapChainImageFormat;
  VkExtent2D m_swapChainExtent;
  // Describes how images are treated for example treat it as 2D texture depth texture without any mipmapping levels
  // stores image views for every image in our swap chain
  std::vector<VkImageView> m_swapChainImageViews;
  VkRenderPass m_renderPass;
  VkDescriptorSetLayout m_descriptorSetLayout;
  VkPipelineLayout m_pipelineLayout;
  VkPipeline m_graphicsPipeline;
  std::vector<VkFramebuffer> m_swapChainFramebuffers;
  VkCommandPool m_commandPool;  
  std::vector<VkCommandBuffer> m_commandBuffers;  // stores all drawing commands so they can be submitted at the same time

  std::vector<VkSemaphore> m_imageAvailableSemaphores;
  std::vector<VkSemaphore> m_imageRenderFinishedSemaphores;
  std::vector<VkFence> m_inFlightFences;
  uint32_t m_currentFrame = 0;

  // vertex buffer
  VkBuffer m_vertexBuffer;
  VkDeviceMemory m_vertexBufferMemory;
  // index buffer
  VkBuffer m_indexBuffer;
  VkDeviceMemory m_indexBufferMemory;

  // uniform buffers
  std::vector<VkBuffer> m_uniformBuffers;
  std::vector<VkDeviceMemory> m_uniformBuffersMemory;
  std::vector<void*> m_uniformBuffersMapped;

  // Descriptor pool
  VkDescriptorPool m_descriptorPool;
  std::vector<VkDescriptorSet> m_descriptorSets;

  bool m_framebufferResized = false;

  // Textures
  VkImage m_textureImage;
  VkDeviceMemory m_textureImageMemory;
  VkImageView m_textureImageView;
  VkSampler m_textureSampler;

  // Depth buffering
  VkImage m_depthImage;
  VkDeviceMemory m_depthImageMemory;
  VkImageView m_depthImageView;

  // Helper functions
  static std::vector<char> readFile(const std::string& filename);
};
