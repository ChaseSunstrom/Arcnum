#ifndef SPARK_LAYER_STACK_HPP
#define SPARK_LAYER_STACK_HPP

#include "spark_pch.hpp"
#include "spark_layer.hpp"

namespace spark
{
	class LayerStack
	{
	public:
		using Iterator = std::vector<std::unique_ptr<Layer>>::iterator;

		LayerStack() = default;
		~LayerStack() = default;
		
		template <typename Ty, typename... Args>
		void PushLayer(const Args&... args)
		{
			static_assert(std::is_base_of_v<Layer, Ty>, "Ty must derive from Layer");
			m_layers.emplace_back(std::make_unique<Ty>(args...));
			m_layers.back()->OnAttach();
		}

		void PopLayer()
		{
			m_layers.back()->OnDetach();
			m_layers.pop_back();
		}

		template <typename Ty, typename... Args>
		void InsertLayer(usize index, const Args&... args)
		{
			static_assert(std::is_base_of_v<Layer, Ty>, "Ty must derive from Layer");
			m_layers.emplace(m_layers.begin() + index, std::make_unique<Ty>(args...));
			m_layers.begin()[index]->OnAttach();
		}
		
		void RemoveLayer(usize index)
		{
			m_layers.begin()[index]->OnDetach();
			m_layers.erase(m_layers.begin() + index);
		}

		Layer& operator[](usize index) { return *m_layers.begin()[index]; }

		Iterator begin() { return m_layers.begin(); }
		Iterator end() { return m_layers.end(); }
	private:
		std::vector<std::unique_ptr<Layer>> m_layers;
	};
}

#endif