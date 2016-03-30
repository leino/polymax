/*

  This is a very special purpose root finder.

  The purpose is to apply it to the derivative of a polynomial with the final goal of finding a finite set of candidates
  for minima/maxima inside the interval.

  This root finder is concerned exlusively with finding out all x values inside a given interval, 
  such that a polynomial is zero at those values.

  Thanks to focusing on this one speficic use-case, certain simplifications in the implementation can been made.
  One is that roots which may lay extremely close to the edges of the interval may be missed.
  We don't care, since we will be including these boundaries as candidates when checking for minima/maxima anyway.
  
  Another simplification is that we don't care about distinguishing all the roots, for instance we don't care much
  if we find only one out of two distinct roots which are extremely close together. 
  If several roots are extremely close together and we found only one of them, the potential effect on the final minimum/maximum will be small, because
  the missed candidate(s) would give very nearly the same result when evaluated.

  For similar reasons, we also don't care to find out wether the zeros we do find are degenerate, or what their exact multiplicities may be.

*/


namespace Polymax
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

                interval_length = Numerics::abs(max_x - min_x);
                if(interval_length < epsilon)
                    break;
                
            }            
            
        }

        float const mid_x = (min_x + max_x)/2.0f;        
#if 1
        // NOTE: the last thing we do is to bisect the x axis linearly in-between min_x and max_x:        
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

            float const epsilon = 1e-10f; // TODO: pass in

            // NOTE: All roots of derivative which are inside our interval.
            int const num_derivative_roots = num_roots;

            // NOTE: Points inside our interval which give us our partition.
            int const num_internal_points = num_derivative_roots;
            
            num_roots = 0;

            float lo_x = min_x;
            float lo_y = evaluate(coefficients[degree], degree, lo_x);

            int lo_idx = -1;
            while(abs(lo_y) < epsilon)
            {
                lo_idx++;
                if(lo_idx == num_internal_points)
                    goto done;
                lo_x = roots[degree-1][num_internal_points];
                lo_y = evaluate(coefficients[degree], degree, lo_x);
            }

            while(true)
            {

                int hi_idx = lo_idx;
                if(hi_idx == num_internal_points)
                    goto done;

                float hi_x;
                float hi_y;
                
                do
                {                    
                    hi_idx++;
                    if(hi_idx == num_internal_points)
                    {
                        hi_x = max_x;
                        hi_y = evaluate(coefficients[degree], degree, hi_x);
                        if(abs(hi_y) < epsilon)
                            goto done;
                    }
                    else
                    {
                        hi_x = roots[degree-1][hi_idx];
                        hi_y = evaluate(coefficients[degree], degree, hi_x);
                    }
                }
                while(abs(hi_y) < epsilon);
                
                if(
                    (lo_y < -epsilon && hi_y > +epsilon) ||
                    (lo_y > +epsilon && hi_y < -epsilon)
                    )
                {
                    // NOTE:
                    // [lo_x, hi_x] brackets a zero. In fact it is an isolated zero because
                    // the polynomial is monotonically increasing or decreasing on [x_lo, x_hi], so
                    // we use our isolated root finder.

                    float const root = isolated_root(lo_x, hi_x, coefficients, degree);
                    assert(root >= min_x && root <= max_x);
                    assert(root > lo_x && root < hi_x);

                    assert(num_roots < MAX_DEGREE);
                    roots[degree][num_roots] = root;
                    num_roots++;

                }

                lo_idx = hi_idx;
                lo_x = hi_x;
                lo_y = hi_y;
                
            }

        done:;
            
        }
        
        return num_roots;
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

        float derivative_coefficients[MAX_DEGREE][MAX_DEGREE] = {};
        float roots[MAX_DEGREE][MAX_DEGREE] = {};

        assert(polynomial_degree <= MAX_DEGREE);
        assert(polynomial_degree > 1);

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

        float const x_min = interval[0];
        float const x_max = interval[1];
        
        int const num_found_roots =
            try_find_roots(interval[0], interval[1], derivative_coefficients, polynomial_degree-1, roots);

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
    
}
