#ifndef SPARK_VULKAN_VERTEX_HPP
#define SPARK_VULKAN_VERTEX_HPP

#include "../../ecs/component/vertex.hpp"
#include "../../spark.hpp"
#include "VULKAN/vulkan.h"

namespace Spark {
static internal::VkVertexInputBindingDescription get_binding_description() {
  internal::VkVertexInputBindingDescription binding_description = {};
  binding_description.binding = 0;
  binding_description.stride = sizeof(Vertex);
  binding_description.inputRate = internal::VK_VERTEX_INPUT_RATE_VERTEX;

  return binding_description;
}

static std::array<internal::VkVertexInputAttributeDescription, 3>
get_attribute_descriptions() {
  std::array<internal::VkVertexInputAttributeDescription, 3>
      attribute_descriptions = {};

  attribute_descriptions[0].binding = 0;
  attribute_descriptions[0].location = 0;
  attribute_descriptions[0].format = internal::VK_FORMAT_R32G32B32_SFLOAT;
  attribute_descriptions[0].offset = offsetof(Vertex, m_position);

  attribute_descriptions[1].binding = 0;
  attribute_descriptions[1].location = 1;
  attribute_descriptions[1].format = internal::VK_FORMAT_R32G32B32_SFLOAT;
  attribute_descriptions[1].offset = offsetof(Vertex, m_normal);

  attribute_descriptions[2].binding = 0;
  attribute_descriptions[2].location = 2;
  attribute_descriptions[2].format = internal::VK_FORMAT_R32G32_SFLOAT;
  attribute_descriptions[2].offset = offsetof(Vertex, m_texcoords);

  return attribute_descriptions;
}
} // namespace spark

#endif