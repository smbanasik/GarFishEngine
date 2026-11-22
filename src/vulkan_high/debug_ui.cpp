#include <vulkan_high/debug_ui.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <vulkan_low/initializers.hpp>

void gf::debug_ui::draw_imgui(VkManager& vk_context, VkCommandBuffer cmd, VkImageView target_image_view) {
    VkRenderingAttachmentInfo color_attachment
        = vk_init::attachment_info(target_image_view, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    VkRenderingInfo render_info
        = vk_init::rendering_info(vk_context.swapchain.swapchain_extent, &color_attachment, nullptr);

    vkCmdBeginRendering(cmd, &render_info);

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

    vkCmdEndRendering(cmd);
}

void gf::debug_ui::setup_imgui_frame(VkManager& vk_context) {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // ImGui::ShowDemoWindow();
    if (ImGui::Begin("background")) {
        ImGui::SliderFloat("Render Scale", &vk_context.render_scale, 0.3f, 1.0f);

        vkl::ComputeEffect& selected = vk_context.background_effects[vk_context.current_background_effect];
        ImGui::Text("Selected effect: ", selected.name);
        ImGui::SliderInt("Effect Index", &vk_context.current_background_effect, 0,
                         vk_context.background_effects.size() - 1);

        ImGui::InputFloat4("data1", (float*)&selected.data.data1);
        ImGui::InputFloat4("data2", (float*)&selected.data.data2);
        ImGui::InputFloat4("data3", (float*)&selected.data.data3);
        ImGui::InputFloat4("data4", (float*)&selected.data.data4);
    }
    if (ImGui::Begin("Stats")) {
        ImGui::Text("frametime %f ms", vk_context.stats.frametime);
        ImGui::Text("draw time %f ms", vk_context.stats.mesh_draw_time);
        ImGui::Text("update time %f ms", vk_context.stats.scene_update_time);
        ImGui::Text("triangles %i", vk_context.stats.triangle_count);
        ImGui::Text("draws %i", vk_context.stats.drawcall_count);
        ImGui::End();
    }
    ImGui::End();

    ImGui::Render();
}

void gf::debug_ui::init_imgui(VkManager& vk_context, GLFWwindow* window) {

    VkDescriptorPoolSize pool_sizes[] = {{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
                                         {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                                         {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                                         {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = (uint32_t)std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    VkDescriptorPool imguiPool;
    vkCreateDescriptorPool(vk_context.core.device, &pool_info, nullptr, &imguiPool);

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(window, true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = vk_context.core.instance;
    init_info.PhysicalDevice = vk_context.core.gpu;
    init_info.Device = vk_context.core.device;
    init_info.Queue = vk_context.core.graphics_queue;
    init_info.DescriptorPool = imguiPool;
    init_info.MinImageCount = 3;
    init_info.ImageCount = 3;
    init_info.UseDynamicRendering = true;
    init_info.PipelineRenderingCreateInfo = {.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};
    init_info.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
    init_info.PipelineRenderingCreateInfo.pColorAttachmentFormats = &vk_context.swapchain.swapchain_format;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&init_info);
    ImGui_ImplVulkan_CreateFontsTexture();

    VkDevice device = vk_context.core.device;
    vk_context.global_deletion_stack.push_function([device, imguiPool]() {
        ImGui_ImplVulkan_Shutdown();
        vkDestroyDescriptorPool(device, imguiPool, nullptr);
    });
}