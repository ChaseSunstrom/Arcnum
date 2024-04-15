#ifndef SPARK_AUDIO_HPP
#define SPARK_AUDIO_HPP

#include "sound.hpp"

#include "../ecs/system/system.hpp"

namespace spark
{
	class audio_manager
	{
	public:

		static audio_manager& get()
		{
			static audio_manager instance;
			return instance;
		}

		sound& create_sound(
				const std::string& name,
				const std::filesystem::path& source,
				bool loops = false,
				bool pause_on_create = true)
		{
			std::unique_ptr<sound> s = std::make_unique<sound>(
					m_engine->play2D(
							source.string().c_str(),
							loops,
							pause_on_create,
							true));
			m_sounds[name] = std::move(s);
			return *m_sounds[name];
		}

		sound& create_sound(
				const std::string& name,
				const std::filesystem::path& source,
				const math::vec3& position,
				bool loops = false,
				bool pause_on_create = true)
		{
			vec3df position_3d(position.x, position.y, position.z);

			std::unique_ptr<sound> s = std::make_unique<sound>(
					m_engine->play3D(
							source.string().c_str(),
							position_3d,
							loops,
							pause_on_create));

			m_sounds[name] = std::move(s);
			return *m_sounds[name];
		}

		sound& get_sound(const std::string& name)
		{
			return *m_sounds[name];
		}

		void play_sound(const std::string& name, bool loops = false)
		{
			sound& s = *m_sounds[name];

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
			sound& s = *m_sounds[name];

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
			m_engine->stopAllSounds();
		}

		void set_sound_pan(const std::string& name, f32 pan)
		{
			m_sounds[name]->set_pan(pan);

			m_engine->update();
		}

		void set_sound_volume(const std::string& name, f32 volume)
		{
			m_sounds[name]->set_volume(volume);

			m_engine->update();
		}

		void set_sound_position(const std::string& name, const math::vec3& position)
		{
			m_sounds[name]->set_position(position);

			m_engine->update();
		}

		ISoundEngine* get_engine() const
		{
			return m_engine;
		}

	private:
		audio_manager() = default;

		~audio_manager()
		{
			m_engine->drop();
		}

	private:
		ISoundEngine* m_engine = createIrrKlangDevice();

		std::unordered_map<std::string, std::unique_ptr<sound>> m_sounds;
	};

	struct audio_component
	{
		audio_component() = default;

		audio_component(const std::string& name, const std::function<bool(component_manager&)>& play_condition) :
				m_name(name), m_play_condition(play_condition) { }

		std::string m_name;

		std::function<bool(component_manager&)> m_play_condition;
	};

	class audio_system :
			public system
	{
	public:
		audio_system(component_manager& component_manager, std::vector<audio_component>* audio_components) :
				system(), m_component_manager(component_manager), m_audio_components(*audio_components) { }

		void on_update(f64 delta_time);

	private:
		std::vector<audio_component>& m_audio_components;

		component_manager& m_component_manager;
	};
}

#endif