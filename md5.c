/*
===============================================================================
    Copyright (C) 2011-2023 Ilya Lyakhovets

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
===============================================================================
*/

// https://en.wikipedia.org/wiki/MD5

#include "md5.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define CHUNK_SIZE 64

#define LEFT_ROTATE(x, s) (((x) << (s)) | ((x) >> (32 - (s))))

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

// Per-round shift amounts
static const char r[] =
{
    7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
    5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
};

// Binary integer part of the sines of integers (Radians)
static const uint32_t k[] =
{
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

hash_t md5(const void *input, size_t len)
{
    // Init
    uint32_t a0 = 0x67452301;
    uint32_t b0 = 0xefcdab89;
    uint32_t c0 = 0x98badcfe;
    uint32_t d0 = 0x10325476;

    int input_index = 0;

    // Multiple of 64 bytes(512 bits) without extra space for length information
    int padded_len = (((((int)len + 8) / 64) + 1) * 64) - 8;

    // In bytes
    uint32_t input_size = 8 * (int)len;

    while (input_index < padded_len)
    {
        uint8_t chunk[CHUNK_SIZE];

        // Pre-processing
        for (int i = 0; i < CHUNK_SIZE;)
        {
            if (input_index < (int)len)
            {
                int size = max(min(64, ((int)len - input_index)), 1);
                memcpy(chunk + i, (uint8_t *)(input) +input_index, size);
                i += size;
                input_index += size;
            }
            else if (input_index == (int)len)
            {
                chunk[i] = 0x80;
                i++;
                input_index++;
            }
            else if (input_index >= padded_len && input_index < padded_len + 4)
            {
                chunk[i] = ((uint8_t *)(&input_size))[input_index - padded_len];
                i++;
                input_index++;
            }
            else
            {
                int size = max(min(64, ((int)padded_len - input_index)), 1);
                memset(chunk + i, 0, size);
                i += size;
                input_index += size;
            }
        }

        // Break chunk into sixteen 32-bit words m[j], 0 ≤ j ≤ 15
        uint32_t *m = (uint32_t *)(chunk);

        // Initialize hash value for this chunk
        uint32_t a = a0;
        uint32_t b = b0;
        uint32_t c = c0;
        uint32_t d = d0;

        for (int j = 0; j < CHUNK_SIZE; j++)
        {
            uint32_t f;
            uint32_t g;

            if (j < 16)
            {
                f = (b & c) | ((~b) & d);
                g = j;
            }
            else if (j < 32)
            {
                f = (d & b) | ((~d) & c);
                g = (5 * j + 1) % 16;
            }
            else if (j < 48)
            {
                f = b ^ c ^ d;
                g = (3 * j + 5) % 16;
            }
            else
            {
                f = c ^ (b | (~d));
                g = (7 * j) % 16;
            }

            f += a + k[j] + m[g];   // m[g] must be a 32-bit block
            a = d;
            d = c;
            c = b;
            b += LEFT_ROTATE(f, r[j]);
        }

        a0 += a;
        b0 += b;
        c0 += c;
        d0 += d;
    }

    return (hash_t) { a0 | (hash64_t)b0 << 32, c0 | (hash64_t)d0 << 32 };
}

hash64_t md5_64(const void *input, size_t len)
{
    hash_t hash = md5(input, len);
    return hash.data[0] ^ hash.data[1];
}
