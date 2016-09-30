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
#include "prng.cpp"
#include "prng_distribution.cpp"
#include "bar_chart.cpp"
#include "log_bar_chart.cpp"
#include "log_polynomial.cpp"

struct TestResult
{
    float maximum_x;
    float maximum_y;
};

struct TestCase
{
    uint64 seed;
    float coefficients[Polymax::MAX_DEGREE];
    float x_min;
    float x_max;
    int degree;
    TestResult reference_result;
};

enum Extreme
{
    Best,
    Worst,
    NumExtremes,
};    

float
condition_number(float const*const coefficients, int const degree, float const x)
{
    float nonnegative_coefficients[Polymax::MAX_DEGREE];
    for(int coefficient_idx=0; coefficient_idx <= degree; coefficient_idx++)
    {
        nonnegative_coefficients[coefficient_idx] = Numerics::abs(coefficients[coefficient_idx]);
    }
    double const positive_value = Polymax::evaluate(nonnegative_coefficients, degree, Numerics::abs(x));
    ENSURE(positive_value > 0);
    double const value = Polymax::evaluate(coefficients, degree, x);
    double const number = positive_value / Numerics::abs(value);
    return float(number);
}

namespace Generation
{

    struct
    PolynomialParameters
    {
        float real_root_real_min;
        float real_root_real_max;
        float complex_root_real_min;
        float complex_root_real_max;
        float complex_root_imaginary_min;
        float complex_root_imaginary_max;
        int min_num_real_roots;
        int max_num_real_roots;
        int min_num_complex_roots;
        int max_num_complex_roots;
    };

    // NOTE:
    // Generate a random "real polynomial" i.e. one with real coefficient,
    // also return a complete list of it's real roots.
    void
    real_polynomial_with_real_roots(
        PolynomialParameters const*const parameters,
        uint64 *const seed,
        float *const coefficients,
        int *const degree,
        int *const num_real_roots,
        float *const real_roots
        )
    {

        ENSURE(parameters->complex_root_imaginary_min >= 0.0f);
        ENSURE(parameters->complex_root_imaginary_min < parameters->complex_root_imaginary_max);

		memset(coefficients, 0, sizeof(float)*Polymax::MAX_DEGREE);
        
        int const min_num_real_roots = parameters->min_num_real_roots;
        int const max_num_real_roots = parameters->max_num_real_roots;
        int const min_num_complex_roots = parameters->min_num_complex_roots;
        int const max_num_complex_roots = parameters->max_num_complex_roots;
        
        *num_real_roots = uniform_range(seed, min_num_real_roots, max_num_real_roots);
        int const num_complex_roots = uniform_range(seed, min_num_complex_roots, max_num_complex_roots);
        ENSURE(*num_real_roots + num_complex_roots <= Polymax::MAX_DEGREE);
        
        // NOTE: Generate the real roots
        float const real_root_real_min = parameters->real_root_real_min;
        float const real_root_real_max = parameters->real_root_real_max;
        for(int root_idx=0; root_idx < *num_real_roots; root_idx++)
        {
            real_roots[root_idx] = uniform_interval(seed, real_root_real_min, real_root_real_max);
        }

        // NOTE: Multiply in the complex conjugate root factors.
        *degree = 0;
        coefficients[0] = float(1);
        float const complex_root_real_min = parameters->complex_root_real_min;
        float const complex_root_real_max = parameters->complex_root_real_max;
        float const complex_root_imaginary_min = parameters->complex_root_imaginary_min;
        float const complex_root_imaginary_max = parameters->complex_root_imaginary_max;
        for(int root_idx=0; root_idx < num_complex_roots; root_idx++)
        {
            // NOTE: Generate the root.
            float const real_part = uniform_interval(seed, complex_root_real_min, complex_root_real_max);
            float const imaginary_part =
                uniform_interval(seed, complex_root_imaginary_min, complex_root_imaginary_max);
            float const magnitude_squared = Numerics::square(real_part) + Numerics::square(imaginary_part);

            // NOTE:
            // Let a be our complex root.
            // We multiply by (x - a)(x - a^*) = x^2 - 2Re(a)x + |a|^2
            *degree += 2;
            for(int coefficient_idx = *degree; coefficient_idx >= 2; coefficient_idx--)
            {
                coefficients[coefficient_idx] =
                    coefficients[coefficient_idx - 2]
                    - float(2)*real_part*coefficients[coefficient_idx - 1]
                    + magnitude_squared*coefficients[coefficient_idx]
                    ;
            }
            coefficients[1] =
                - float(2)*real_part*coefficients[0]
                + magnitude_squared*coefficients[1]
                ;
            coefficients[0] = magnitude_squared*coefficients[0];
            
        }

        // NOTE: Multiply in the real root factors
        for(int root_idx = 0; root_idx < *num_real_roots; root_idx++)
        {
            float const root = real_roots[root_idx];
            // NOTE:
            // Let our real root be a, then
            // we multiply by x - a
            *degree += 1;
            for(int coefficient_idx = *degree; coefficient_idx - 1 >= 0; coefficient_idx--)
            {
                coefficients[coefficient_idx] =
                    coefficients[coefficient_idx - 1]
                    - root*coefficients[coefficient_idx]
                    ;                
            }
            coefficients[0] = -root*coefficients[0];
        }
        
    }
    
}

