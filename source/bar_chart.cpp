namespace BarChart
{

    // Context and Chart together represents a bar chart.
    // The range is 'lo' .. 'hi', and this is divided into 'num_bins' bins.
    // 'num_low_outliers' and 'num_high_outliers' keeps track of recorded entries
    // that were too low/high to hit the range.
    
    struct Context
    {
        float low;
        float high;
        int num_bins;        
    };
    
    struct Chart
    {
        // NOTE: counts is Context::num_bins entries long
        int *counts;
        int num_low_outliers;
        int num_high_outliers;
    };

    void
    initialize(
        Context *const ctx,
        Chart *const ch,
        float const low,
        float const high,
        int const num_bins,
        int *const counts
        )
    {
        // NOTE: zero the counts
        memset(counts, 0, sizeof(int)*num_bins);        
        
        ENSURE(low < high);
        ctx->low = low;
        ctx->high = high;
        ctx->num_bins = num_bins;
        ch->counts = counts;
        ch->num_low_outliers = 0;
        ch->num_high_outliers = 0;
    }
    
    void
    record_entry(Context const*const ctx, Chart *const ch, float const x)
    {
        using namespace Numerics;
        
        int const bin_idx = (int)floor( float(ctx->num_bins)*( (x - ctx->low)/(ctx->high - ctx->low) ) );
        if(bin_idx < 0)
        {
            ch->num_low_outliers++;
        }
        else if(bin_idx >= ctx->num_bins)
        {
            ch->num_high_outliers++;
        }
        else
        {
            ch->counts[bin_idx]++;
        }
    }

    // Extract the range covered by a bin of a given index. Works even if the index is out of buffer range.
    void
    bin_range(Context const*const ctx, int const bin_idx, float *const low, float *const high)
    {
        // NOTE: just an unlerp on hi,lo with parameter bin_idx/num_bins
        float const t_low = float(bin_idx + 0)/float(ctx->num_bins);
        float const t_high = float(bin_idx + 1)/float(ctx->num_bins);
        *low  = ctx->low + (ctx->high - ctx->low)*t_low;
        *high = ctx->low + (ctx->high - ctx->low)*t_high;
    }

    void
    bin_limit_low(Context const*const ctx, int const bin_idx, float *const low)
    {
        // NOTE: just an unlerp on lo,hi with parameter bin_idx/num_bins
        float const t = float(bin_idx + 0)/float(ctx->num_bins);
        *low = float(   ctx->low + (ctx->high - ctx->low)*t   );
    }    

    void
    bin_limit_high(Context const*const ctx, int const bin_idx, float *const high)
    {
        // NOTE: just an unlerp on hi,lo with parameter bin_idx/num_bins
        float const t = float(bin_idx + 1)/float(ctx->num_bins);
        *high = float(   ctx->low + (ctx->high - ctx->low)*t   );
    }        
    
    int
    bin_count(Chart const*const ch, int const bin_idx)
    {
        ENSURE(bin_idx >= 0);
        return ch->counts[bin_idx];
    }
    
}
