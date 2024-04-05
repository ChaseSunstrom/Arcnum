#ifndef SPARK_WINDOW_MANAGER_HPP
#define SPARK_WINDOW_MANAGER_HPP

#include "../spark.hpp"
#include "window.hpp"
#include "opengl_window.hpp"
#include "vulkan_window.hpp"
#include "metal_window.hpp"
#include "directx_window.hpp"

namespace spark
{
	enum class window_type
	{
		UNKNOWN = 0,
		OPENGL,
		VULKAN,
		METAL,
		DIRECTX
	};

	template <typename W>
	concept is_window_type = std::is_base_of_v<window, W>;

	inline window_type type_to_enum(const std::type_index& type)
	{
		if (type == typeid(opengl_window))
			return window_type::OPENGL;
		else if (type == typeid(vulkan_window))
			return window_type::VULKAN;
		else if (type == typeid(metal_window))
			return window_type::METAL;
		else if (type == typeid(directx_window))
			return window_type::DIRECTX;
		else
			return window_type::UNKNOWN;
	}

	class window_manager
	{
	public:
		static window_manager& get()
		{
			static window_manager instance;
			return instance;
		}
		template <is_window_type T>
		void add_window(const T& window)
		{
			m_windows[typeid(T)] = &window;

			if (m_windows.size() == 1)
				set_window<T>();
		}
		template <is_window_type T>
		void set_window()
		{
			// At to prevent accessing a window that hasnt been added yet
			m_current_window = m_windows.at(typeid(T));
		}
		std::type_index get_window_type()
		{
			return typeid(*m_current_window);
		}
		window& get_current_window()
		{
			return *m_current_window;
		}
		window_type get_current_window_type()
		{
			return type_to_enum(typeid(*m_current_window));
		}
		template <is_window_type T>
		bool is_window_type(window_type type)
		{
			return type == type_to_enum(T);
		}
	private:
		window_manager() = default;
		~window_manager() = default;
	private:
		window* m_current_window;
		// Needs window pointers because I cant store references in a unordered_map
		// its a pointer to the static windows anyway, it should be fine
		std::unordered_map<std::type_index, window*> m_windows;
	};

	std::type_index get_window_type()
	{
		auto& _window_manager = engine::get<window_manager>();

		return _window_manager.get_window_type();
	}
}

#endif