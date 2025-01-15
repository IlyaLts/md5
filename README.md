md5
======
Simple implementation of the MD5 hashing algorithm.

### Usage
```
#include "md5.h"

int main()
{
	hash_t hash = md5("Hello, World!");
	// hash.data[0];
	// hash.data[1];
	
	hash64_t hash64 = md5_64("Hello, World!");

	return 0;
}
```

## Documentation
  
| Types | Descriptions |
| --- | --- |
| hash_t | A struct with two 64-bit unsigned integers. (data[2]) |
| hash64_t | 64-bit unsigned integer. |

| Functions | Descriptions |
| --- | --- |
| hash_t md5(const void *input, size_t len) | Calculates and returns an MD5 hash from an input. (hash_t) |
| hash64_t md5_64(const void *input, size_t len) | Calculates and returns a custom 64-bit version of MD5 hash from an input. |
