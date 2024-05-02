#ifndef SPARK_AUDIO_HPP
#define SPARK_AUDIO_HPP

#include "sound.hpp"

#include "../util/singleton.hpp"
#include "../ecs/system/system.hpp"

namespace spark
{
	class AudioManager :
		public Singleton<AudioManager>
	{
	public:

		static AudioManager& get()
		{
			static AudioManager instance;
			return instance;
		}

		Sound& create_sound(
				const std::string& name,
				const std::filesystem::path& source,
				bool loops = false,
				bool pause_on_create = true)
		{
			std::unique_ptr<Sound> s = std::make_unique<Sound>(
					m_Engine->play2D(
							source.string().c_str(),
							loops,
							pause_on_create,
							true));
			m_sounds[name] = std::move(s);
			return *m_sounds[name];
		}

		Sound& create_sound(
				const std::string& name,
				const std::filesystem::path& source,
				const math::vec3& position,
				bool loops = false,
				bool pause_on_create = true)
		{
			vec3df position_3d(position.x, position.y, position.z);

			std::unique_ptr<Sound> s = std::make_unique<Sound>(
					m_Engine->play3D(
							source.string().c_str(),
							position_3d,
							loops,
							pause_on_create));

			m_sounds[name] = std::move(s);
			return *m_sounds[name];
		}

		Sound& get_sound(const std::string& name)
		{
			return *m_sounds[name];
		}

		void play_sound(const std::string& name, bool loops = false)
		{
			Sound& s = *m_sounds[name];

			if (s.get_raw_sound()->isFinished())
			{
				s.get_raw_sound()->setPlayPosition(0);
				s.get_raw_sound()->setIsLooped(loops);
				s.resume();
			}
			else
			{
				s.resume();
			}
		}

		void play_sound(const std::string& name, const math::vec3& position, bool loops = false)
		{
			Sound& s = *m_sounds[name];

			if (s.get_raw_sound()->isFinished())
			{
				s.get_raw_sound()->setPlayPosition(0);
				s.get_raw_sound()->setIsLooped(loops);
				s.set_position(position);
				s.resume();
			}
			else
			{
				s.resume();
			}
		}

		void pause_sound(const std::string& name)
		{
			m_sounds[name]->pause();
		}

		void resume_sound(const std::string& name)
		{
			m_sounds[name]->resume();
		}

		void loop_sound(const std::string& name)
		{
			m_sounds[name]->loop();
		}

		void stop_sound(const std::string& name)
		{
			m_sounds[name]->stop();
		}

		void stop_all()
		{
			m_Engine->stopAllSounds();
		}

		void set_sound_pan(const std::string& name, f32 pan)
		{
			m_sounds[name]->set_pan(pan);

			m_Engine->update();
		}

		void set_sound_volume(const std::string& name, f32 volume)
		{
			m_sounds[name]->set_volume(volume);

			m_Engine->update();
		}

		void set_sound_position(const std::string& name, const math::vec3& position)
		{
			m_sounds[name]->set_position(position);

			m_Engine->update();
		}

		ISoundEngine* get_Engine() const
		{
			return m_Engine;
		}

	private:
		AudioManager() = default;

		~AudioManager()
		{
			m_Engine->drop();
		}

	private:
		ISoundEngine* m_Engine = createIrrKlangDevice();

		std::unordered_map<std::string, std::unique_ptr<Sound>> m_sounds;
	};

	struct AudioComponent
	{
		AudioComponent() = default;

		AudioComponent(const std::string& name, const std::function<bool()>& play_condition) :
				m_name(name), m_play_condition(play_condition) { }

		std::string m_name;

		std::function<bool()> m_play_condition;
	};

	class AudioSystem :
			public System
	{
	public:
		AudioSystem(std::vector<std::optional<AudioComponent>>* audio_components) :
				System(), m_audio_components(*audio_components) { }

		void on_update(f64 delta_time);

	private:
		std::vector<std::optional<AudioComponent>>& m_audio_components;
	};
}

#endif