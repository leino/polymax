namespace Log
{
    
    void
    polynomial(
        float const*const coefficients,
        int const degree
        )
    {
        for(int coefficient_idx=0; coefficient_idx <= degree; coefficient_idx++)
        {
            string("    ");
            float const coefficient = coefficients[coefficient_idx];
            floating_point(coefficient);
            if(coefficient_idx > 0)
            {
                string("*x^");
                integer(coefficient_idx);
            }
            if(coefficient_idx < degree)
                string(" + ");
            newline();
        }    
    }
    
}
