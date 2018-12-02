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

#include <iostream>
#include <window.h>


#include "window.h"
#include "log.h"

namespace TWE
{
#if defined( VK_USE_PLATFORM_WIN32_KHR )

#elif defined( VK_USE_PLATFORM_XCB_KHR )
    static inline std::unique_ptr<xcb_intern_atom_reply_t> intern_atom_helper( xcb_connection_t *p_connection, bool only_if_exists, const std::string& str )
    {
        xcb_intern_atom_cookie_t cookie = xcb_intern_atom( p_connection, only_if_exists, str.size(), str.c_str() );

        return std::unique_ptr<xcb_intern_atom_reply_t>( xcb_intern_atom_reply( p_connection, cookie, NULL ) );
    }
#endif

    event window::event_handler::pop_event( ) noexcept
    {
        const auto ret = buffer_[head_];

        buffer_[head_] = { };

        head_ = ( head_ + 1 ) % BUFFER_SIZE;

        return ret;
    }

    void window::event_handler::push_event( event event ) noexcept
    {
        if (( tail_ + 1 ) % BUFFER_SIZE == head_ )
        {
            pop_event( );
        }
    
        buffer_[tail_] = event;
    
        if ( buffer_[tail_].type_ == event::type::mouse_button_pressed )
        {
            button_states_[static_cast<size_t>( buffer_[tail_].mouse_button.button_ )] = true;
        }
        else if ( buffer_[tail_].type_ == event::type::mouse_button_released )
        {
            button_states_[static_cast<size_t>( buffer_[tail_].mouse_button.button_ )] = false;
        }
        else if ( buffer_[tail_].type_ == event::type::key_pressed )
        {
            key_states_[static_cast<size_t>( buffer_[tail_].key.key_ )] = true;
        }
        else if ( buffer_[tail_].type_ == event::type::key_released )
        {
            key_states_[static_cast<size_t>( buffer_[tail_].key.key_ )] = false;
        }
        
        tail_ = ( tail_ + 1 ) % BUFFER_SIZE;
    }

    bool window::event_handler::is_keyboard_key_pressed( keyboard::key key_code ) noexcept
    {
        return key_states_[static_cast<size_t>( key_code )];
    }
    
    bool window::event_handler::is_mouse_button_pressed( mouse::button button_code ) noexcept
    {
        return button_states_[static_cast<size_t>( button_code )];
    }
    
