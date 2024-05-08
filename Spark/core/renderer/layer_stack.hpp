#ifndef SPARK_LAYER_STACK_HPP
#define SPARK_LAYER_STACK_HPP

#include "layer.hpp"

namespace Spark {
// ==============================================================================
// LAYER STACK:   | Used for different application layers
// ==============================================================================

class LayerStack {
public:
  LayerStack() = default;

  ~LayerStack() = default;

  template <typename T, typename... Args> void push_layer(Args &&...args) {
    auto layer = std::make_unique<T>(std::forward<Args>(args)...);
    layer->on_attach();
    m_layers.emplace_back(std::move(layer));
  }

  template <typename T, typename... Args> void push_overlay(Args &&...args) {
    auto overlay = std::make_unique<T>(std::forward<Args>(args)...);
    overlay->on_attach();
    m_layers.emplace_back(std::move(overlay));
  }

  inline void pop_layer() { m_layers.pop_back(); }

  inline void pop_overlay() { m_layers.pop_back(); }

  inline std::vector<std::unique_ptr<Layer>> &get_layers() { return m_layers; }

private:
  std::vector<std::unique_ptr<Layer>> m_layers =
      std::vector<std::unique_ptr<Layer>>();
};

} // namespace spark

#endif // CORE_LAYER_STACK_H