const float PI_FLOAT = (float)M_PI;
const float POSITIVE_INFINITY_FLOAT = (float)(+1e308 * 10);
const float NEGATIVE_INFINITY_FLOAT = (float)(-1e308 * 10);
// NOTE:
// We are assuming that if the most significant bit in the mantissa is set then the float is quiet.
// Not a huge assumption on the modern processors we are interested in.
const float QUIET_NAN_FLOAT = uint32_as_float(0x7fc00000);
const int MIN_INT = INT_MIN;
const int MAX_INT = INT_MAX;

