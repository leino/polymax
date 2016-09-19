#define _USE_MATH_DEFINES
#include <math.h>
#undef _USE_MATH_DEFINES
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include "ensure.h"
#include "array.h"
#include "integer.h"
#include "raw_cast.cpp"
#include "constants.h"
#include "numerics.cpp"
#include "serialization_limits.h"
#include "log.h"
namespace Polymax{ int const MAX_DEGREE = 16; }
#include "polymax.cpp"
#include "polymax_test_data.h"
#include "prng.cpp"
#include "prng_distribution.cpp"
#include "bar_chart.cpp"
#include "log_bar_chart.cpp"

int const NUM_AUTOMATED_TESTS = 1000000;

struct RandomTestParameters
{
    float min_coefficient;
    float max_coefficient;
};

struct RandomTestCase
{
    uint64 seed;
    float coefficients[Polymax::MAX_DEGREE];
    float x_min;
    float x_max;
    int degree;
};

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

void
random_test_case(
    RandomTestParameters const*const parameters,
    uint64 *const seed,
    RandomTestCase *const test_case
    )
{

    test_case->seed = *seed;
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

void
run_automated_test_cases(uint64 *const seed)
{

    {
        Log::string("test seed: ");
        Log::unsigned_integer_64(*seed);
        Log::newlines(2);
    }
    
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
    
    int const num_samples = 1000;
    
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
            Numerics::max(Numerics::abs(test_result.maximum_y), Numerics::abs(test_reference_result.maximum_y));
        
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

    Log::string("overall relative difference distribution: ");
    Log::newline();
    Log::bar_chart(&ch, &ctx);
    Log::newline();

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

            LOG_EXPRESSION_UNSIGNED_INTEGER_64(test_case->seed);
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

            newline();
        }
        
    }
    
    
}
int
main(int /*argument_count*/, char** /*arguments*/)
{
    // run_manual_test_cases();
    
    uint64 seed = 98332274319817232;
    run_automated_test_cases(&seed);
    run_automated_test_cases(&seed);
    run_automated_test_cases(&seed);
        
    return EXIT_SUCCESS;        
}
