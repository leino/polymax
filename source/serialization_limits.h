// NOTE: needed because of Qt and it's MOC compiler
#pragma once

namespace SerializationLimits
{

#if defined(_MSC_VER)
	static_assert(INT32_MAX == +2147483647i32, "Invalid assumed maximum int size");
	static_assert(INT32_MIN == -2147483648i32, "Invalid assumed minimum int size");
#else
    static_assert(INT32_MAX == +2147483647, "Invalid assumed maximum int size");
	static_assert(INT32_MIN == -2147483648, "Invalid assumed minimum int size");
#endif
    
	int const MAX_BASE_10_LENGTH_INTEGER_32 = 11;

    static_assert(UINT32_MAX == 4294967295, "Invalid assumptions about uint32 range");
    int const MAX_BASE_10_LENGTH_UNSIGNED_INTEGER_32 = 10;

    static_assert(UINT64_MAX == 18446744073709551615u, "Invalid assumptions about uint64 range");
    int const MAX_BASE_10_LENGTH_UNSIGNED_INTEGER_64 = 20;

    // NOTE: "worst case" is "ff"
    int const MAX_BASE_16_LENGTH_UINT8 = 2;

	// NOTE: not yet a proven choice
	// TODO: prove it by testing all floats!
	// NOTE:
	// Here is my current "worst case" example:
	// "-1.23456789e-128"
	int const MAX_BASE_10_LENGTH_FLOATING_POINT_32 = 16;
}
