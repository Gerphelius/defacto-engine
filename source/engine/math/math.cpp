#include <random>

namespace DF
{

DF_API float Map(float value, float low1, float high1, float low2, float high2)
{
    return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
}

static std::random_device rd;
static std::seed_seq seed { rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd() };
static std::mt19937 mt { seed };

DF_API int RandomInt(int min = 0, int max = INT_MAX)
{
    std::uniform_int_distribution<int> uniform_dist(min, max);

    return uniform_dist(mt);
}

} // namespace DF
