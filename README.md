# gtsc
generic type safe C

## What's this?
gtsc provides generic typesafe containers, memory pools, and a few other things
like a string implementation, a bit vector, and array algos.

## Goals
1. Genericity; use containers with different types
2. Type safety; do 1. without losing your mind
3. Performance by non-pessimization; do the least amount of work needed (mostly)
4. Single binary text per module regardless of number of types; icache friendly

## How it works
void * are great, but dangerous:
```
#include <string.h>

typedef struct foo {int n;} foo;
typedef struct bar {int n;} bar;

int main(int argc, char * argv[])
{
	foo a; bar b;
	
	a.n = 10;
	memcpy(&b, &a, sizeof(foo)); // <-- bad and unsafe
	return 0;
}
```
This compiles perfectly fine and happily copies a type foo over a type bar.

Macros to the rescue:
```
#include <string.h>

typedef struct foo {int n;} foo;
typedef struct bar {int n;} bar;

#define MEMCPY_DEFINE(NAME, TYPE)                                              \
static inline TYPE * memcpy_##NAME(TYPE * dest, TYPE * src)                    \
{                                                                              \
    return (TYPE *)memcpy((void *)dest, (void *)src, sizeof(TYPE));            \
}

MEMCPY_DEFINE(foo_safe, foo);

int main(int argc, char * argv[])
{
	foo a; bar b;
	
	a.n = 10;
	memcpy_foo_safe(&b, &a);
	return 0;
}
```

This does not compile and provides a verbose error message:
```
tmp.c: In function ‘main’:
tmp.c:19:25: warning: passing argument 1 of ‘memcpy_foo_safe’ from incompatible pointer type [-Wincompatible-pointer-types]
   19 |         memcpy_foo_safe(&b, &a);
      |                         ^~
      |                         |
      |                         bar *
tmp.c:7:43: note: expected ‘foo *’ but argument is of type ‘bar *’
    7 | static inline TYPE * memcpy_##NAME(TYPE * dest, TYPE * src)                    \
      |                                           ^
tmp.c:12:1: note: in expansion of macro ‘MEMCPY_DEFINE’
   12 | MEMCPY_DEFINE(foo_safe, foo);
      | ^~~~~~~~~~~~~
```

When the types are correct and it does compile, the static inline wrapper exists
only when compiled with no optimizations:
```
$ gcc tmp.c -o tmp.bin
$ objdump -M intel -D tmp.bin | grep memcpy
0000000000001149 <memcpy_foo_safe>:
    119e:   e8 a6 ff ff ff          call   1149 <memcpy_foo_safe>
$
```

And goes away with optimizations turned on:
```
$ gcc tmp.c -o tmp.bin -O1
$ objdump -M intel -D tmp.bin | grep memcpy
$
```

This basic idea of a typeless void * base implementation and use of static
inline wrappers as a compile time type check construct with sizeof() guarantee
is extended to all generic types in gtsc.

## Templates?
Templates are cool, but this approach goes the opposite way. Templates, like in
C++ or in other C header and macro based implementations, generate code. That
is, generally, vector\<int\> would generate a vector implementation for ints, and
vector\<double\> would generate a different vector implementation for doubles.
There would be two separate push_back() functions, for example:
```
#include <vector>

int main(int argc, char * argv[])
{
	std::vector<int> ints;
	ints.push_back(1);
	
	std::vector<double> dbls;
	dbls.push_back(1.0);
	return 0;
}
...
$ objdump -M intel -D tmp.bin -C | grep push_back
    126d:   e8 56 02 00 00          call   14c8 <std::vector<int, std::allocator<int> >::push_back(int&&)>
    1299:   e8 46 03 00 00          call   15e4 <std::vector<double, std::allocator<double> >::push_back(double&&)>
00000000000014c8 <std::vector<int, std::allocator<int> >::push_back(int&&)>:
00000000000015e4 <std::vector<double, std::allocator<double> >::push_back(double&&)>:
```

While with gtsc only the typeless implementation remains:
```
#include "vect/vect.h"

VECT_DEFINE(int, int);
VECT_DEFINE(dbl, double);

int main(int argc, char * argv[])
{
	vect_int vint;
	vect_int_create(&vint);
	
	int n = 1;
	vect_int_push(&vint, &n);
	
	vect_dbl vdbl;
	vect_dbl_create(&vdbl);
	
	double d = 1.0;
	vect_dbl_push(&vdbl, &d);
	
	return 0;
}
...
$ objdump -M intel -D tmp.bin -C | grep 'vect_.*_push'
    1229:   e8 bf 01 00 00          call   13ed <vect_void_push>
    1259:   e8 8f 01 00 00          call   13ed <vect_void_push>
00000000000013ed <vect_void_push>:
    140c:   73 0d                   jae    141b <vect_void_push+0x2e>
    1419:   74 1f                   je     143a <vect_void_push+0x4d>
```

