/*

  This is a maximum finder specifically for finding the maxium of a real-valued polynomial.

  Note that the root finder is not to be used as a general-purpose root finder.
  It makes some simplifying assumptions that can only be made for the purposes of finding maxima of polynomials.
  In particular, it may not find all roots, it is only "guaranteed" to find those roots which are relevant 
  for maximizing the polynomial.

*/


namespace Polymax
{
    
    inline bool
    safely_positive(float const epsilon, float const x)
    {
        ENSURE(epsilon > 0.0f);
        return x > +epsilon;
    }

    inline bool
    safely_negative(float const epsilon, float const x)
    {
        ENSURE(epsilon > 0.0f);
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
    
    inline float
    isolated_root(
        float min_x, float max_x,
        float const coefficients[MAX_DEGREE][MAX_DEGREE],
        int const degree,
        float const epsilon = 1e-6f
        )
    {
        ENSURE(degree > 0);

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

				ENSURE(min_x <= max_x);

                interval_length = Numerics::abs(max_x - min_x);
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

				ENSURE(min_x <= max_x);

                interval_length = Numerics::abs(max_x - min_x);
                if(interval_length < epsilon)
                    break;
                
            }            
            
        }

        float const mid_x = (min_x + max_x)/2.0f;
		ENSURE(mid_x >= min_x);
		ENSURE(mid_x <= max_x);

//#define FINAL_BISECTION


        
#if defined(FINAL_BISECTION)
        
        // NOTE: the last thing we do is to bisect the x axis linearly in-between min_x and max_x:        
        float const slope_mid_x = evaluate(coefficients[degree-1], degree-1, mid_x);
        float const mid_y = evaluate(coefficients[degree], degree, mid_x);
        float const x = mid_x - mid_y/slope_mid_x;

