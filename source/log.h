#define LOG_OUTPUT_VISUAL_STUDIO_CONSOLE 1
#define LOG_OUTPUT_STDOUT 0

#define LOG_OUTPUT (LOG_OUTPUT_STDOUT)

#define LOG_EXPRESSION_FLOAT(expr) Log::string(#expr " = "); Log::floating_point(expr);
#define LOG_EXPRESSION_UNSIGNED_INTEGER_64(expr) Log::string(#expr " = "); Log::unsigned_integer_64(expr);

namespace Log
{
    
    inline void
    string(char const*const message)
    {
#if LOG_OUTPUT == LOG_OUTPUT_VISUAL_STUDIO_CONSOLE
        OutputDebugStringA(message);
#elif LOG_OUTPUT == LOG_OUTPUT_STDOUT
        printf(message);
#endif
    }
    
    inline void
    unsigned_integer_32(uint32 const n)
    {
        size_t const size =
            SerializationLimits::MAX_BASE_10_LENGTH_UNSIGNED_INTEGER_32 + 1;
        
        char buffer[size];
        int needed_buffer_size = _snprintf(buffer, size, "%" PRIu32, n);
        if(needed_buffer_size > size)
        {
            string("warning: log_uint32 buffer size too small");
            ENSURE(false);
            return;
        }
        else if(needed_buffer_size == size)
        {
            string("warning: log_uint32 buffer size too small to append a null terminator");
            ENSURE(false);            
            return;
        }
        ENSURE( buffer[needed_buffer_size] == '\0' );
        string(buffer);
    }

    inline void
    unsigned_integer(uint const n)
    {
        unsigned_integer_32(n);
    }
    
    inline void
    unsigned_integer_64(uint64 const n)
    {
        size_t const size = SerializationLimits::MAX_BASE_10_LENGTH_UNSIGNED_INTEGER_64 + 1;
        char buffer[size];
        int needed_buffer_size = _snprintf(buffer, size, "%" PRIu64, n);
        if(needed_buffer_size > size)
        {
            string("warning: log_uint32 buffer size too small");
            ENSURE(false);
            return;
        }
        else if(needed_buffer_size == size)
        {
            string("warning: log_uint32 buffer size too small to append a null terminator");
            ENSURE(false);            
            return;
        }
        ENSURE( buffer[needed_buffer_size] == '\0' );
        string(buffer);
    }    
    
    inline void integer(int const n)
    {
        // We need a buffer size of ceil(log10(2^32))=10,
        // plus one character for the sign, and one for zero termination
        size_t const size = 10+1+1;
        char buffer[size];
        int needed_buffer_size = _snprintf(buffer, size, "%d", n);
        if(needed_buffer_size > size)
        {
            string("warning: log_uint32 buffer size too small");
            ENSURE(false);
            return;
        }
        else if(needed_buffer_size == size)
        {
            string("warning: log_uint32 buffer size too small to append a null terminator");
            ENSURE(false);            
            return;
        }
        ENSURE( buffer[needed_buffer_size] == '\0' );
        string(buffer);
    }    
    
    inline void newline()
    {
        string("\n");
    }

    inline void newlines(int const n)
    {
        for(int i=0; i<n; ++i)
            newline();
    }

    inline void spaces(int const n)
    {
        for(int i=0; i<n; ++i)
            string(" ");
    }    
    
    
    inline void floating_point(float const x)
    {

        // TODO: find out how long a floating point number can be when printed
        // out as a string
        size_t const size = 100;
    
        char buffer[size];
        int needed_buffer_size = _snprintf(buffer, size, "%1.8e", x);
        if(needed_buffer_size > size){
            string("warning: log_float32 buffer size too small");
            return;
        }else if(needed_buffer_size == size){
            string("warning: log_float32 buffer size too small to append a null terminator");
            return;
        }
        ENSURE( buffer[needed_buffer_size] == '\0' );
        string(buffer);
        
    }

    // NOTE: num_digits_left and num_digits_right specify the number of digits to the left and right
    // of the decomal point, respectively.
    inline void floating_point_specific_precision(
        float const x, int const num_digits_left = 1, int const num_digits_right = 8
        )
    {

        // TODO: find out how long a floating point number can be when printed
        // out as a string
        size_t const size = 100;
    
        char buffer[size];
        int needed_buffer_size = _snprintf(buffer, size, "%*.*f", num_digits_left, num_digits_right, x);
        if(needed_buffer_size > size){
            string("warning: log_float32 buffer size too small");
            return;
        }else if(needed_buffer_size == size){
            string("warning: log_float32 buffer size too small to append a null terminator");
            return;
        }
        ENSURE( buffer[needed_buffer_size] == '\0' );
        string(buffer);
        
    }    

    // NOTE: num_digits_left and num_digits_right specify the number of digits to the left and right
    // of the decomal point, respectively.
    inline void
        floating_point_specific_precision_sign(
            float const x, int const num_digits_left = 1, int const num_digits_right = 8
            )
    {

        if(x >= float(0))
            string("+");
        floating_point_specific_precision(x, num_digits_left, num_digits_right);
        
    }
    
    void tuple_float32_2(float x, float y)
    {
        string("(");
        floating_point(x);
        string(", ");
        floating_point(y);
        string(")");
    
    }    
    
}
