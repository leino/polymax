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
namespace Polymax{ int const MAX_DEGREE = 7; }
#include "polymax.cpp"

// NOTE:
// The test data I'm comparing against comes from copy-pasting in
// the polynomials int Google and inspecting their graphs.
//
// Very qucik-n-dirty but works so far in the sense that I reproduce the results
// to within a certain tolerance.

struct TestCase
{
    int degree;
    float coefficients[Polymax::MAX_DEGREE];
    float interval[2];
    float maximum_x;
    float maximum_y;
};

TestCase test_cases[] = 
{
    
    {

        /*
          NOTE: Reference data from inspecting graphs on Google:
          NOTE: Polynomial for google:
          1.0123*x^0-1.032*x^1-3.012*x^2+2.012*x^3+1.0123*x^4+1.0*x^5
        */        

        // degree and coefficients
        5,
        {
            +1.0123f,
            -1.032f,
            -3.012f,
            +2.012f,
            +1.0123f,
            +1.0f
        },

        // interval
        { -1.0f, +0.8f },

        // maximum x,y
        -0.150601134f,
        1.0929769f
        
    },

    {

        /*
          NOTE: Reference data from inspecting graphs on Google:
          NOTE: Polynomial for google:
          0.0*x^0+0.0*x^1+0.0*x^2-23.007*x^3+100.02*x^4
        */        

        // degree and coefficients
        4,
        {
            0.0f,
            0.0f,
            0.0f,
            -23.007f,
            +100.02f
        },

        // interval
        {0.0f, 0.2f},

        // maximum x and y
        0.0f,
        0.0f

    },


    {
        
        /*
          NOTE: Reference data from inspecting graphs on Google:
          NOTE: Polynomial for google:
          0*​x^​0+​0*​x^​1-​1.023*​x^​2+​23.007*​x^​4
        */

        // degree and coefficients
        4,
        {
            0.0f,
            0.0f,
            -1.023f,
            0.0f,
            +23.007f
        },

        // interval
        {-0.2f, +0.2f},

        // maximum x and y
        0.0f,
        0.0f
        
    },

    {
        
        /*
          NOTE: Reference data from inspecting graphs on Google:
          NOTE: Polynomial for google:
          -10000+1000x-100x^2+10x^3-x^4+0.1x^5-0.01x^6
        */

        // degree and coefficients
        6,
        {

            -10000.0f,
            +1000.0f,
            -100.0f,
            +10.0f,
            -1.0f,
            +0.1f,
            -0.01f,
        },

        // interval
        {-10.0f, +15.0f},

        // maximum x and y
        6.7032f,
        -6350.939f,
        
    }    
    
};

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
