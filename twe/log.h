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

#ifndef ENGINE_LOGGER_H
#define ENGINE_LOGGER_H

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "TWE_core.h"

namespace twe
{
    class log
    {
    public:
        static void TWE_API init( );

        inline static spdlog::logger& get_core_logger( )
        {
            return *core_logger_;
        }

    private:
        static std::shared_ptr<spdlog::logger> core_logger_;
    };

    inline static void core_info( const std::string& msg )
    {
        log::get_core_logger().info( msg );
    }
    inline static void core_trace( const std::string& msg )
    {
        log::get_core_logger().trace( msg );
    }
    inline static void core_warn( const std::string& msg )
    {
        log::get_core_logger().warn( msg );
    }
    inline static void core_critical( const std::string& msg )
    {
        log::get_core_logger().critical( msg );
    }
    inline static void core_error( const std::string& msg )
    {
        log::get_core_logger().error( msg );
    }

    template<typename... arguments>
    inline static void core_info( const std::string& msg, const arguments&... args )
    {
        log::get_core_logger().info( msg.c_str( ), args... );
    }
    template<typename... arguments>
    inline static void core_trace( const std::string& msg, const arguments&... args )
    {
        log::get_core_logger().trace( msg.c_str( ), args... );
    }
    template<typename... arguments>
    inline static void core_warn( const std::string& msg, const arguments&... args )
    {
        log::get_core_logger().warn( msg.c_str( ), args... );
    }
    template<typename... arguments>
    inline static void core_critical( const std::string& msg, const arguments&... args )
    {
        log::get_core_logger().critical( msg.c_str(), args... );
    }
    template<typename... arguments>
    inline static void core_error( const std::string& msg, const arguments&... args )
    {
        log::get_core_logger().error( msg.c_str( ), args... );
    }
}

#endif //ENGINE_LOGGER_H