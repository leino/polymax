#define _USE_MATH_DEFINES
#include <math.h>
#undef _USE_MATH_DEFINES
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "ensure.h"
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

struct RandomTestParameters
{
    float min_coefficient;
    float max_coefficient;
};

struct RandomTestCase
{
    float coefficients[Polymax::MAX_DEGREE];
    float x_min;
    float x_max;
    int degree;
};

void
random_test_case(
    RandomTestParameters const*const parameters,
    uint64 *const seed,
    RandomTestCase *const test_case
    )
{

    memset(test_case, 0, sizeof(TestCase));
    
    test_case->degree = uniform_range(seed, 2, 7);
    for(int coefficient_idx=0; coefficient_idx <= test_case->degree; coefficient_idx++)
    {
        float const coefficient = uniform_interval(seed, parameters->min_coefficient, parameters->max_coefficient);
        test_case->coefficients[coefficient_idx] = coefficient;
    }

    // NOTE: generate some random range
    test_case->x_min = uniform_interval(seed, -1.0f, +1.0f);
    test_case->x_max = test_case->x_min + uniform_interval(seed, 1.0f, 5.0f);
    ENSURE(test_case->x_min < test_case->x_max);
    
}

namespace Log
{
    
    using namespace BarChart;
    
    void
    bar_chart(Chart const*const ch, Context const*const ctx)
    {
        ENSURE(ctx->high >= ctx->low);
        
        // NOTE: use appropriate number of significant digits to display the output in, so as to not
        // clutter things up unneccessarily, yet still being able to distinguish bin limits
        float const range = ctx->high - ctx->low;
        float const bin_range = range/float(ctx->num_bins);
        float const bin_range_exponent = Numerics::log10_float(bin_range);
        int const num_displayed_significant_digits =
            (int)Numerics::ceiling(Numerics::abs(bin_range_exponent));

        int const num_displayed_decimals = bin_range_exponent < 0 ? num_displayed_significant_digits : 0;
        // NOTE: num digits on left/right side of decimal point
        int const num_digits_left = num_displayed_significant_digits - num_displayed_decimals;
        int const num_digits_right = num_displayed_decimals;
        
        
        if(ch->num_low_outliers > 0)
        {
            using namespace Log;

            spaces(Numerics::max_int(1, num_digits_left) + num_digits_right + 2);
            string(" ... ");
            floating_point_specific_precision_sign(ctx->low, num_digits_left, num_digits_right);
            string(": ");
            integer(ch->num_low_outliers);
            newline();
        }

        // NOTE: we want to summarize long runs of empty bins, so we are printing ranges of bins rather than
        // individual bins (although a range can have length 1, of course)
        int lo_bin_idx = 0;
        while(lo_bin_idx < ctx->num_bins)
        {

            using namespace Log;


            int count = ch->counts[lo_bin_idx];
            int hi_bin_idx = lo_bin_idx+1;
            if(count == 0)
            {
                while(hi_bin_idx < ctx->num_bins)
                {
                    if(ch->counts[hi_bin_idx] != 0)
                    {
                        break;
                    }
                    hi_bin_idx++;
                }
            }

            float lo;
            BarChart::bin_limit_low(ctx, lo_bin_idx, &lo);
            float hi;
            BarChart::bin_limit_high(ctx, hi_bin_idx-1, &hi);

#if 0
            string("[");
            integer(lo_bin_idx);
            string(", ");
            integer(hi_bin_idx);
            string(")");
            string(" ");            
#endif
            

            floating_point_specific_precision_sign(lo, num_digits_left, num_digits_right);
            string(" ... ");
            floating_point_specific_precision_sign(hi, num_digits_left, num_digits_right);
            string(": ");
            integer(count);
            newline();


            lo_bin_idx = hi_bin_idx;
        }
        if(ch->num_high_outliers > 0)
        {
            using namespace Log;
            floating_point_specific_precision_sign(ctx->high, num_digits_left, num_digits_right);
            string(" ... ");
            spaces(Numerics::max_int(1, num_digits_left) + num_digits_right + 2);
            string(": ");
            
            integer(ch->num_high_outliers);
        }
        
    }

};

