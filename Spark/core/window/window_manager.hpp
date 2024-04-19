#ifndef SPARK_WINDOW_MANAGER_HPP
#define SPARK_WINDOW_MANAGER_HPP

#include "../spark.hpp"
#include "window.hpp"
#include "vulkan/vulkan_window.hpp"
#include "metal/metal_window.hpp"
#include "window_type.hpp"
#include "directx/directx_window.hpp"

namespace spark
{
	template <typename W> concept is_window_type = std::is_base_of_v<window, W>;

	class window_manager
	{
	public:
		static window_manager& get()
		{
			static window_manager instance;
			return instance;
		}

		template <is_window_type T>
		void add_window(T& window)
		{
			m_windows[typeid(T)] = &window;

			if (m_windows.size() == 1)
			{
				set_window<T>();
			}
		}

		template <is_window_type T>
		void set_window()
		{
			m_current_window = m_windows[typeid(T)];
		}

		window& get_current_window() const
		{
			return *m_current_window;
		}

		window_type get_current_window_type() const
		{
			return m_current_window->get_window_type();
		}

		template <is_window_type T>
		bool is_window_type(window_type type) const
		{
			return type == type_to_enum(T);
		}

	private:
		window_manager()
		{
			auto& vk_window = engine::get<vulkan_window>();
			add_window(vk_window);
		}

		~window_manager() = default;

	private:
		window* m_current_window;

		// Needs window pointers because you cant store references in an unordered_map
		// its a pointer to the static windows anyway, it should be fine
		std::unordered_map<std::type_index, window*> m_windows;
	};
}

#endif