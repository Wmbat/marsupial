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

#ifndef TWE_CORE_H
#define TWE_CORE_H

#include <cstdint>

#include <vulkan/vulkan.hpp>

#if defined( TWE_PLATFORM_WINDOWS )
#if defined( TWE_BUILD_DLL )
#define TWE_API __declspec( dllexport )
#else
#define TWE_API __declspec( dllimport )
#endif
#else
#define TWE_API
#endif

namespace twe
{
    static constexpr uint32_t kilobyte = 1024;
    static constexpr uint32_t megabyte = kilobyte * kilobyte;
    
    constexpr unsigned long long operator "" _kg( unsigned long long size ) { return size * kilobyte; }
    constexpr unsigned long long operator "" _mb( unsigned long long size ) { return size * megabyte; }
}


#endif //TWE_CORE_H