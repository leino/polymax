inline float
uniform_interval(uint64* seed, float a, float b)
{

    float const uint64_max_float = (float)UINT64_MAX;
    float const random_normalized = (float)(*seed)/uint64_max_float;
    float const random = Numerics::lerp_float(a, b, random_normalized);
    ENSURE(random >= a);
    ENSURE(random <= b);
    *seed = prng_step(*seed);
    return random;
}

inline int
uniform_range(uint64 *const seed, int const lo, int const hi)
{
    ENSURE(lo <= hi);

    int const offset = Numerics::remainder(hi - lo + 1, Numerics::abs((int)*seed));
    int const sample = lo + offset;
    ENSURE(sample >= lo);
    ENSURE(sample <= hi);
    *seed = prng_step(*seed);
    return sample;
}

inline bool
uniform_bool(uint64* seed)
{
    bool const result = *seed & 1;
    *seed = prng_step(*seed);
    return result;
}

// NOTE: p is the probability for true
inline bool
distribution_bool(uint64* seed, float const p)
{
    // TODO: precision problems?
    float const threshold_float = p*(float)UINT64_MAX;
    uint64 const threshold_uint64 = (uint64)threshold_float;
    bool const result = *seed <= threshold_uint64;
    *seed = prng_step(*seed);
    return result;
}
