#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "integer.h"
#include "numerics.cpp"
#include "log.h"
namespace Polyroot{ int const MAX_DEGREE = 7; }
#include "polyroot.cpp"

int
main(int argument_count, char** arguments)
{
    argument_count; arguments;

    using namespace Polyroot;
    
    float roots[MAX_DEGREE][MAX_DEGREE] = {};

    float const min_x = -100.0f;
    float const max_x = +10.0f;

    int const highest_degree = 5;
    assert(highest_degree <= MAX_DEGREE);

    float coefficients[MAX_DEGREE][MAX_DEGREE] = {};
    coefficients[highest_degree][0] = +1.0123f;
    coefficients[highest_degree][1] = -1.032f;
    coefficients[highest_degree][2] = -3.012f;
    coefficients[highest_degree][3] = +2.012f;
    coefficients[highest_degree][4] = +1.0123f;
    coefficients[highest_degree][5] = +1.0f;

    // NOTE: compute all of the derivatives
    for(int degree = highest_degree-1; degree >= 0; degree--)
    {
        for(int coefficient_idx=0; coefficient_idx <= degree; coefficient_idx++)
        {
            coefficients[degree][coefficient_idx] =
                float(coefficient_idx+1)*coefficients[degree+1][coefficient_idx+1];
        }
    }

    {
        int const num_found_roots = try_find_roots(min_x, max_x, coefficients, highest_degree, roots);
        using namespace Log;

        for(int root_idx=0; root_idx < num_found_roots; root_idx++)
        {
            unsigned_integer(root_idx);
            string(": ");
            float const root = roots[highest_degree][root_idx];
            floating_point(root);
            string(", value: ");
            floating_point(evaluate(coefficients[highest_degree], highest_degree, root));
            newline();
        }
        
    }
    
    return EXIT_SUCCESS;        
}
