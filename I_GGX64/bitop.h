#ifndef __BITOP_H__
#define __BITOP_H__

#if defined(_MSC_VER) || defined(__MINGW32__)
#else
    static __inline unsigned char _BitScanForward64(unsigned long* Index, unsigned long long Mask)
    {
        if (!Mask)
            return 0;
        *Index = (unsigned long)__builtin_ctzll(Mask);
        return 1;
    }
    static __inline unsigned char _BitScanReverse64(unsigned long* Index, unsigned long long Mask)
    {
        if (!Mask)
            return 0;
        *Index = (unsigned long)(63 - __builtin_clzll(Mask));
        return 1;
    }
#endif

#endif