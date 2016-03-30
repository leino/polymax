// NOTE:
// This is a not-very-random psuedo-random-number generator (PRNG) called "xorshift64star".
// It takes a (preferrably nonzero) 64-bit seed.
// Replace with any fast PRNG which is good enough for our purposes here (generating geometry).
inline uint64 prng_step(uint64 x)
{
    
	x ^= x >> 12;
	x ^= x << 25;
	x ^= x >> 27;
	return x * 2685821657736338717ULL;
    
}
