// TODO:
// Add more tests.
// Add some tests with slightly less well-conditioned polynomials.


#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "array.h"
#include "integer.h"
#include "numerics.cpp"
#include "log.h"
namespace Polymax{ int const MAX_DEGREE = 16; }
#include "polymax.cpp"
#include "test_data.h"

int
main(int argument_count, char** arguments)
{
    argument_count; arguments;

    using namespace Polymax;

    for(uint test_idx=0; test_idx < ARRAY_LENGTH(test_cases); test_idx++)
    {

        TestCase const*const tc = &test_cases[test_idx];

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
            maximum(
                tc->coefficients,
                tc->degree,
                tc->interval,
                &maximum_x,
                &maximum_y
                );
           

            {

                using namespace Log;
                
                string("found maximum: ");
                floating_point(maximum_y);
                string(" at ");
                floating_point(maximum_x);
                string(", reference maximum: ");
                floating_point(tc->maximum_y);
                string(" at ");
                floating_point(tc->maximum_x);
                
                newline();
            }
            
        }

        Log::newline();
    }
    
    return EXIT_SUCCESS;        
}
