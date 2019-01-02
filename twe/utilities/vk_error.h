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

#ifndef ENGINE_VK_ERROR_H
#define ENGINE_VK_ERROR_H

#include <system_error>

#include <vulkan/vulkan.hpp>

#include "../TWE_core.h"

namespace twe
{
    /*!
     * @brief An error for vulkan related problems. It inherits from
     * std::system_error.
     */
    class vk_error : public std::system_error
    {
    private:
        /*!
         * @brief The category of the error. Inherits from
         * std::error_category.
         */
        class category : public std::error_category
        {
        public:
            /*!
             * @brief Get the name of the error class.
             * @return The name of the error class.
             */
            const char* name( ) const noexcept override;
            /*!
             * @brief Get the name of the Vulkan error type.
             * @param ev The Vulkan error code.
             * @return The associated name with the error code.
             */
            std::string message( int ev ) const override;
        };
        
    public:
        /*!
         * @brief Ctor to create the vk_error.
         * @param result The Vulkan error code.
         * @param message The message to print along with the error name.
         */
        TWE_API vk_error( const vk::Result& result, const std::string& message );
    };
    
}

#endif //ENGINE_vk_error_H