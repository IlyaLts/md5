#ifndef __MD5_H__
#define __MD5_H__

#if defined(_WIN32) && (!(defined(_CRT_SECURE_NO_WARNINGS)))
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long long hash64_t;

typedef struct
{
    hash64_t data[2];
} hash_t;

hash_t md5(const void *input, size_t len);
hash64_t md5_64(const void *input, size_t len);

#ifdef __cplusplus
}
#endif

#endif // !__MD5_H__