    bool window::event_handler::is_empty( ) const noexcept
    {
        return head_ == tail_;
    }

#if defined( VK_USE_PLATFORM_WIN32_KHR )
    window::window ( HINSTANCE instance, const std::string& title )
        : title_( title )
    {

    }
#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )

#elif defined( VK_USE_PLATFORM_XCB_KHR )
    window::window( const std::string &title )
            :
            title_( title ),
            open_( true )
    {
        core_info ( "Using XCB for window creation." );

        /** Connect to X11 window system. */
        p_xcb_connection_ = std::unique_ptr<xcb_connection_t, std::function<void( xcb_connection_t* )>>(
                xcb_connect( nullptr, &settings_.default_screen_id_ ),
                []( xcb_connection_t* p ) { xcb_disconnect( p ); } );

        if( xcb_connection_has_error( p_xcb_connection_.get() ) )
        {
            core_error( "Failed to connecte to the X server.\nDisconnecting from X server.\nExiting Application." );

            p_xcb_connection_.reset( );
        }
        else
        {
            core_info( "XCB -> Connection to X server established." );
        }

        /** Get Default monitor */
        auto monitor_nbr = xcb_setup_roots_iterator( xcb_get_setup( p_xcb_connection_.get() ) ).rem;

        /** Loop through all available monitors. */
        auto iter = xcb_setup_roots_iterator( xcb_get_setup( p_xcb_connection_.get() ) );
        while( monitor_nbr-- > 1 )
        {
            xcb_screen_next( &iter );   // TODO: Allow user to pick their prefered monitor.
        }
        p_xcb_screen_ = iter.data;

        xcb_window_ = xcb_generate_id( p_xcb_connection_.get() );
        core_info( "XCB -> window ID generated: " + std::to_string( xcb_window_ ) + '.' );

        uint32_t value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
        uint32_t value_list[32];
        value_list[0] = p_xcb_screen_->black_pixel;
        value_list[1] =
                XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY |
                XCB_EVENT_MASK_FOCUS_CHANGE |
                XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
                XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                XCB_EVENT_MASK_POINTER_MOTION;

        if( settings_.fullscreen_ )
        {
            core_info( "XCB -> window fullscreen mode." );

            settings_.width_ = p_xcb_screen_->width_in_pixels;
            settings_.height_ = p_xcb_screen_->height_in_pixels;
        }

        xcb_create_window(
                p_xcb_connection_.get(),                          /* Connection             */
                XCB_COPY_FROM_PARENT,                             /* Depth ( same as root ) */
                xcb_window_,                                      /* Window ID              */
                p_xcb_screen_->root,                              /* Parent Window          */
                settings_.x_, settings_.y_,                       /* Window Position        */
                settings_.width_, settings_.height_, 10,          /* Window + border Size   */
                XCB_WINDOW_CLASS_INPUT_OUTPUT,                    /* Class                  */
                p_xcb_screen_->root_visual,                       /* Visual                 */
                value_mask, value_list );                         /* Masks                  */

        core_info( "XCB -> window created." );

        auto reply = intern_atom_helper( p_xcb_connection_.get(), true, "WM_PROTOCOLS" );

        p_xcb_wm_delete_window_ = intern_atom_helper( p_xcb_connection_.get(), false, "WM_DELETE_WINDOW" );

        /** Allows checking of window closing event. */
        xcb_change_property(
                p_xcb_connection_.get(), XCB_PROP_MODE_REPLACE,
                xcb_window_, reply->atom, 4, 32, 1,
                &p_xcb_wm_delete_window_->atom );

        /** Change the title of the window. */
        xcb_change_property(
                p_xcb_connection_.get(), XCB_PROP_MODE_REPLACE,
                xcb_window_, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
                title_.size(), title_.c_str() );


        /** Set the window to fullscreen if fullscreen is enabled. */
        if ( settings_.fullscreen_ )
        {
            auto p_atom_wm_state = intern_atom_helper( p_xcb_connection_.get(), false, "_NET_WM_STATE" );
            auto p_atom_wm_fullscreen = intern_atom_helper( p_xcb_connection_.get(), false, "_NET_WM_STATE_FULLSCREEN");

            xcb_change_property(
                    p_xcb_connection_.get(), XCB_PROP_MODE_REPLACE,
                    xcb_window_, p_atom_wm_state->atom,
                    XCB_ATOM_ATOM, 32, 1,
                    &p_atom_wm_fullscreen->atom );
        }

        xcb_map_window( p_xcb_connection_.get(), xcb_window_ );
        xcb_flush( p_xcb_connection_.get() );
    }
#endif
    window::window( window&& rhs ) noexcept
    {
        *this = std::move( rhs );
    }
    window::~window( )
    {
#if defined( VK_USE_PLATFORM_WIN32_KHR )
#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )
#elif defined( VK_USE_PLATFORM_XCB_KHR )
        xcb_destroy_window( p_xcb_connection_.get(), xcb_window_ );

        core_info( "XCB -> window destroyed." );
        core_info( "XCB -> Disconnected from X server." );
#endif
    }
    
