#include "audio.hpp"
#include "../app/app.hpp"

namespace spark
{
	void AudioSystem::on_update(f64 delta_time)
	{
		AudioManager& audio_man = Engine::get<AudioManager>();

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