void
generate_test_case(
    uint64 *const seed,
    TestCase *const test_case
    )
{

    test_case->seed = *seed;
    float const derivative_real_roots_min_real = -4.0f;
    float const derivative_real_roots_max_real = +4.0f;
    float const derivative_complex_roots_min = -6.0f;
    float const derivative_complex_roots_max = +6.0f;
    
    float derivative_roots[Polymax::MAX_DEGREE];
    int num_derivative_roots;
    
    // NOTE:
    // First we generate the derivative of our final polynomial
    // (so that we can know all of the candidates for maxima, i.e. the roots of the
    //  derivative, exactly)
    {
        using namespace Generation;
        
        int const max_derivative_degree = 7;
        ENSURE_STATIC(max_derivative_degree < Polymax::MAX_DEGREE);
    
        PolynomialParameters parameters;
        parameters.real_root_real_min = derivative_real_roots_min_real;
        parameters.real_root_real_max = derivative_real_roots_max_real;
        parameters.complex_root_real_min = derivative_complex_roots_min;
        parameters.complex_root_real_max = derivative_complex_roots_max;
        parameters.complex_root_imaginary_min = float(0);
        parameters.complex_root_imaginary_max = uniform_interval(seed, float(0.5), float(1));
        parameters.min_num_real_roots = 3;
        parameters.max_num_real_roots = 4;
        parameters.min_num_complex_roots = 0;
        parameters.max_num_complex_roots = 2;
        ENSURE(parameters.max_num_real_roots + 2*parameters.max_num_complex_roots < Polymax::MAX_DEGREE);
        
        real_polynomial_with_real_roots(
            &parameters,
            seed,
            test_case->coefficients,
            &test_case->degree,
            &num_derivative_roots,
            derivative_roots
            );
        ENSURE(test_case->degree < Polymax::MAX_DEGREE);
    }

    // NOTE:
    // Pick appropriate limits based on how derivative roots are distributed
    {
        float min = POSITIVE_INFINITY_FLOAT;
        float max = NEGATIVE_INFINITY_FLOAT;
        for(int root_idx=0; root_idx < num_derivative_roots; root_idx++)
        {
            float const root = derivative_roots[root_idx];
            min = Numerics::min(min, root);
            max = Numerics::max(max, root);
        }
        ENSURE(min < max);
        test_case->x_min = min + (max - min)*0.05f;
        test_case->x_max = max - (max - min)*0.05f;
    }

    // NOTE: Now integrate the derivative
    test_case->degree++;
    ENSURE(test_case->degree <= Polymax::MAX_DEGREE);
    for(int coefficient_idx=test_case->degree; coefficient_idx >= 1; coefficient_idx--)
    {
        test_case->coefficients[coefficient_idx] =
            test_case->coefficients[coefficient_idx-1]/float(coefficient_idx);
    }
    // NOTE: integration constant does not matter, we will adjust the constant term below anyway
    test_case->coefficients[0] = uniform_interval(seed, float(-5), float(+5));
    
    // NOTE: Find the maximum by checking the endpoints as well as the real roots of the derivative.
    float maximum_x;
    float maximum_y;
    {
        
        // First check the endpoints
        float const x_min_y = Polymax::evaluate(test_case->coefficients, test_case->degree, test_case->x_min);
        float const x_max_y = Polymax::evaluate(test_case->coefficients, test_case->degree, test_case->x_max);
        if(x_min_y > x_max_y)
        {
            maximum_x = test_case->x_min;
            maximum_y = x_min_y;
        }
        else
        {
            maximum_x = test_case->x_max;
            maximum_y = x_max_y;
        }
        
        // Now check the real roots of the derivative
        for(int root_idx=0; root_idx < num_derivative_roots; root_idx++)
        {
            
            float const root = derivative_roots[root_idx];
            
            if(root >= test_case->x_min && root <= test_case->x_max)
            {
                float const y = Polymax::evaluate(test_case->coefficients, test_case->degree, root);
                if(y > maximum_y)
                {
                    maximum_y = y;
                    maximum_x = root;
                }
            }
            
        }
        
    }
    
    test_case->reference_result.maximum_x = maximum_x;
    test_case->reference_result.maximum_y = maximum_y;
    
}

