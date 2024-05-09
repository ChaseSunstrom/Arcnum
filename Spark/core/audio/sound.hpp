#ifndef SPARK_SOUND_HPP
#define SPARK_SOUND_HPP

#include "../spark.hpp"

namespace Spark
{
class Audio
{
  public:
    Audio(internal::ISound *s) : m_sound(s)
    {
        m_sound->grab();
    }

    Audio(Audio &sound)
    {
        m_sound = sound.m_sound;
        m_sound->grab();
    }

    ~Audio()
    {
        m_sound->drop();
    }

    Audio &operator=(Audio &sound)
    {
        m_sound = sound.m_sound;
        m_sound->grab();
    }

    internal::ISound *get_raw_sound() const
    {
        return m_sound;
    }

    void set_position(const math::vec3 &position)
    {
        internal::vec3df position_3d(position.x, position.y, position.z);
        m_sound->setPosition(position_3d);
    }

    void pause()
    {
        m_sound->setIsPaused(true);
    }

    void resume()
    {
        m_sound->setIsPaused(false);
    }

    void loop()
    {
        m_sound->setIsLooped(true);
    }

    void stop()
    {
        m_sound->setIsLooped(false);
    }

    void set_volume(f32 volume)
    {
        m_sound->setVolume(volume);
    }

    void set_pan(f32 pan)
    {
        m_sound->setPan(pan);
    }

    bool is_playing()
    {
        return !m_sound->isFinished();
    }

  private:
    internal::ISound *m_sound;
};

} // namespace Spark

#endif