    window &window::operator=( window &&rhs ) noexcept
    {
        if( this != &rhs )
        {
            title_ = rhs.title_;
            rhs.title_ = { };
            
            open_ = rhs.open_;
            rhs.open_ = false;
#if defined( VK_USE_PLATFORM_WIN32_KHR )

#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )

#elif defined( VK_USE_PLATFORM_XCB_KHR )
            p_xcb_connection_ = std::move( p_xcb_connection_ );
            
            p_xcb_screen_ = rhs.p_xcb_screen_;
            rhs.p_xcb_screen_ = nullptr;
            
            p_xcb_wm_delete_window_ = std::move( rhs.p_xcb_wm_delete_window_ );
            
            xcb_window_ = rhs.xcb_window_;
            rhs.xcb_window_ = 0;
#endif
            
            event_handler_ = rhs.event_handler_;
            rhs.event_handler_ = { };
            
            settings_ = rhs.settings_;
            rhs.settings_ = { };
        }
        
        return *this;
    }

    event window::pop_event( ) noexcept
    {
        return event_handler_.pop_event();
    }
    
    bool window::is_event_queue_empty( )
    {
        return event_handler_.is_empty();
    }
    
    bool window::is_kbd_key_pressed( keyboard::key key_code ) noexcept
    {
        return event_handler_.is_keyboard_key_pressed( key_code );
    }
    
    bool window::is_mb_pressed( mouse::button button_code ) noexcept
    {
        return event_handler_.is_mouse_button_pressed( button_code );
    }
    
    void window::poll_events( )
    {
#if defined( VK_USE_PLATFORM_WIN32_KHR )

#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )

#elif defined( VK_USE_PLATFORM_XCB_KHR )
        xcb_generic_event_t *e;
        while( ( e = xcb_poll_for_event( p_xcb_connection_.get() ) ) )
        {
            switch ( e->response_type & 0x7f )
            {
                case XCB_CLIENT_MESSAGE:
                {
                    const auto *message_event = reinterpret_cast<const xcb_client_message_event_t *>( e );
            
                    if ( message_event->data.data32[0] == p_xcb_wm_delete_window_->atom )
                    {
                        open_ = false;
                    }
                } break;
                case XCB_DESTROY_NOTIFY:
                {
                    open_ = false;
                } break;
                case XCB_CONFIGURE_NOTIFY:
                {
                    const auto *motion_event = reinterpret_cast<const xcb_configure_notify_event_t *>( e );
        
                    const auto resize = event( )
                        .set_type( event::type::window_resize )
                        .set_window_resize( static_cast<uint32_t>( motion_event->width ),
                                            static_cast<uint32_t>( motion_event->height ));
        
                    event_handler_.push_event( resize );
        
                    const auto move = event( )
                        .set_type( event::type::window_move )
                        .set_window_move( static_cast<uint32_t>( motion_event->x ),
                                          static_cast<uint32_t>( motion_event->y ));
        
                    event_handler_.push_event( move );
                } break;
                case XCB_FOCUS_IN:
                {
                    const auto *focus_in_event = reinterpret_cast< const xcb_focus_in_event_t * >( e );

                    const auto focus_in = event ( )
                        .set_type ( event::type::window_focus_in );

                    event_handler_.push_event ( focus_in );
                } break;
                case XCB_FOCUS_OUT:
                {
                    const auto *focus_out_event = reinterpret_cast<const xcb_focus_out_event_t *>( e );
                
                    const auto focus_out = event( )
                        .set_type( event::type::window_focus_out );
                
                    event_handler_.push_event( focus_out );
                } break;
                case XCB_KEY_PRESS:
                {
                    const auto *key_press_event = reinterpret_cast<const xcb_key_press_event_t *>( e );
                
                    const auto key_press = event( )
                        .set_type( event::type::key_pressed )
                        .set_key( static_cast<keyboard::key>( key_press_event->detail ) );
                
                    event_handler_.push_event( key_press );
                } break;
                case XCB_KEY_RELEASE:
                {
                    const auto *key_release_event = reinterpret_cast<const xcb_key_release_event_t *>( e );
                
                    const auto key_release = event( )
                        .set_type( event::type::key_released )
                        .set_key( static_cast<keyboard::key>( key_release_event->detail ) );
                
                    event_handler_.push_event( key_release );
                } break;
                case XCB_BUTTON_PRESS:
                {
                    const auto *button_press_event = reinterpret_cast<const xcb_button_press_event_t *>( e );
                
                    const auto button_press = event( )
                        .set_type( event::type::mouse_button_pressed )
                        .set_mouse_button( static_cast<mouse::button>( button_press_event->detail ) );
                    
                    event_handler_.push_event( button_press );
                } break;
                case XCB_BUTTON_RELEASE:
                {
                    const auto *button_release_event = reinterpret_cast<const xcb_button_release_event_t *>( e );
                    
                    const auto button_release = event( )
                        .set_type( event::type::mouse_button_released )
                        .set_mouse_button( static_cast<mouse::button>( button_release_event->detail ) );
    
                    event_handler_.push_event( button_release );
                } break;
                case XCB_MOTION_NOTIFY:
                {
                    const auto *cursor_motion = reinterpret_cast<const xcb_motion_notify_event_t *>( e );
        
                    const auto mouse_move = event( )
                        .set_type( event::type::mouse_move )
                        .set_mouse_move( static_cast<int32_t>( cursor_motion->event_x ),
                                         static_cast<int32_t>( cursor_motion->event_y ));
        
                    event_handler_.push_event( mouse_move );
                } break;
            }
    
            free( e );
        }
#endif
    }
    void window::handle_event( const event &event ) noexcept
    {
        if( event.type_ == event::type::window_move )
        {
            settings_.x_ = event.window_move.x_;
            settings_.y_ = event.window_move.y_;
        }
        else if( event.type_ == event::type::window_resize )
        {
            settings_.width_ = event.window_resize.x_;
            settings_.height_ = event.window_resize.y_;
        }
    }