        // TODO: find out why these sometimes fail when we do the final bisection
		ENSURE(x >= min_x);
		ENSURE(x >= max_x);
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
        float roots[MAX_DEGREE][MAX_DEGREE],
        float const bracket_epsilon = 1e-10f,
        float const isolated_root_finder_epsilon = 1e-6f
        )
    {
        using namespace Numerics;
        
        ENSURE(min_x < max_x);
        ENSURE(highest_degree > 0);

        int num_roots = 0;
        
        // NOTE: Try to find the linear root if it exists inside the interval.
        {
            int const degree = 1;
            float const root = -coefficients[degree][0]/coefficients[degree][1];
            if(root > min_x && root < max_x)
            {
                roots[degree][0] = root;
                num_roots++;
            }
        }

        // NOTE:
        // Try to find roots of successively lower derivatives.
        // The roots of the one-higher derivative at each step provides
        // a partition of our interval into pieces of monotone increase/decrease, which
        // helps to provide brackets to the roots we want to find.
        for(int degree = 2; degree <= highest_degree; degree++)
        {

            // NOTE: All roots of derivative which are inside our interval.
            int const num_derivative_roots = num_roots;

            // NOTE: Points inside our interval which give us our partition.
            int const num_internal_points = num_derivative_roots;
            
            num_roots = 0;

            // NOTE: our first bracket candidate is always the left limit of the interval
            float lo_x = min_x;
            float lo_y = evaluate(coefficients[degree], degree, lo_x);

            // NOTE: skip candidates so long as the left bracket candidate is small
            int lo_idx = -1;
            while(abs(lo_y) < bracket_epsilon)
            {
                lo_idx++;
                if(lo_idx == num_internal_points)
                    goto level_done;
                lo_x = roots[degree-1][num_internal_points];
                lo_y = evaluate(coefficients[degree], degree, lo_x);
            }

            while(true)
            {

                int hi_idx = lo_idx;
                if(hi_idx == num_internal_points)
                    goto level_done;

                float hi_x;
                float hi_y;

                // NOTE: search for the high limit, or if none can be found we skip to the next level
                do
                {                    
                    hi_idx++;
                    if(hi_idx == num_internal_points)
                    {
                        hi_x = max_x;
                        hi_y = evaluate(coefficients[degree], degree, hi_x);
                        if(abs(hi_y) < bracket_epsilon)
                            goto level_done;
                    }
                    else
                    {
                        hi_x = roots[degree-1][hi_idx];
                        hi_y = evaluate(coefficients[degree], degree, hi_x);
                    }
                }
                while(abs(hi_y) < bracket_epsilon);

                // NOTE: We now know that our bracket does not take small values at it's limits:
                ENSURE(abs(lo_y) >= bracket_epsilon);
                ENSURE(abs(hi_y) >= bracket_epsilon);
                // ... however, we still have to check if it actually contains a zero, by comparing a signs
                if(
                    (lo_y < float(0) && hi_y > float(0)) ||
                    (lo_y > float(0) && hi_y < float(0))
                    )
                {
                    // NOTE:
                    // The interval [lo_x, hi_x] brackets a zero, and does not take small values at it's limits.
                    // In fact it is an isolated zero because
                    // the polynomial is monotonically increasing or decreasing on [x_lo, x_hi], so
                    // we use our isolated root finder.

                    float const root = isolated_root(lo_x, hi_x, coefficients, degree, isolated_root_finder_epsilon);
                    ENSURE(root >= min_x && root <= max_x);
                    ENSURE(root > lo_x && root < hi_x);

                    ENSURE(num_roots < MAX_DEGREE);
                    roots[degree][num_roots] = root;
                    num_roots++;

                }

                // NOTE: low bracket limit for next root is just the high limit of the current bracket
                lo_idx = hi_idx;
                lo_x = hi_x;
                lo_y = hi_y;
                
            }

        level_done:;
            
        }
        
        return num_roots;
    }

    void
    maximum(
        float const polynomial_coefficients[MAX_DEGREE],
        int const polynomial_degree,
        float const x_min, float const x_max,
        float *const maximum_x,
        float *const maximum_y
        )
    {

        float derivative_coefficients[MAX_DEGREE][MAX_DEGREE] = {};
        float roots[MAX_DEGREE][MAX_DEGREE] = {};

        ENSURE(polynomial_degree <= MAX_DEGREE);
        ENSURE(polynomial_degree > 1);

        // TODO: get rid of this step, just compute the first derivative separately
        for(int coefficient_idx=0; coefficient_idx <= polynomial_degree; coefficient_idx++)
            derivative_coefficients[polynomial_degree][coefficient_idx] = polynomial_coefficients[coefficient_idx];
        
        // NOTE: compute all of the derivatives
        // TODO: stop when derivative order goes below linear
        for(int derivative_degree = polynomial_degree-1; derivative_degree >= 0; derivative_degree--)
        {
            for(int coefficient_idx=0; coefficient_idx <= derivative_degree; coefficient_idx++)
            {
                derivative_coefficients[derivative_degree][coefficient_idx] =
                    float(coefficient_idx+1)*derivative_coefficients[derivative_degree+1][coefficient_idx+1];
            }
        }

        float const bracket_epsilon = 1e-10f;
        float const isolated_root_finder_epsilon = 1e-6f;
        int const num_found_roots =
            try_find_roots(
                x_min,
                x_max,
                derivative_coefficients,
                polynomial_degree-1,
                roots,
                bracket_epsilon,
                isolated_root_finder_epsilon
                );

        float const x_min_y = evaluate(polynomial_coefficients, polynomial_degree, x_min);
        float const x_max_y = evaluate(polynomial_coefficients, polynomial_degree, x_max);
        
        if(x_min_y > x_max_y)
        {
            *maximum_x = x_min;            
            *maximum_y = x_min_y;
        }
        else
        {
            *maximum_x = x_max;            
            *maximum_y = x_max_y;
        }
            
        for(int root_idx=0; root_idx < num_found_roots; root_idx++)
        {
            float const x = roots[polynomial_degree-1][root_idx];
            float const y = evaluate(polynomial_coefficients, polynomial_degree, x);
            if(y > *maximum_y)
            {
                *maximum_x = x;
                *maximum_y = y;
            }
        }
        
    }    

    void
    maximum(
        float const polynomial_coefficients[MAX_DEGREE],
        int const polynomial_degree,
        float const interval[2],
        float *const maximum_x,
        float *const maximum_y
        )
    {
        maximum(polynomial_coefficients, polynomial_degree, interval[0], interval[1], maximum_x, maximum_y);
    }    
    
}
