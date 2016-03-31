#define _USE_MATH_DEFINES
#include <math.h>
#undef _USE_MATH_DEFINES
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "array.h"
#include "integer.h"
#include "raw_cast.cpp"
#include "constants.h"
#include "numerics.cpp"
#include "log.h"
namespace Polymax{ int const MAX_DEGREE = 16; }
#include "polymax.cpp"
#include "test_data.h"
#include "prng.cpp"
#include "prng_distribution.cpp"
#include "bar_chart.cpp"

int const NUM_AUTOMATED_TESTS = 1000000;

void
run_manual_test_cases()
{    
    
    for(uint test_idx=0; test_idx < ARRAY_LENGTH(test_cases); test_idx++)
    {

        using namespace Polymax;
        
        TestCase const*const tc = &test_cases[test_idx];

        using namespace Log;
        string("Test ");
        unsigned_integer(test_idx);
        newline();
        string("~~~~~~~~~~~~~~");
        newline();

        float maximum_x;
        float maximum_y;
        maximum(
            tc->coefficients,
            tc->degree,
            tc->interval,
            &maximum_x,
            &maximum_y
            );
           

        string("found maximum: ");
        floating_point(maximum_y);
        string(" at ");
        floating_point(maximum_x);
        string(", reference maximum: ");
        floating_point(tc->maximum_y);
        string(" at ");
        floating_point(tc->maximum_x);
                
        newlines(2);

    }
    
}

float
run_automated_test_cases()
{

    float worst_ratio = 1.0f;
    
    using namespace Polymax;

    /*

      NOTE: 

      We are generating random polynomials and guessing a bunch of times to find a "maximum".
      We then check to make sure that guessing does not beat our "exact" maximum finder too badly! :)
      That is the test.

     */

#define BUILD_BAR_CHART
    
#if defined(BUILD_BAR_CHART)
    BarChart::Context ctx;
    BarChart::Chart ch;
    int counts[20];
    BarChart::initialize(&ctx, &ch, 0.5f, 1.5f, ARRAY_LENGTH(counts), counts);
#endif
    
    int const num_samples = 1000;
    
    int const num_test_cases = NUM_AUTOMATED_TESTS;
    float const min_coefficient = -5.0f;
    float const max_coefficient = +5.0f;

    uint64 seed = 983274123412;
    
    for(int test_case_idx=0; test_case_idx < num_test_cases; test_case_idx++)
    {

        // NOTE: generate some random polynomial
        float coefficients[MAX_DEGREE] = {};
        int const degree = uniform_range(&seed, 2, 7);
        for(int coefficient_idx=0; coefficient_idx <= degree; coefficient_idx++)
        {
            float const coefficient = uniform_interval(&seed, min_coefficient, max_coefficient);
            coefficients[coefficient_idx] = coefficient;
        }

        // NOTE: generate some random range
        float const x_min = uniform_interval(&seed, -1.0f, +1.0f);
        float const x_max = x_min + uniform_interval(&seed, 1.0f, 5.0f);
        assert(x_min < x_max);
        
        // NOTE: check around randomly for maximum
        float reference_maximum_x = QUIET_NAN_FLOAT;
        
        float reference_maximum_y = NEGATIVE_INFINITY_FLOAT;
        {
            int const num_segments = num_samples-1;
            float const sample_spacing = (x_max - x_min)/num_segments;
            for(int sample_idx=0; sample_idx < num_samples; sample_idx++)
            {
                float const sample_x = x_min + float(sample_idx)*sample_spacing;
                float const sample_y = evaluate(coefficients, degree, sample_x);
                if(sample_y > reference_maximum_y)
                {
                    reference_maximum_x = sample_x;
                    reference_maximum_y = sample_y;
                }
            }
        }
        // TODO: add a step that seeks for local maximum at the found maximum candidate?

        float maximum_x;
        float maximum_y;
        maximum(coefficients, degree, x_min, x_max, &maximum_x, &maximum_y);

        float const ratio = maximum_y/reference_maximum_y;
        if(ratio < worst_ratio)
        {
            worst_ratio = ratio;
        }

        BarChart::record_entry(&ctx, &ch, ratio);        
        
    }

#if defined(BUILD_BAR_CHART)

    if(ch.num_low_outliers > 0)
    {
        using namespace Log;            
        string("low outliers: ");
        integer(ch.num_low_outliers);
        newline();
    }
    for(int bin_idx = 0; bin_idx < ctx.num_bins; bin_idx++)
    {
        
        using namespace Log;
        int const count = ch.counts[bin_idx];
        float ratio_lo;
        float ratio_hi;
        BarChart::bin_range(&ctx, bin_idx, &ratio_lo, &ratio_hi);
        floating_point(ratio_lo);
        string(" ... ");
        floating_point(ratio_hi);
        string(": ");
        integer(count);
        newline();
        
    }
    if(ch.num_high_outliers > 0)
    {
        using namespace Log;            
        string("high outliers: ");
        integer(ch.num_high_outliers);
        newline();
    }
    
    
#endif    

    return worst_ratio;
    
}
int
main(int argument_count, char** arguments)
{
    argument_count; arguments;

    // run_manual_test_cases();
    run_automated_test_cases();
        
    return EXIT_SUCCESS;        
}
