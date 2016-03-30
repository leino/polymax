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
namespace Polymax{ int const MAX_DEGREE = 7; }
#include "polymax.cpp"

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

    using namespace Polymax;

    int const num_tests = 3;
    float coefficients[num_tests][MAX_DEGREE] = {};
    float reference_maxima_x[num_tests] = {};
    float reference_maxima_y[num_tests] = {};    
    float interval[num_tests][2] = {};
    int degree[num_tests] = {};

    {
        int const test_idx = 0;
        degree[test_idx] = 5;
        coefficients[test_idx][0] = +1.0123f;
        coefficients[test_idx][1] = -1.032f;
        coefficients[test_idx][2] = -3.012f;
        coefficients[test_idx][3] = +2.012f;
        coefficients[test_idx][4] = +1.0123f;
        coefficients[test_idx][5] = +1.0f;

        /*
          NOTE: Reference data from inspecting graphs on Google:
          NOTE: Polynomial for google:
          1.0123*x^0-1.032*x^1-3.012*x^2+2.012*x^3+1.0123*x^4+1.0*x^5
         */
        interval[test_idx][0] = -1.0f;
        interval[test_idx][1] = +0.8f;
        reference_maxima_y[test_idx] = 1.0929769f;
        reference_maxima_x[test_idx] = -0.150601134f;
        
    }

    {
        int const test_idx = 1;
        degree[test_idx] = 4;
        coefficients[test_idx][0] = 0.0f;
        coefficients[test_idx][1] = 0.0f;
        coefficients[test_idx][2] = 0.0f;
        coefficients[test_idx][3] = -23.007f;
        coefficients[test_idx][4] = +100.02f;
        
        /*
          NOTE: Reference data from inspecting graphs on Google:
          NOTE: Polynomial for google:
          0.0*x^0+0.0*x^1+0.0*x^2-23.007*x^3+100.02*x^4
         */
        interval[test_idx][0] = 0.0f;
        interval[test_idx][1] = 0.2f;
        reference_maxima_y[test_idx] = 0.0f;
        reference_maxima_x[test_idx] = 0.0f;

    }

    {
        int const test_idx = 2;
        degree[test_idx] = 4;
        coefficients[test_idx][0] = 0.0f;
        coefficients[test_idx][1] = 0.0f;
        coefficients[test_idx][2] = -1.023f;
        coefficients[test_idx][3] = 0.0f;
        coefficients[test_idx][4] = +23.007f;
        
        /*
          NOTE: Reference data from inspecting graphs on Google:
          NOTE: Polynomial for google:
          0*​x^​0+​0*​x^​1-​1.023*​x^​2+​23.007*​x^​4
         */

        interval[test_idx][0] = -0.2f;
        interval[test_idx][1] = +0.2f;
        reference_maxima_y[test_idx] = 0.0f;
        reference_maxima_x[test_idx] = 0.0f;
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
        
        {

            float maximum_x;
            float maximum_y;
            maximum(coefficients[test_idx], degree[test_idx], interval[test_idx], &maximum_x, &maximum_y);
           

            {

                using namespace Log;
                
                string("found maximum: ");
                floating_point(maximum_y);
                string(" at ");
                floating_point(maximum_x);
                string(", reference maximum: ");
                floating_point(
                    reference_maxima_y[test_idx]
                    );
                string(" at ");
                floating_point(
                    reference_maxima_x[test_idx]
                    );
                
                newline();
            }
            
        }

        Log::newline();
    }
    
    return EXIT_SUCCESS;        
}
