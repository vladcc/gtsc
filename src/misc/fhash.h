#ifndef FHASH_H
#define FHASH_H

// Module:      fhash
// Description: A collection of proven hash functions. None invented by me.
// Author: Vladimir Dinev
// vld.dinev@gmail.com
// 2022-10-05

static inline size_t hash_jenkins_oat(const unsigned char * data, size_t len)
{
    size_t hash = 0;
    for (const unsigned char * pch = data, * end = data+len; pch < end; ++pch)
    {
        hash += *pch;
        hash += hash << 10;
        hash ^= hash >> 6;
    }

    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;

    return hash;
}

static inline size_t hash_djb2(const unsigned char * data, size_t len)
{
    size_t hash = 5381;
    for (const unsigned char * pch = data, * end = data+len; pch < end; ++pch)
        hash = ((hash << 5) + hash) + *pch; /* hash * 33 + val */
    return hash;
}

static inline size_t hash_sdbm(const unsigned char * data, size_t len)
{
    size_t hash = 0;
    for (const unsigned char * pch = data, * end = data+len; pch < end; ++pch)
        hash = *pch + (hash << 6) + (hash << 16) - hash;
    return hash;
}

static inline size_t hash_dek(const unsigned char * data, size_t len)
{
    size_t hash = 0;
    for (const unsigned char * pch = data, * end = data+len; pch < end; ++pch)
        hash = ((hash << 5) ^ (hash >> 27)) ^ (*pch);
    return hash;
}
#endif
