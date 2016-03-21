// TODO:
// Add more tests.
// Add some tests with slightly less well-conditioned polynomials.


#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "integer.h"
#include "numerics.cpp"
#include "log.h"
namespace Polyroot{ int const MAX_DEGREE = 7; }
#include "polyroot.cpp"

// NOTE:
// The test data I'm comparing against comes from copy-pasting in
// the polynomials int Google and inspecting their graphs.
//
// Very qucik-n-dirty but works so far in the sense that I reproduce the results
// to within a certain tolerance.
//
// TODO: Find a good reference root finder to compare against instead of doing the above.

int
main(int argument_count, char** arguments)
{
    argument_count; arguments;

    using namespace Polyroot;

    int const num_tests = 2;
    float coefficients[num_tests][MAX_DEGREE][MAX_DEGREE] = {};
    float reference_roots[num_tests][MAX_DEGREE] = {};
    int num_reference_roots[num_tests] = {};
    int highest_degree[num_tests] = {};

    {
        int const test_idx = 0;
        int const degree = 5;
        highest_degree[test_idx] = degree;
        coefficients[test_idx][degree][0] = +1.0123f;
        coefficients[test_idx][degree][1] = -1.032f;
        coefficients[test_idx][degree][2] = -3.012f;
        coefficients[test_idx][degree][3] = +2.012f;
        coefficients[test_idx][degree][4] = +1.0123f;
        coefficients[test_idx][degree][5] = +1.0f;

        /*
          NOTE: Reference data from inspecting graphs on Google:
          NOTE: Polynomial for google:
          1.0123*x^0-1.032*x^1-3.012*x^2+2.012*x^3+1.0123*x^4+1.0*x^5
         */
        reference_roots[test_idx][0] = -0.6348567f;
        reference_roots[test_idx][1] = +0.5583167f;
        reference_roots[test_idx][2] = +0.7839570f;
        num_reference_roots[test_idx] = 3;
        
    }
    
    {
        int const test_idx = 1;
        int const degree = 4;
        highest_degree[test_idx] = degree;
        coefficients[test_idx][degree][0] = 0.0f;
        coefficients[test_idx][degree][1] = 0.0f;
        coefficients[test_idx][degree][2] = 0.0f;
        coefficients[test_idx][degree][3] = -23.007f;
        coefficients[test_idx][degree][4] = +100.02f;
        
        /*
          NOTE: Reference data from inspecting graphs on Google:
          NOTE: Polynomial for google:
          0.0*x^0+0.0*x^1+0.0*x^2-23.007*x^3+100.02*x^4
         */
        reference_roots[test_idx][0] =  0.0000000f;
        reference_roots[test_idx][1] = +0.2300240f;
        num_reference_roots[test_idx] = 2;        
    }
    
    for(uint test_idx=0; test_idx < num_tests; test_idx++)
    {

        {
            using namespace Log;
            string("Test ");
            unsigned_integer(test_idx);
            newline();
            string("~~~~~~~~~~~~~~");
            newline();
        }
        
        float roots[MAX_DEGREE][MAX_DEGREE] = {};

        float const min_x = -100.0f;
        float const max_x = +100.0f;

        assert(highest_degree[test_idx] <= MAX_DEGREE);
        
        // NOTE: compute all of the derivatives
        for(int degree = highest_degree[test_idx]-1; degree >= 0; degree--)
        {
            for(int coefficient_idx=0; coefficient_idx <= degree; coefficient_idx++)
            {
                coefficients[test_idx][degree][coefficient_idx] =
                    float(coefficient_idx+1)*coefficients[test_idx][degree+1][coefficient_idx+1];
            }
        }

        {
            int const num_found_roots =
                try_find_roots(min_x, max_x, coefficients[test_idx], highest_degree[test_idx], roots);
            using namespace Log;

            int const num_ref_roots = num_reference_roots[test_idx];
            
            for(int root_idx=0; root_idx < num_found_roots; root_idx++)
            {
                unsigned_integer(root_idx);
                string(": ");
                float const found_root = roots[highest_degree[test_idx]][root_idx];
                string("found root: ");
                floating_point(found_root);
                string(", reference root: ");
                if(root_idx >= num_ref_roots)
                {
                    string("???");
                }
                else
                {
                    float const reference_root = reference_roots[test_idx][root_idx];
                    floating_point(
                        reference_root
                        );
                    string(", absolute error: ");
                    floating_point(Numerics::abs(reference_root - found_root));
                }
                string(", value: ");
                floating_point(
                    evaluate(coefficients[test_idx][highest_degree[test_idx]], highest_degree[test_idx], found_root)
                    );
                newline();
            }

            string("# roots found: ");
            unsigned_integer(num_found_roots);
            string("/");
            unsigned_integer(num_ref_roots);
            newline();
            
        }

        Log::newline();
    }
    
    return EXIT_SUCCESS;        
}
