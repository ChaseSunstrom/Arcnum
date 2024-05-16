#ifndef SPARK_Math_RANDOM_HPP
#define SPARK_Math_RANDOM_HPP

#include "../spark.hpp"

namespace Spark
{
namespace Math
{
enum class Coin
{
    TAILS = 0,
    HEADS = 1
};

u32 roll_dice(const u32 min, const u32 max);
std::vector<u32> multi_dice_roll(const u32 min, const u32 max, const u32 num_dice);
std::vector<u32> roll_unfair_dice(const u32 min, const u32 max, const std::vector<f64> &percentages);
Coin flip_coin();
Coin flip_unfair_coin(const f64 percentage);

template <typename T> class RandomGenerator
{
  private:
    std::random_device m_rd;
    std::mt19937 m_gen;
    std::uniform_real_distribution<T> m_dis;

  public:
    RandomGenerator(T min, T max) : m_gen(m_rd()), m_dis(min, max)
    {
    }

    T generate()
    {
        return m_dis(m_gen);
    }

    T generate(T min, T max)
    {
        return std::uniform_real_distribution<T>(min, max)(m_gen);
    }

    std::vector<T> generate_sequence(u64 count)
    {
        std::vector<T> sequence(count);
        for (auto &value : sequence)
        {
            value = generate();
        }
        return sequence;
    }

    std::vector<T> generate_sequence(u64 count, T min, T max)
    {
        std::vector<T> sequence(count);
        for (auto &value : sequence)
        {
            value = generate(min, max);
        }
        return sequence;
    }
};

template <typename T> class WeightedRandomGenerator
{
  private:
    std::random_device m_rd;
    std::mt19937 m_gen;
    std::discrete_distribution<std::size_t> m_dis;
    std::vector<T> m_values;

  public:
    WeightedRandomGenerator(const std::vector<T> &values, const std::vector<f64> &weights)
        : m_gen(m_rd()), m_dis(weights.begin(), weights.end()), m_values(values)
    {
    }

    T generate()
    {
        return m_values[m_dis(m_gen)];
    }

    std::vector<T> generate_sequence(u64 count)
    {
        std::vector<T> sequence(count);
        for (auto &value : sequence)
        {
            value = generate();
        }
        return sequence;
    }
};

template <typename T> class NormalDistributionGenerator
{
  private:
    std::random_device m_rd;
    std::mt19937 m_gen;
    std::normal_distribution<T> m_dis;

  public:
    NormalDistributionGenerator(T mean, T stddev) : m_gen(m_rd()), m_dis(mean, stddev)
    {
    }

    T generate()
    {
        return m_dis(m_gen);
    }

    std::vector<T> generate_sequence(u64 count)
    {
        std::vector<T> sequence(count);
        for (auto &value : sequence)
        {
            value = generate();
        }
        return sequence;
    }
};

} // namespace Math
} // namespace Spark

#endif // SPARK_Math_RANDOM_HPP