void
print_test_result(
    int const num_discarded_test_cases,
    BarChart::Chart const*const chart,
    BarChart::Context const*const chart_context,
    TestCase const extreme_cases[Extreme::NumExtremes],
    TestResult const extreme_reference_results[Extreme::NumExtremes],
    TestResult const extreme_results[Extreme::NumExtremes],
    float const extreme_quotient[Extreme::NumExtremes]
    )
{
    using namespace Log;

    Log::string("overall quotient distribution: ");
    Log::newline();
    Log::bar_chart(chart, chart_context);
    Log::newline();

    LOG_EXPRESSION_UNSIGNED_INTEGER(num_discarded_test_cases);
    Log::newline();
    
    newline();
    for(int extreme_idx=0; extreme_idx < Extreme::NumExtremes; extreme_idx++)
    {
        Extreme const extreme = Extreme(extreme_idx);
        TestCase const*const test_case = &extreme_cases[extreme];
        TestResult const*const reference_result = &extreme_reference_results[extreme];
        TestResult const*const result = &extreme_results[extreme];
        float const quotient = extreme_quotient[extreme];

        switch(extreme)
        {
        case Extreme::Best: string("best case: "); break;
        case Extreme::Worst: string("worst case: "); break;
        };
        newline();

        LOG_EXPRESSION_UNSIGNED_INTEGER_64(test_case->seed);
        newline();
            
        LOG_EXPRESSION_FLOAT(quotient);
        newline();
            
        string("polynomial = ");
        newline();
        Log::polynomial(test_case->coefficients, test_case->degree);

        float const reference_condition_number =
            condition_number(test_case->coefficients, test_case->degree, reference_result->maximum_x);
        LOG_EXPRESSION_FLOAT(reference_condition_number); newline();
            
        LOG_EXPRESSION_FLOAT(test_case->x_min); newline();
        LOG_EXPRESSION_FLOAT(test_case->x_max); newline();
            
        LOG_EXPRESSION_FLOAT(reference_result->maximum_x); newline();
        LOG_EXPRESSION_FLOAT(reference_result->maximum_y); newline();
            
        LOG_EXPRESSION_FLOAT(result->maximum_x); newline();
        LOG_EXPRESSION_FLOAT(result->maximum_y); newline();

        newline();
    }
    
}

