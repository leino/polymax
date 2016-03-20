namespace Polyroot
{
    
    inline bool
    safely_positive(float const epsilon, float const x)
    {
        assert(epsilon > 0.0f);
        return x > +epsilon;
    }

    inline bool
    safely_negative(float const epsilon, float const x)
    {
        assert(epsilon > 0.0f);
        return x < -epsilon;
    }

    inline bool
    interval_safely_contains_zero(float const epsilon, float const x_lo, float const x_hi)
    {
        return
            ( safely_negative(epsilon, x_lo) && safely_positive(epsilon, x_hi) ) ||
            ( safely_positive(epsilon, x_lo) && safely_negative(epsilon, x_hi) );
    }

    inline float
    evaluate(float const*const coefficients, int const degree, float const x)
    {
        int i = degree;
        float y = coefficients[i];
        while(i > 0)
        {
            y *= x;
            i--;            
            y += coefficients[i];
        }
        return y;
    }

    // NOTE: Newton's method
    inline float
    isolated_root(
        float min_x, float max_x,
        float const coefficients[MAX_DEGREE][MAX_DEGREE],
        int const degree
        )
    {
        assert(degree > 0);
        
		float const epsilon = 1e-6f;

        float const min_y = evaluate(coefficients[degree], degree, min_x);
        float const max_y = evaluate(coefficients[degree], degree, max_x);

        float interval_length;
        
        if( min_y < max_y )
        {
            
            while(true)
            {

                float const x = (min_x + max_x)/2.0f;                
                
                float const y = evaluate(coefficients[degree], degree, x);
                
                if( y < 0.0f )
                {
                    min_x = x;
                }
                else
                {
                    max_x = x;
                }

                interval_length = Numerics::abs_float(max_x - min_x);
                if(interval_length < epsilon)
                    break;
                
            }
            
        }
        else
        {

            while(true)
            {
                
                float const x = (min_x + max_x)/2.0f;
                
                float const y = evaluate(coefficients[degree], degree, x);
                
                if( y > 0.0f )
                {
                    min_x = x;
                }
                else
                {
                    max_x = x;
                }

                interval_length = Numerics::abs_float(max_x - min_x);
                if(interval_length < epsilon)
                    break;
                
            }            
            
        }

        // NOTE: the last thing we do is to bisect the x axis linearly in-between min_x and max_x:
        float const mid_x = (min_x + max_x)/2.0f;        
#if 1
        float const slope_mid_x = evaluate(coefficients[degree-1], degree-1, mid_x);
        float const mid_y = evaluate(coefficients[degree], degree, mid_x);
        float const x = mid_x - mid_y/slope_mid_x;
        return x;
#else
        return mid_x;
#endif
        
    }
    
    inline int
    try_find_roots(
        float const min_x,
        float const max_x,
        float coefficients[MAX_DEGREE][MAX_DEGREE],
        int const highest_degree,
        float roots[MAX_DEGREE][MAX_DEGREE]
        )
    {
        using namespace Numerics;
        
        assert(min_x < max_x);
        assert(highest_degree > 0);

        int num_roots = 0;
        
        // NOTE: try to find the linear root
        {
            // 0 = p->coefficients[0] + p->coefficients[1] * x
            // Solving for x, we get
            int const degree = 1;
            float const root = -coefficients[degree][0]/coefficients[degree][1];
            if(root >= min_x && root <= max_x)
            {
                roots[degree][0] = root;
                num_roots++;
            }
        }
        
        for(int degree = 2; degree <= highest_degree; degree++)
        {

            int const num_roots_prev = num_roots;
            num_roots = 0;
            for(int idx=0; idx < num_roots_prev + 1; idx++)
            {
            
                int const idx_lo = idx + 0;
                int const idx_hi = idx + 1;
            
                float const x_lo = (idx == 0) ? min_x : roots[degree-1][idx-1];
                float const x_hi = (idx == num_roots_prev) ? max_x : roots[degree-1][idx];
                assert(x_lo < x_hi);

                // OPTIMIZE: evaluate together, cache results for next iteration
                float const y_lo = evaluate(coefficients[degree], degree, x_lo);
                float const y_hi = evaluate(coefficients[degree], degree, x_hi);

                float const epsilon = 1e-6f; // TODO: pass in
                if(interval_safely_contains_zero(epsilon, y_lo, y_hi))
                {
                    // TODO: think about if/when this can be guaranteed to succeed                
                    float const root = isolated_root(x_lo, x_hi, coefficients, degree);

                    // NOTE: we ignore the root if it's outside of the interval of interest
                    if(root >= min_x && root <= max_x)
                    {
                        roots[degree][num_roots] = root;
                        num_roots++;
                    }
                    
                }
            
            }
            
        }
        
        return num_roots;
    }
    
}
