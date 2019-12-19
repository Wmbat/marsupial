/**
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

#ifndef LUCIOLE_VK_PIPELINES_PIPELINE_DATA_HPP
#define LUCIOLE_VK_PIPELINES_PIPELINE_DATA_HPP

#include <luciole/vk/pipelines/pipeline.hpp>

#include <vulkan/vulkan.h>

namespace vk::pipeline
{
   struct data
   {
      pipeline::type pipeline_type;
   };
} // namespace vk::pipeline

#endif // LUCIOLE_VK_PIPELINES_PIPELINE_DATA_HPP