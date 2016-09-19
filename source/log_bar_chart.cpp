namespace Log
{
    
    using namespace BarChart;
    
    void
    bar_chart(Chart const*const ch, Context const*const ctx)
    {
        ENSURE(ctx->high >= ctx->low);
        
        // NOTE: use appropriate number of significant digits to display the output in, so as to not
        // clutter things up unneccessarily, yet still being able to distinguish bin limits
        float const range = ctx->high - ctx->low;
        float const bin_range = range/float(ctx->num_bins);
        float const bin_range_exponent = Numerics::log10_float(bin_range);
        int const num_displayed_significant_digits =
            (int)Numerics::ceiling(Numerics::abs(bin_range_exponent));

        int const num_displayed_decimals = bin_range_exponent < 0 ? num_displayed_significant_digits : 0;
        // NOTE: num digits on left/right side of decimal point
        int const num_digits_left = num_displayed_significant_digits - num_displayed_decimals;
        int const num_digits_right = num_displayed_decimals;
        
        
        if(ch->num_low_outliers > 0)
        {
            using namespace Log;

            spaces(Numerics::max_int(1, num_digits_left) + num_digits_right + 2);
            string(" ... ");
            floating_point_specific_precision_sign(ctx->low, num_digits_left, num_digits_right);
            string(": ");
            integer(ch->num_low_outliers);
            newline();
        }

        // NOTE: we want to summarize long runs of empty bins, so we are printing ranges of bins rather than
        // individual bins (although a range can have length 1, of course)
        int lo_bin_idx = 0;
        while(lo_bin_idx < ctx->num_bins)
        {

            using namespace Log;


            int count = ch->counts[lo_bin_idx];
            int hi_bin_idx = lo_bin_idx+1;
            if(count == 0)
            {
                while(hi_bin_idx < ctx->num_bins)
                {
                    if(ch->counts[hi_bin_idx] != 0)
                    {
                        break;
                    }
                    hi_bin_idx++;
                }
            }

            float lo;
            BarChart::bin_limit_low(ctx, lo_bin_idx, &lo);
            float hi;
            BarChart::bin_limit_high(ctx, hi_bin_idx-1, &hi);

#if 0
            string("[");
            integer(lo_bin_idx);
            string(", ");
            integer(hi_bin_idx);
            string(")");
            string(" ");            
#endif
            

            floating_point_specific_precision_sign(lo, num_digits_left, num_digits_right);
            string(" ... ");
            floating_point_specific_precision_sign(hi, num_digits_left, num_digits_right);
            string(": ");
            integer(count);
            newline();


            lo_bin_idx = hi_bin_idx;
        }
        if(ch->num_high_outliers > 0)
        {
            using namespace Log;
            floating_point_specific_precision_sign(ctx->high, num_digits_left, num_digits_right);
            string(" ... ");
            spaces(Numerics::max_int(1, num_digits_left) + num_digits_right + 2);
            string(": ");
            
            integer(ch->num_high_outliers);
        }
        
    }

}
