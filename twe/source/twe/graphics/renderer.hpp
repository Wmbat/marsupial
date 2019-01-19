/*
 *  Copyright (C) 2018-2019 Wmbat
 *
 *  wmbat@protonmail.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  You should have received a copy of the GNU General Public License
 *  GNU General Public License for more details.
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef VULKAN_PROJECT_RENDERER_H
#define VULKAN_PROJECT_RENDERER_H

#include <optional>

#include "vertex_buffer.hpp"
#include "../twe_core.hpp"
#include "../utilities/vk_utils.hpp"
#include "../vulkan/vk_memory_allocator.hpp"
#include "../shader_manager.hpp"
#include "../window/base_window.hpp"
#include "../pipeline_manager.hpp"

#include "../vulkan/context.hpp"
#include "../vulkan/swapchain.hpp"

namespace twe
{
    class renderer
    {
    private:
        struct queue_family_indices_type;
        struct swapchain_support_details_type;

    public:
        TWE_API renderer( base_window* p_window, const std::string& app_name, uint32_t app_version );
        TWE_API renderer( const renderer& renderer ) noexcept = delete;
        TWE_API renderer( renderer&& renderer ) noexcept;
        TWE_API ~renderer( );

        TWE_API renderer& operator=( const renderer& renderer ) noexcept = delete;
        TWE_API renderer& operator=( renderer&& renderer ) noexcept;
        
        template<shader_type T>
        uint32_t create_shader( const std::string& filepath, const std::string& entry_point )
        {
            return shader_manager_.insert<T>( shader_create_info{ vk_context_.device_.get(), filepath, entry_point } );
        }
        
        template<pipeline_type T>
        uint32_t create_pipeline( const std::string& pipeline_definition, uint32_t vert_id, uint32_t frag_id )
        {
            std::vector<vk::Viewport> viewports = {
                vk::Viewport( )
                    .setX( 0.0f )
                    .setY( 0.0f )
                    .setWidth( static_cast<float>( swapchain_.extent_.width ) )
                    .setHeight( static_cast<float>( swapchain_.extent_.height ) )
                    .setMinDepth( 0.0f )
                    .setMaxDepth( 1.0f )
            };
    
            std::vector<vk::Rect2D> scissors = {
                vk::Rect2D( )
                    .setOffset( { 0, 0 } )
                    .setExtent( swapchain_.extent_ )
            };
            
            const auto create_info = pipeline_create_info( )
                .set_device( &vk_context_.device_.get() )
                .set_render_pass( &vk_context_.render_pass_.get() )
                .set_pipeline_definition( pipeline_definition )
                .set_shader_manager( &shader_manager_ )
                .set_shader_ids( vert_id, frag_id )
                .set_viewports( viewports )
                .set_scissors( scissors );
            
            return pipeline_manager_.insert<T>( create_info );
        }
        
        TWE_API void set_pipeline( const uint32_t id );
        TWE_API void switch_pipeline( const uint32_t id );

        void TWE_API draw_frame( );
        
        void TWE_API record_draw_calls( );
        
        void TWE_API on_window_close( const window_close_event& event );
        void TWE_API on_framebuffer_resize( const framebuffer_resize_event& event );
        
        void TWE_API set_clear_colour( const glm::vec4& colour );
    
    private:
        void set_up( );
        
        const vk::UniqueInstance create_instance( const std::string& app_name, uint32_t app_version ) const noexcept;
        
        const vk::UniqueHandle<vk::DebugReportCallbackEXT, vk::DispatchLoaderDynamic> create_debug_report_callback( ) const noexcept;
    
        const vk::PhysicalDevice pick_physical_device( ) const noexcept;
    
        const vk::UniqueDevice create_device( ) const noexcept;
        
        const vk::UniqueSemaphore create_semaphore( ) const noexcept;
        
        const vk::UniqueFence create_fence( ) const noexcept;
        
        const vk::UniqueCommandPool create_command_pool( uint32_t queue_family ) const noexcept;
        
        const std::vector<vk::CommandBuffer> create_command_buffers( uint32_t count ) const noexcept;
        
        const vk::UniqueRenderPass create_render_pass( vk::PipelineBindPoint bind_point = vk::PipelineBindPoint::eGraphics ) const noexcept;
  

        bool check_instance_extension_support( const std::vector<const char*>& instance_extensions ) const noexcept;
    
        bool check_debug_layer_support( const std::vector<const char*>& debug_layers ) const noexcept;
    
        bool is_physical_device_suitable( const vk::SurfaceKHR& surface, const vk::PhysicalDevice& physical_device,
            const std::vector<const char*>& device_extensions ) const noexcept;
    
        bool check_physical_device_extension_support( const vk::PhysicalDevice& physical_device,
            const std::vector<const char*>& device_extensions ) const noexcept;
    
        bool is_swapchain_adequate( const vk::SurfaceKHR& surface,
            const vk::PhysicalDevice &physical_device ) const noexcept;
    
        const queue_family_indices_type find_queue_family_indices( const vk::SurfaceKHR& surface,
            const vk::PhysicalDevice &physical_device ) const noexcept;

        const swapchain_support_details_type query_swapchain_support( const vk::SurfaceKHR& surface,
            const vk::PhysicalDevice &physical_device ) const noexcept;

        const vk::SurfaceFormatKHR choose_swapchain_surface_format(
                const std::vector<vk::SurfaceFormatKHR>& available_formats ) const noexcept;

    private:
        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
        
        uint32_t window_width_;
        uint32_t window_height_;

        bool is_window_closed_ = false;
        bool framebuffer_resized_ = false;
    
        glm::vec4 clear_colour_;
        
        size_t current_frame_ = 0;
        
        uint32_t current_pipeline_;
        
        // vulkan::context<MAX_FRAMES_IN_FLIGHT> test_;
        
        struct vk_context_t
        {
            vk::DispatchLoaderDynamic dispatch_loader_dynamic_;
            
            vk::UniqueInstance instance_;
            
            vk::UniqueHandle<vk::DebugReportCallbackEXT, vk::DispatchLoaderDynamic> debug_report_callback_;
            
            vk::UniqueSurfaceKHR surface_;
            vk::PhysicalDevice gpu_;
            vk::UniqueDevice device_;
            vk::Queue graphics_queue_;
            
            std::vector<vk::UniqueSemaphore> image_available_semaphores_;
            std::vector<vk::UniqueSemaphore> render_finished_semaphores_;
            std::vector<vk::UniqueFence> in_flight_fences_;
            
            vk::UniqueCommandPool command_pool_;
            std::vector<vk::CommandBuffer> command_buffers_;
            
            vk::SurfaceFormatKHR surface_format_;
            
            /*
            struct swapchain
            {
                vk::UniqueSwapchainKHR swapchain_;
                
                vk::Extent2D extent_;
                
                std::vector<vk::Image> image_;
                std::vector<vk::UniqueImageView> image_views_;
                std::vector<vk::UniqueFramebuffer> framebuffers_;
            } swapchain_;
            */
            
            vk::UniqueRenderPass render_pass_;
            
            std::vector<const char*> instance_extensions_;
            std::vector<const char*> device_extensions_;
            std::vector<const char*> validation_layers_;
        } vk_context_;
    
        vulkan::swapchain swapchain_;
        
        vk_memory_allocator memory_allocator_;
        
        vertex_buffer vertex_buffer_;
        
        shader_manager shader_manager_;
        pipeline_manager pipeline_manager_;
        
    private:
        
        struct queue_family_indices_type
        {
            std::optional<uint32_t> graphic_family_;
            std::optional<uint32_t> present_family_;
            
            bool has_rendering_support( ) const
            {
                return graphic_family_.has_value() && present_family_.has_value();
            }
        };
        struct swapchain_support_details_type
        {
            vk::SurfaceCapabilitiesKHR capabilities_;
            std::vector<vk::SurfaceFormatKHR> formats_;
            std::vector<vk::PresentModeKHR> present_modes_;
        };
    };
}

#endif //VULKAN_PROJECT_RENDERER_H