#define LOG_OUTPUT_VISUAL_STUDIO_CONSOLE 1
#define LOG_OUTPUT_STDOUT 0

#define LOG_OUTPUT (LOG_OUTPUT_STDOUT)

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
    unsigned_integer(uint32 const n)
    {
        // We need a buffer size of ceil(log10(2^32))=10,
        // plus one character for the sign bit, and one for zero termination
        size_t const size = 10+1+1;
        char buffer[size];
        int needed_buffer_size = _snprintf(buffer, size, "%d", n);
        if(needed_buffer_size > size)
        {
            string("warning: log_uint32 buffer size too small");
            assert(false);
            return;
        }
        else if(needed_buffer_size == size)
        {
            string("warning: log_uint32 buffer size too small to append a null terminator");
            assert(false);            
            return;
        }
        assert( buffer[needed_buffer_size] == '\0' );
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
            assert(false);
            return;
        }
        else if(needed_buffer_size == size)
        {
            string("warning: log_uint32 buffer size too small to append a null terminator");
            assert(false);            
            return;
        }
        assert( buffer[needed_buffer_size] == '\0' );
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
    
    
    inline void floating_point(float const x)
    {

        // TODO: find out how long a floating point number can be when printed
        // out as a string
        size_t const size = 100;
    
        char buffer[size];
        int needed_buffer_size = _snprintf(buffer, size, "%f", x);
        if(needed_buffer_size > size){
            string("warning: log_float32 buffer size too small");
            return;
        }else if(needed_buffer_size == size){
            string("warning: log_float32 buffer size too small to append a null terminator");
            return;
        }
        assert( buffer[needed_buffer_size] == '\0' );
        string(buffer);
        
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
