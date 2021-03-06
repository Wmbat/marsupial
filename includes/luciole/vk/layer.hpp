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

#ifndef LUCIOLE_LAYER_HPP
#define LUCIOLE_LAYER_HPP

/* INCLUDES */
#include <string>

namespace vk
{
    /**
     * @brief Data aggregate holding the information about Vulkan
     * layers.
     */
    struct layer
    {
        /**
         * @brief Enum to define how needed the extension is.
         */
        enum class priority
        {
            e_none,
            e_required,
            e_optional
        };

        priority priority = priority::e_none;
        bool found = false;
        std::string name = { };
    };
} // namespace vk

#endif // LUCIOLE_LAYER_HPP
