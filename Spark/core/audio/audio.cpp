#include "audio.hpp"
#include "../app/app.hpp"

namespace spark
{
	void audio_system::on_update(f64 delta_time)
	{
		audio_manager& audio_man = engine::get<audio_manager>();

		for (const auto& component: m_audio_components)
		{
			if (component.m_play_condition(m_component_manager))
			{
				audio_man.play_sound(component.m_name);
			}
		}
	}
}