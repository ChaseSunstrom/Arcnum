#include "audio.hpp"
#include "../app/app.hpp"

namespace spark
{
	void audio_system::on_update(f64 delta_time)
	{
		audio_manager& audio_man = engine::get<audio_manager>();

		for (const auto& component: m_audio_components)
		{
			if (!component.has_value())
				continue;

			if (component.value().m_play_condition())
			{
				audio_man.play_sound(component.value().m_name);
			}
		}
	}
}