## Performance
I've benchmarked only the hash tables and hash sets, since these are the
interesting parts. oa_hash is an open address hash table, ch_hash is a chained
hash table, and ptr_hash is a chained hash table specialized only for pointers.
That is, it does not copy the elements inside itself, saves only pointers to
those elements. The data has to be allocated elsewhere, as is often the case in
practice. This avoids calling memcpy() all the time.

The English dictionary found on Linux systems with and without optimizations:
```
$ cat /usr/share/dict/words | ./hashes.no-opt.bin | column -s';' -t
Things  Rep  Type            Insert  Lookup  Remove  LFind   LNotFind  LAll    Iter
234937  1    oa_hash         0.063s  0.021s  0.013s  0.007s  0.015s    0.021s  0.004s
234937  1    ch_hash         0.063s  0.024s  0.015s  0.007s  0.010s    0.017s  0.007s
234937  1    ptr_hash        0.043s  0.021s  0.012s  0.006s  0.008s    0.014s  0.003s
234937  1    std::unord_map  0.068s  0.039s  0.030s  0.013s  0.012s    0.025s  0.006s

$ cat /usr/share/dict/words | ./hashes.opt.bin | column -s';' -t
Things  Rep  Type            Insert  Lookup  Remove  LFind   LNotFind  LAll    Iter
234937  1    oa_hash         0.039s  0.012s  0.007s  0.003s  0.008s    0.011s  0.002s
234937  1    ch_hash         0.029s  0.012s  0.008s  0.004s  0.005s    0.009s  0.002s
234937  1    ptr_hash        0.026s  0.013s  0.007s  0.003s  0.004s    0.007s  0.001s
234937  1    std::unord_map  0.030s  0.013s  0.014s  0.004s  0.004s    0.008s  0.002s
```

The test inserts all strings in the table (Insert), looks them all up (Lookup),
removes 40% (Remove), looks up all not removed (LFind), all removed (LNotFind),
all removed and not removed (LAll), and then iterates over the container.

2 million, random 20 character strings:
```
# such as these
$ cat /dev/random | tr -dc '[[:print:]]' | head -c $((20 * 2000000)) | sed -E 's/.{20}/&\n/g' | head -n5
`#s9DJz^>::z~ehujcEe
o.5<DKP.ww[`ETvkOqz(
>Ww$NTaO"^',6H )4|5%
gO6JzI4|-Xp*v048'&'>
r0v]3s(b_}=dYWd3,*3J

$ cat /dev/random | tr -dc '[[:print:]]' | head -c $((20 * 2000000)) | sed -E 's/.{20}/&\n/g' | ./hashes.no-opt.bin | column -s';' -t
Things   Rep  Type            Insert  Lookup  Remove  LFind   LNotFind  LAll    Iter
2000000  1    oa_hash         0.935s  0.403s  0.240s  0.128s  0.228s    0.357s  0.071s
2000000  1    ch_hash         0.930s  0.469s  0.287s  0.147s  0.224s    0.368s  0.112s
2000000  1    ptr_hash        0.777s  0.447s  0.271s  0.141s  0.207s    0.347s  0.067s
2000000  1    std::unord_map  1.068s  0.614s  0.517s  0.232s  0.234s    0.467s  0.135s

$ cat /dev/random | tr -dc '[[:print:]]' | head -c $((20 * 2000000)) | sed -E 's/.{20}/&\n/g' | ./hashes.opt.bin | column -s';' -t
Things   Rep  Type            Insert  Lookup  Remove  LFind   LNotFind  LAll    Iter
2000000  1    oa_hash         0.655s  0.326s  0.195s  0.102s  0.163s    0.262s  0.027s
2000000  1    ch_hash         0.643s  0.371s  0.183s  0.107s  0.123s    0.202s  0.029s
2000000  1    ptr_hash        0.562s  0.302s  0.158s  0.081s  0.122s    0.199s  0.026s
2000000  1    std::unord_map  0.720s  0.457s  0.334s  0.165s  0.127s    0.278s  0.067s
```

It fares pretty well against the std::unordered_map. This is not an attack on
STL. Gtsc's hash tables are not subject to a standardized library, so they take
a few liberties here and there. Like using pools to avoid malloc() and free().
Also, templates make possible certain inlinings which you simply cannot easily
or reliably approach in C. Like inlining comparison and hash functions. This is
factored out in the tests. If it's allowed, then it's really difficult to beat
std::unordered_map's lookup speed.
