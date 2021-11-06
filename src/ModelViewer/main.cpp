#include <Ranae/Common.h>
#include <iostream>
#include <vulkan/vulkan.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

namespace ranae {

  bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
      std::cerr << "Failed to initialize SDL2.\n";
      return false;
    }
    defer({ SDL_Quit(); })

    SDL_Window *window = {};
    if (!(window = SDL_CreateWindow("Ranae Model Viewer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1920, 1080, SDL_WINDOW_VULKAN))) {
      std::cerr << "Failed to create window.\n";
      return false;
    }
    defer( { SDL_DestroyWindow(window); } )

    VkResult result = {};

    VkApplicationInfo app_info = {
      .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pApplicationName   = "Ranae Model Viewer",
      .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
      .pEngineName        = "Ranae",
      .engineVersion      = VK_MAKE_VERSION(1, 0, 0),
      .apiVersion         = VK_API_VERSION_1_2
    };

    VkInstanceCreateInfo instance_info = {
      .sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pApplicationInfo = &app_info,
    };

    VkInstance instance = {};
    {
      std::vector<const char *> instance_extensions = {
      };
      uint32_t our_instance_extension_count = uint32_t(instance_extensions.size());
      uint32_t sdl_instance_extension_count = {};
      if (SDL_Vulkan_GetInstanceExtensions(window, &sdl_instance_extension_count, nullptr) != SDL_TRUE) {
        std::cerr << "Failed to get SDL instance extension count.\n";
        return false;
      }
      instance_extensions.resize(our_instance_extension_count + sdl_instance_extension_count);
      if (SDL_Vulkan_GetInstanceExtensions(window, &sdl_instance_extension_count, instance_extensions.data() + our_instance_extension_count) != SDL_TRUE) {
        std::cerr << "Failed to get SDL instance extensions.\n";
        return false;
      }

      instance_info.enabledExtensionCount   = uint32_t(instance_extensions.size());
      instance_info.ppEnabledExtensionNames = instance_extensions.data();

      if ((result = vkCreateInstance(&instance_info, nullptr, &instance)) != VK_SUCCESS) {
        std::cerr << "Failed to create Vulkan instance.\n";
        return false;
      }
    }
    defer( { vkDestroyInstance(instance, nullptr); })

    VkPhysicalDevice physical_device = {};
    {
      uint32_t physical_device_count = {};
      if ((result = vkEnumeratePhysicalDevices(instance, &physical_device_count, nullptr)) != VK_SUCCESS) {
        std::cerr << "Failed to get physical device count.\n";
        return false;
      }
      auto physical_devices = std::vector<VkPhysicalDevice>(physical_device_count);
      if ((result = vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices.data())) != VK_SUCCESS) {
        std::cerr << "Failed to enumerate physical devices.\n";
        return false;
      }

      // Use first device for now.
      if (physical_device_count)
        physical_device = physical_devices[0];
    }
    if (!physical_device) {
      std::cerr << "Failed to find suitable physical device.\n";
      return false;
    }

    VkSurfaceKHR surface = {};
    if (SDL_Vulkan_CreateSurface(window, instance, &surface) != SDL_TRUE) {
      std::cerr << "Failed to create SDL2 surface.\n";
      return false;
    }

    uint32_t graphics_queue_family_index = ~0u;
    {
      uint32_t queue_family_count = {};
      vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);
      auto queue_families = std::vector<VkQueueFamilyProperties>(queue_family_count);
      vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families.data());
      for (uint32_t i = 0; i < queue_family_count; i++) {
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
          graphics_queue_family_index = i;
          break;
        }
      }
    }
    if (graphics_queue_family_index == ~0u) {
      std::cerr << "Failed to find graphics queue.\n";
      return false;
    }

    float queue_priority = 1.0f;
    VkDeviceQueueCreateInfo queue_info = {
      .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .queueFamilyIndex = graphics_queue_family_index,
      .queueCount       = 1,
      .pQueuePriorities = &queue_priority,
    };

    VkPhysicalDeviceFeatures device_features = {};

    const auto device_extensions = array_of<const char*>(
      VK_KHR_SWAPCHAIN_EXTENSION_NAME
    );

    VkDeviceCreateInfo device_info = {
        .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount    = 1,
        .pQueueCreateInfos       = &queue_info,
        .enabledExtensionCount   = device_extensions.size(),
        .ppEnabledExtensionNames = device_extensions.data(),
        .pEnabledFeatures        = &device_features,
    };

    VkDevice device = {};
    if ((result = vkCreateDevice(physical_device, &device_info, nullptr, &device)) != VK_SUCCESS) {
      std::cerr << "Failed to create Vulkan device.\n";
      return false;
    }

    VkQueue queue = {};
    vkGetDeviceQueue(device, graphics_queue_family_index, 0, &queue);

    VkCommandPoolCreateInfo command_pool_info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .queueFamilyIndex = graphics_queue_family_index
    };

    VkCommandPool command_pool = {};
    if ((result = vkCreateCommandPool(device, &command_pool_info, nullptr, &command_pool)) != VK_SUCCESS) {
      std::cerr << "Failed to create command pool.\n";
      return false;
    }

    constexpr uint32_t FramesInFlight = 3;

    std::array<VkCommandBuffer, FramesInFlight> cmd_buffers = {};

    VkCommandBufferAllocateInfo cmd_buffer_allocate_info = {
      .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .commandPool        = command_pool,
      .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = FramesInFlight,
    };
    if ((result = vkAllocateCommandBuffers(device, &cmd_buffer_allocate_info, cmd_buffers.data())) != VK_SUCCESS) {
      std::cerr << "Failed to allocate command buffers.\n";
      return false;
    }

    std::array<VkFence, FramesInFlight> wsi_fences = {};
    for (uint32_t i = 0; i < FramesInFlight; i++) {
      VkFenceCreateInfo fence_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
      };

      if ((result = vkCreateFence(device, &fence_info, nullptr, &wsi_fences[i])) != VK_SUCCESS) {
        std::cerr << "Failed to create fences.\n";
        return false;
      }
    }

    VkSwapchainCreateInfoKHR swapchain_info = {
      .sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .surface          = surface,
      .minImageCount    = 1,
      .imageFormat      = VK_FORMAT_B8G8R8A8_UNORM,
      .imageColorSpace  = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
      .imageExtent      = VkExtent2D{ 1920, 1080 },
      .imageArrayLayers = 1,
      .imageUsage       = VK_IMAGE_USAGE_TRANSFER_DST_BIT,
      .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .preTransform     = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
      .compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      .presentMode      = VK_PRESENT_MODE_MAILBOX_KHR,
      .clipped          = VK_TRUE,
    };

    VkSwapchainKHR swapchain = {};
    if ((result = vkCreateSwapchainKHR(device, &swapchain_info, nullptr, &swapchain)) != VK_SUCCESS) {
      std::cerr << "Failed to create swapchain.";
      return false;
    }

    uint32_t swapchain_image_count = {};
    if ((result = vkGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, nullptr)) != VK_SUCCESS) {
      std::cerr << "Failed to get swapchain image count.\n";
      return false;
    }
    auto swapchain_images = std::vector<VkImage>(swapchain_image_count);
    if ((result = vkGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, swapchain_images.data())) != VK_SUCCESS) {
      std::cerr << "Failed to get swapchain images.\n";
      return false;
    }

    auto swapchain_image_views = std::vector<VkImageView>(swapchain_image_count);
    for (uint32_t i = 0; i < swapchain_image_count; i++) {
      VkImageViewCreateInfo image_view_info = {
        .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image    = swapchain_images[i],
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format   = VK_FORMAT_B8G8R8A8_UNORM,
        .subresourceRange = {
          .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
          .baseMipLevel   = 0,
          .levelCount     = 1,
          .baseArrayLayer = 0,
          .layerCount     = 1,
        }
      };
      if ((result = vkCreateImageView(device, &image_view_info, nullptr, &swapchain_image_views[i])) != VK_SUCCESS) {
        std::cerr << "Failed to create image view\n";
        return false;
      }
    }

    uint32_t frame_id = 0;
    bool shouldQuit = false;
    while (!shouldQuit) {
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) shouldQuit = true;
      }

      if ((result = vkWaitForFences(device, 1, &wsi_fences[frame_id], VK_TRUE, ~0u)) != VK_SUCCESS) {
        std::cerr << "Failed to wait for WSI fences.\n";
        return false;
      }

      uint32_t image_index = {};
      result = vkAcquireNextImageKHR(device, swapchain, ~0u, VK_NULL_HANDLE, wsi_fences[frame_id], &image_index);
      // blah remake.
      frame_id = (frame_id + 1) % FramesInFlight;

      VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .swapchainCount = 1,
        .pSwapchains = &swapchain,
      };
      result = vkQueuePresentKHR(queue, &present_info);
    }

    return true;
  }
}

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;
  ranae:: init();
}