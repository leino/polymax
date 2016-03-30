inline float
uint32_as_float(uint32 const a)
{
    static_assert(sizeof(uint32) == sizeof(float), "float needs to be 32 bits");
    
    union
    {
        float f;
        uint32 u;
    } x;

    x.u = a;
    return x.f;
}