void
run_automated_test_cases(uint64 *const seed)
{

    using namespace Polymax;

    enum Extreme
    {
        Best,
        Worst,
        NumExtremes,
    };
    
    struct TestResult
    {
        float maximum_x;
        float maximum_y;
    };
    
    float extreme_difference[Extreme::NumExtremes];
    extreme_difference[Extreme::Worst] = POSITIVE_INFINITY_FLOAT;
    extreme_difference[Extreme::Best] = NEGATIVE_INFINITY_FLOAT;

    /*

      NOTE: 

      We are generating random polynomials and guessing a bunch of times to find a "maximum".
      We then check to make sure that guessing does not beat our "exact" maximum finder too badly! :)
      That is the test.

     */

    BarChart::Context ctx;
    BarChart::Chart ch;
    int counts[100];
    float const minimum_difference = -0.001f;
    float const maximum_difference = +0.0f;
    BarChart::initialize(&ctx, &ch, minimum_difference, maximum_difference, ARRAY_LENGTH(counts), counts);
    
    int const num_samples = 10000;
    
    int const num_test_cases = NUM_AUTOMATED_TESTS;
    
    RandomTestParameters parameters;
    parameters.min_coefficient = -5.0f;
    parameters.max_coefficient = +5.0f;



    
    RandomTestCase extreme_test_cases[Extreme::NumExtremes] = {};
    TestResult extreme_test_results[Extreme::NumExtremes] = {};
    TestResult extreme_test_reference_results[Extreme::NumExtremes] = {};
        
    for(int test_case_idx=0; test_case_idx < num_test_cases; test_case_idx++)
    {

        RandomTestCase test_case;
        random_test_case(&parameters, seed, &test_case);
        
        // NOTE: scan for a "reference maximum", i.e. a candidate for maximum
        TestResult test_reference_result;
        test_reference_result.maximum_x = QUIET_NAN_FLOAT;        
        test_reference_result.maximum_y = NEGATIVE_INFINITY_FLOAT;
        {
            int const num_segments = num_samples-1;
            for(int sample_idx=0; sample_idx < num_samples; sample_idx++)
            {
                float const sample_x =
                    test_case.x_min +
                    (float(sample_idx)/float(num_segments))*(test_case.x_max - test_case.x_min);
                
                float const sample_y = evaluate(test_case.coefficients, test_case.degree, sample_x);
                if(sample_y > test_reference_result.maximum_y)
                {
                    test_reference_result.maximum_x = sample_x;
                    test_reference_result.maximum_y = sample_y;
                }
            }
        }
        // TODO: add a step that seeks for local maximum at the found maximum candidate?

        TestResult test_result;
        maximum(
            test_case.coefficients,
            test_case.degree,
            test_case.x_min,
            test_case.x_max,
            &test_result.maximum_x,
            &test_result.maximum_y
            );

        float const difference =
            (test_result.maximum_y - test_reference_result.maximum_y)/
            Numerics::min(Numerics::abs(test_result.maximum_y), Numerics::abs(test_reference_result.maximum_y));
        
        if(difference < extreme_difference[Extreme::Worst])
        {
            // NOTE: copy down the worst test case
            extreme_difference[Extreme::Worst] = difference;
            extreme_test_cases[Extreme::Worst] = test_case;
            extreme_test_results[Extreme::Worst] = test_result;
            extreme_test_reference_results[Extreme::Worst] = test_reference_result;
        }
        if(difference > extreme_difference[Extreme::Best])
        {
            // NOTE: copy down the best test case
            extreme_difference[Extreme::Best] = difference;
            extreme_test_cases[Extreme::Best] = test_case;
            extreme_test_results[Extreme::Best] = test_result;
            extreme_test_reference_results[Extreme::Best] = test_reference_result;
        }

        
        BarChart::record_entry(&ctx, &ch, difference);
        
    }

    Log::bar_chart(&ch, &ctx);
    Log::newlines(3);


    {
        using namespace Log;

        newline();
        for(int extreme_idx=0; extreme_idx < Extreme::NumExtremes; extreme_idx++)
        {
            Extreme const extreme = Extreme(extreme_idx);
            RandomTestCase const*const test_case = &extreme_test_cases[extreme];
            TestResult const*const reference_result = &extreme_test_reference_results[extreme];
            TestResult const*const result = &extreme_test_results[extreme];
            float const relative_difference = extreme_difference[extreme];

            switch(extreme)
            {
            case Extreme::Best: string("best case: "); break;
            case Extreme::Worst: string("worst case: "); break;
            };
            newline();
            
            LOG_EXPRESSION_FLOAT(relative_difference);
            newline();
            
            string("polynomial = ");
            for(int coefficient_idx=0; coefficient_idx <= test_case->degree; coefficient_idx++)
            {
                float const coefficient = test_case->coefficients[coefficient_idx];
                floating_point(coefficient);
                if(coefficient_idx > 0)
                {
                    string("*x^");
                    integer(coefficient_idx);
                }
                if(coefficient_idx < test_case->degree)
                    string(" + ");
            
            }
            newline();
            LOG_EXPRESSION_FLOAT(test_case->x_min); newline();
            LOG_EXPRESSION_FLOAT(test_case->x_max); newline();
            
            LOG_EXPRESSION_FLOAT(reference_result->maximum_x); newline();
            LOG_EXPRESSION_FLOAT(reference_result->maximum_y); newline();
            LOG_EXPRESSION_FLOAT(result->maximum_x); newline();
            LOG_EXPRESSION_FLOAT(result->maximum_y); newline();

            newlines(2);
        }
        
    }
    
    
}
int
main(int argument_count, char** arguments)
{
    argument_count; arguments;

    // run_manual_test_cases();
    uint64 seed = 983274123412;
    run_automated_test_cases(&seed);
    run_automated_test_cases(&seed);
    run_automated_test_cases(&seed);
    run_automated_test_cases(&seed);
        
    return EXIT_SUCCESS;        
}
