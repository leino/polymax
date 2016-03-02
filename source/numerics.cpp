namespace Numerics
{

    inline float
    sin_float(float x)
    {
        // TODO: intrinsic
        return sinf(x);
    }

    inline float
    lerp_float(float const from, float const to, float const t)
    {
        return from + (to - from)*t;
    }
    
    inline float
    floor_float(float const x)
    {
        // TODO: intrinsic
        return floorf(x);
    }

    inline float
    exp_float(float const x)
    {
        // TODO: intrinsic
        return expf(x);
    }    
    
    inline float
    cos_float(float x)
    {
        // TODO: intrinsic
        return cosf(x);
    }

    inline float
    tan_float(float x)
    {
        // TODO: intrinsic
        return tanf(x);
    }    
    
    inline float
    pow_float(float base, float exponent)
    {
        // TODO: intrinsic
        return powf(base, exponent);
    }


    inline float
    sqrt_float(float x)
    {
        // TODO: intrinsic
        return sqrtf(x);
    }

    inline int
    sign_int(int const x)
    {
        if(x < 0)
        {
            return -1;
        }
        else if(x > 0)
        {
            return +1;
        }
        else
        {
            return 0;
        }
    }
    
// NOTE: inclusive bounds
    inline int
    clamped_int(int const min, int const max, int const x)
    {
        assert( min <= max );
        if( x <= min )
        {
            return min;
        }
        else if( x >= max )
        {
            return max;
        }
        else
        {
            return x;
        }
    }

    inline float
    abs_float(float x)
    {
        // TODO: intrinsic?
        if( x < 0.0f )
        {
            return -x;
        }
        else
        {
            return +x;
        }
    }
    
    inline float
    clamped_float(float min, float max, float x)
    {
        // TODO: intrinsic?
        if(x <= min)
        {
            return min;
        }
        else if(x >= max)
        {
            return max;
        }
        else
        {
            return x;
        }
    }

    inline uint
    max_uint(uint a, uint b)
    {
        if(a > b)
        {
            return a;
        }
        else
        {
            return b;
        }
    }    
    
    inline int
    max_int(int a, int b)
    {
        if(a > b)
        {
            return a;
        }
        else
        {
            return b;
        }
    }

    inline int
    min_int(int a, int b)
    {
        if(a < b)
        {
            return a;
        }
        else
        {
            return b;
        }
    }    
    
    inline float
    max_float(float a, float b)
    {
        // TODO: intrinsic?
        if(a > b)
        {
            return a;
        }
        else
        {
            return b;
        }
    }

    inline float
    min_float(float a, float b)
    {
        // TODO: intrinsic?
        if(a < b)
        {
            return a;
        }
        else
        {
            return b;
        }
    }    

    // NOTE: using the "round down" convention
    inline float
    remainder_float(float const modulus, float const x)
    {
        assert(modulus > 0.0f);

        float const quotient = floor_float(x/modulus);
        float const result = x - quotient*modulus;
        
        assert(result >= 0.0f);
        assert(result <= modulus);
        
        return result;
    }    

    // NOTE: using the "round down" convention    
    inline int
    remainder_int(int const modulus, int const x)
    {
        assert(modulus > 0);

        int result = -1;
        // TODO: intrinsic?
        if( x >= 0 )
        {
            result = x % modulus;
        }
        else
        {
            result = modulus - ((-x) % modulus);
        }

        assert(result >= 0);
        assert(result < modulus);
    
        return result;
    }

    inline float
    log_float(float const base, float const x)
    {
        // TODO: intrinsic?
        // NOTE:
        // x = b^log_base(x) = (10^logf(b))^log_base(x) = 10^(logf(b) * log_base(x))
        // logf(x) = logf(b) * log_base(x)
        // log_base(x) = logf(x) / logf(b)
        float const log_base_inv = 1.0f / logf(base);
        float const result = logf(x) * log_base_inv;
        return result;
    }

    inline float
    square_float(float const x)
    {
        return pow_float(x, 2.0f);
    }

};
