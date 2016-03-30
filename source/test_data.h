// NOTE:
// The test data I'm comparing against comes from copy-pasting in
// the polynomials int Google and inspecting their graphs.
//
// Very qucik-n-dirty but works so far in the sense that I reproduce the results
// to within a certain tolerance.

struct TestCase
{
    int degree;
    float coefficients[Polymax::MAX_DEGREE+1];
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
        
    },

    {
        
        /*
          NOTE: Reference data from inspecting graphs on Google:
          NOTE: Polynomial for google:
          -11784. x^5+23059.5 x^6+4269.85 x^7+163.306 x^8-2.13 x^9
        */

        // degree and coefficients
        9,
        {
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f, // x^0, x^1, x^2, x^3, x^4
            -11784.0f, // x^5
            +23059.5f, // x^6
            +4269.85f, // x^7
            +163.306f, // x^8
            -2.13f,    // x^9
            
        },

        // interval
        {-12.0f, +3.0f},

        // maximum x and y
        -8.00641044f,
        +5.022308e8f,
        
    } 

    /*
      -2.78619981
      +1.38923550 *  x
      -2.01507425 * x^2
      -1.58375883 * x^3
      +2.93501425 * x^4
      -1.19036698 * x^5
      +4.91635609 * x^6
      -3.16007376 * x^7

    */
    
    
};
