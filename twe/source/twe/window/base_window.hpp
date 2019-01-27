/*!
 *  Copyright (C) 2018 Wmbat
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

#ifndef BASE_WINDOW_H
#define BASE_WINDOW_H



#include "event.hpp"
#include "../twe_core.hpp"
#include "../utilities/message.hpp"
#include "../vulkan/core.hpp"

namespace twe
{
    class base_window
    {
    public:
        virtual ~base_window( ) = default;
    
        TWE_API virtual void poll_events( ) = 0;
    
        TWE_API bool is_open( ) const noexcept;
    
        TWE_API virtual vk::UniqueSurfaceKHR create_surface( const vk::Instance& instance ) const noexcept = 0;
    
        TWE_API uint32_t get_width( ) const noexcept;
        TWE_API uint32_t get_height( ) const noexcept;
    
        template<class C>
        std::enable_if_t<std::is_same_v<C, key_event_delg>, void > set_event_callback ( const C& callback )
        {
            key_event_.add_callback( callback );
        }
    
        template<class C>
        std::enable_if_t<std::is_same_v<C, mouse_button_event_delg>, void> set_event_callback( const C& callback )
        {
            mouse_button_event_.add_callback( callback );
        }
    
        template<class C>
        std::enable_if_t<std::is_same_v<C, mouse_motion_event_delg>, void> set_event_callback( const C& callback )
        {
            mouse_motion_event_.add_callback( callback );
        }
    
        template<class C>
        std::enable_if_t<std::is_same_v<C, window_close_event_delg>, void> set_event_callback( const C& callback )
        {
            window_close_event_.add_callback( callback );
        }
    
        template<class C>
        std::enable_if_t<std::is_same_v<C, framebuffer_resize_event_delg>, void> set_event_callback( const C& callback )
        {
            framebuffer_resize_event_.add_callback( callback );
        }
        
    protected:
        std::string title_;
    
        bool open_ = false;
    
        struct settings
        {
            uint32_t x_ = 100;
            uint32_t y_ = 100;
        
            uint32_t width_ = 1080;
            uint32_t height_ = 720;
        
            int default_screen_id_ = 0;
        
            bool fullscreen_ = false;
        } settings_;
        
        message_handler<const key_event> key_event_;
        message_handler<const mouse_button_event> mouse_button_event_;
        message_handler<const mouse_motion_event> mouse_motion_event_;
        message_handler<const window_close_event> window_close_event_;
        message_handler<const framebuffer_resize_event> framebuffer_resize_event_;
    };
}

#endif //BASE_WINDOW_H