    void window::set_title( const std::string &title ) noexcept
    {
#if defined( VK_USE_PLATFORM_WIN32_KHR )
#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )
#elif defined( VK_USE_PLATFORM_XCB_KHR )
        xcb_change_property(
                p_xcb_connection_.get(), XCB_PROP_MODE_REPLACE,
                xcb_window_, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
                title.size(), title.c_str() );

        xcb_flush( p_xcb_connection_.get() );
#endif
    }

    const std::string& window::get_title( ) const noexcept
    {
        return title_;
    }

    bool window::is_open( ) const noexcept
    {
        return open_;
    }

    vk_return_type<VkSurfaceKHR> window::create_surface( const VkInstance &instance ) const noexcept
    {
        VkSurfaceKHR surface;

#if defined( VK_USE_PLATFORM_WIN32_KHR )
        const VkWin32SurfaceCreateInfoKHR create_info
        {
            VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,            // sType
            nullptr,                                                    // pNext
            { },                                                        // flags
            win_instance_,                                            // hinstance
            win_window_                                               // hwnd
        };

        return { vkCreateWin32SurfaceKHR( instance, &create_info, nullptr, &surface ), surface };
#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )
        const VkWaylandSurfaceCreateInfoKHR create_info
        {
            VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,          // sType
            nullptr,                                                    // pNext
            { },                                                        // flags
        };

#elif defined( VK_USE_PLATFORM_XCB_KHR )
        const VkXcbSurfaceCreateInfoKHR create_info
        {
            VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,              // sType
            nullptr,                                                    // pNext
            { },                                                        // flags
            p_xcb_connection_.get(),                                    // connection
            xcb_window_                                                 // window
        };

        return { vkCreateXcbSurfaceKHR( instance, &create_info, nullptr, &surface ), surface };
#endif
    }

    uint32_t window::get_width( ) const noexcept
    {
        return settings_.width_;
    }

    uint32_t window::get_height( ) const noexcept
    {
        return settings_.height_;
    }
}