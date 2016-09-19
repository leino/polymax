#if defined(ENSURE_DEBUGBREAK_ON_ERROR)
#define ENSURE(expr) if(!(expr)){__debugbreak();}
#else
#define ENSURE(expr)
#endif

#define ENSURE_EQUIVALENT(a, b) ENSURE((a) == (b));