void
run_and_print_test(
    uint64 *const seed
    )
{    

    BarChart::Context chart_context;
    BarChart::Chart chart;
    int bin_counts[100000];
    float const minimum_quotient = float(0);
    float const maximum_quotient = float(1.00001);
    BarChart::initialize(
        &chart_context,
        &chart,
        minimum_quotient,
        maximum_quotient,
        ARRAY_LENGTH(bin_counts),
        bin_counts
        );
    
    float extreme_quotient[Extreme::NumExtremes];
    extreme_quotient[Extreme::Worst] = POSITIVE_INFINITY_FLOAT;
    extreme_quotient[Extreme::Best] = NEGATIVE_INFINITY_FLOAT;
    TestCase extreme_cases[Extreme::NumExtremes] = {};
    TestResult extreme_results[Extreme::NumExtremes] = {};
    TestResult extreme_reference_results[Extreme::NumExtremes] = {};
    
    
    int const num_cases = 1000000;
    int num_discarded_cases = 0;

    for(int case_idx=0; case_idx < num_cases; case_idx++)
    {

        using namespace Polymax;
        
        TestCase test_case = {};
        generate_test_case(
            seed,
            &test_case
            );

        TestResult test_result;
        maximum(
            test_case.coefficients,
            test_case.degree,
            test_case.x_min,
            test_case.x_max,
            &test_result.maximum_x,
            &test_result.maximum_y
            );

        float const test_result_maximum_y =
            evaluate(test_case.coefficients, test_case.degree, test_result.maximum_x);

        float const test_reference_result_maximum_y =
            evaluate(test_case.coefficients, test_case.degree, test_case.reference_result.maximum_x);

        float const reference_condition_number =
            condition_number(test_case.coefficients, test_case.degree, test_case.reference_result.maximum_x);

        if(reference_condition_number >= float(1000))
        {
            num_discarded_cases++;
            continue;
        }
        
        float quotient;
        if(test_result_maximum_y < 0 && test_reference_result_maximum_y < 0)
        {
            quotient =
                float(test_reference_result_maximum_y / test_result_maximum_y);
        }
        else if(test_result_maximum_y > 0 && test_reference_result_maximum_y > 0)
        {
            quotient =
                float(test_result_maximum_y / test_reference_result_maximum_y);
        }
        else
        {
            // NOTE: discarding test case, it just adds noise
            num_discarded_cases++;
            continue;
        }

        if(quotient > extreme_quotient[Extreme::Best])
        {
            // NOTE: copy down the best test case
            extreme_quotient[Extreme::Best] = quotient;
            extreme_cases[Extreme::Best] = test_case;
            extreme_results[Extreme::Best] = test_result;
            extreme_reference_results[Extreme::Best] = test_case.reference_result;
        }        

        if(quotient < extreme_quotient[Extreme::Worst])
        {
            // NOTE: copy down the worst test case
            extreme_quotient[Extreme::Worst] = quotient;
            extreme_cases[Extreme::Worst] = test_case;
            extreme_results[Extreme::Worst] = test_result;
            extreme_reference_results[Extreme::Worst] = test_case.reference_result;
        }
        
        BarChart::record_entry(&chart_context, &chart, quotient);        
        
    }

    print_test_result(
        num_discarded_cases,
        &chart,
        &chart_context,
        extreme_cases,
        extreme_reference_results,
        extreme_results,
        extreme_quotient
        );
    
}

int
main(int /*argument_count*/, char** /*arguments*/)
{
    uint const num_test_cases = 10;

    uint64 seed = 987123987987123;
    int const num_runs = 4;
    for(int run_idx=0; run_idx < num_runs; run_idx++)
    {
        Log::string("Test ");
        Log::integer(run_idx + 1);
        Log::newline();
        Log::string("~~~~~~~~~~~~");
        Log::newline();
        run_and_print_test(&seed);
    }
    
    return EXIT_SUCCESS;        
}
