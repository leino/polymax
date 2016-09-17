#define ENSURE(expr) if(!(expr)){__debugbreak();}
#define ENSURE_EQUIVALENT(a, b) ENSURE((a) == (b));
