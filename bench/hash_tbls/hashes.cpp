#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>

#include <iostream>
#include <string>
#include <vector>

#include <map>
#include <unordered_map>

#include "oa_hash/oa_hash.h"
#include "ch_hash/ch_hash.h"
#include "ptr_hash/ptr_hash.h"

#define key_type char *
#define val_type int

OA_HASH_DEFINE(htyp, key_type, val_type);
CH_HASH_DEFINE(htyp, key_type, val_type);
PTR_HASH_DEFINE(htyp, key_type, val_type);

static inline size_t hash_djb2_str(const char * data)
{
	size_t hash = 5381;
	while (*data)
	{
		hash = ((hash << 5) + hash) + *data; /* hash * 33 + val */
		++data;
	}	 
	return hash;
}

static int cmp_key_type(const void * k1, const void * k2)
{
	key_type * s1 = (key_type *)k1;
    key_type * s2 = (key_type *)k2;
    return strcmp(*s1, *s2);
}

static size_t hash_key_type(const void * key)
{
	key_type * str = (key_type *)key;
	return hash_djb2_str(*str);
}

static int cmp_key_type_ptrh(const void * k1, const void * k2)
{
	key_type s1 = (key_type)k1;
    key_type s2 = (key_type )k2;
    return strcmp(s1, s2);
}

static size_t hash_key_type_ptrh(const void * key)
{
	key_type str = (key_type)key;
	return hash_djb2_str(str);
}

void equit(std::string msg)
{
	puts(std::string("error: " + msg).c_str());
	exit(EXIT_FAILURE);
}

#define STL_PTR_CALLS
#ifdef STL_PTR_CALLS
struct Hash_key_type
{
	Hash_key_type() : hash(hash_key_type) {}
	chh_get_hash hash;
	
    size_t operator()(key_type n) const
    {
        return hash((const void *)&n);
    }
};

struct Equal_key_type
{
	Equal_key_type() : cmp(cmp_key_type) {}
	chh_compar cmp;
	
    bool operator()(key_type lhs, key_type rhs) const
    {
        return (cmp((const void *)&lhs, (const void *)&rhs) == 0);
    }
};

struct Less_key_type
{
	Less_key_type() : cmp(cmp_key_type) {}
	chh_compar cmp;
	
    bool operator()(key_type lhs, key_type rhs) const
    {
        return (cmp((const void *)&lhs, (const void *)&rhs) < 0);
    }
};
#else
struct Hash_key_type
{
    size_t operator()(key_type n) const
    {
        return hash_key_type((const void *)&n);
    }
};

struct Equal_key_type
{
    bool operator()(key_type lhs, key_type rhs) const
    {
        return (cmp_key_type((const void *)&lhs, (const void *)&rhs) == 0);
    }
};

struct Less_key_type
{
    bool operator()(key_type lhs, key_type rhs) const
    {
        return (cmp_key_type((const void *)&lhs, (const void *)&rhs) < 0);
    }
};
#endif



class container {
	public:
	virtual void insert(const key_type * key, const val_type * val) = 0;
	virtual val_type * lookup(const key_type * key) = 0;
	virtual const char * type(void) = 0;
	virtual void remove(const key_type * key) = 0;
	virtual void clear(void) = 0;
	virtual void iter_init(void) = 0;
	virtual const key_type * iter_next(val_type ** out_val) = 0;
	virtual size_t size(void) = 0;
};

class std_map : public container
{	
	public:
	void insert(const key_type * key, const val_type * val) override
	{
		omap[*((key_type*)key)] = *val;
	}
	val_type * lookup(const key_type * key) override
	{
		const auto & it = omap.find(*((key_type*)key));
		return (it != omap.end()) ? &(it->second) : NULL;
	}
	void remove(const key_type * key) override
	{
		omap.erase(*((key_type*)key));
	}
	void clear(void) override
	{
		omap.clear();
	}
	const char * type(void) override
	{
		return "std::map";
	}
	size_t size(void) override
	{
		return omap.size();
	}
	
	void iter_init(void)
	{
		iter = omap.begin();
	}
	const key_type * iter_next(val_type ** out_val)
	{
		const key_type * pkey = NULL;
		if (iter != omap.end())
		{
			pkey = (const key_type *)&(iter->first);
			*out_val = &(iter->second);
			++iter;
		}
		return pkey;
	}
	
	private:
	std::map<key_type, val_type, Less_key_type> omap;
	std::map<key_type, val_type, Less_key_type>::iterator iter;
};

class std_umap : public container
{	
	public:
	std_umap()
	{
		umap.reserve(32);
	}
	void insert(const key_type * key, const val_type * val) override
	{
		umap[*((key_type*)key)] = *val;
	}
	val_type * lookup(const key_type * key) override
	{
		const auto & it = umap.find(*((key_type*)key));
		return (it != umap.end()) ? &(it->second) : NULL;
	}
	void remove(const key_type * key) override
	{
		umap.erase(*((key_type*)key));
	}
	void clear(void) override
	{
		umap.clear();
	}
	const char * type(void) override
	{
		return "std::unord_map";
	}
	size_t size(void) override
	{
		return umap.size();
	}
	
	void iter_init(void)
	{
		iter = umap.begin();
	}
	const key_type * iter_next(val_type ** out_val)
	{
		const key_type * pkey = NULL;
		if (iter != umap.end())
		{
			pkey = (const key_type *)&(iter->first);
			*out_val = &(iter->second);
			++iter;
		}
		return pkey;
	}
	
	private:
	std::unordered_map<key_type, val_type, Hash_key_type, Equal_key_type> umap;
	std::unordered_map<key_type, val_type, Hash_key_type, Equal_key_type>::iterator iter;
};

class oa_hash : public container
{
	public:
	oa_hash()
	{
		oah_htyp_create(&oah, hash_key_type, cmp_key_type);
	}
	~oa_hash()
	{
		oah_htyp_destroy(&oah);
	}
	
	void insert(const key_type * key, const val_type * val) override
	{
		oah_htyp_insert(&oah, key, val);
	}
	val_type * lookup(const key_type * key) override
	{
		return oah_htyp_lookup(&oah, key);
	}
	void remove(const key_type * key) override
	{
		oah_htyp_remove(&oah, key);
	}
	void clear(void) override
	{
		oah_htyp_clear(&oah);
	}
	const char * type(void) override
	{
		return "oa_hash";
	}
	size_t size(void) override
	{
		return oah_htyp_entries(&oah);
	}
	
	void iter_init(void)
	{
		iter = OAH_ITER_INIT;
	}
	const key_type * iter_next(val_type ** out_val)
	{
		return oah_htyp_iterate(&oah, &iter, out_val);
	}
	
	private:
	oah_htyp oah;
	oah_iterator iter;
};

class ch_hash : public container
{
	public:
	ch_hash()
	{
		chh_htyp_create(&chh, hash_key_type, cmp_key_type);
	}
	~ch_hash()
	{
		chh_htyp_destroy(&chh);
	}
	void insert(const key_type * key, const val_type * val) override
	{
		chh_htyp_insert(&chh, key, val);
	}
	val_type * lookup(const key_type * key) override
	{
		return chh_htyp_lookup(&chh, key);
	}
	void remove(const key_type * key) override
	{
		chh_htyp_remove(&chh, key);
	}
	void clear(void) override
	{
		chh_htyp_clear(&chh);
	}
	const char * type(void) override
	{
		return "ch_hash";
	}
	size_t size(void) override
	{
		return chh_htyp_elems(&chh);
	}
	
	void iter_init(void)
	{
		chh_iterator_init(&iter);
	}
	const key_type * iter_next(val_type ** out_val)
	{
		return chh_htyp_iterate(&chh, &iter, out_val);
	}
	
	private:
	chh_htyp chh;
	chh_iterator iter;
};

class ptr_hash : public container
{
	public:
	ptr_hash()
	{
		ptrh_htyp_create(&ptrh, hash_key_type_ptrh, cmp_key_type_ptrh);
	}
	~ptr_hash()
	{
		ptrh_htyp_destroy(&ptrh);
	}
	void insert(const key_type * key, const val_type * val) override
	{
		ptrh_htyp_insert(&ptrh, (const char **)*key,
			(const int *)((size_t)*val));
	}
	val_type * lookup(const key_type * key) override
	{
		static int num;
		num = (int)((size_t)ptrh_htyp_lookup(&ptrh, (const char **)*key));
		return num ? &num : NULL;
	}
	void remove(const key_type * key) override
	{
		ptrh_htyp_remove(&ptrh, (const char **)*key);
	}
	void clear(void) override
	{
		ptrh_htyp_clear(&ptrh);
	}
	const char * type(void) override
	{
		return "ptr_hash";
	}
	size_t size(void) override
	{
		return ptrh_htyp_elems(&ptrh);
	}
	
	void iter_init(void)
	{
		ptrh_iterator_init(&iter);
	}
	const key_type * iter_next(val_type ** out_val)
	{
		static size_t num_, * num = &num_;
		*out_val = (int *)num;
		
		static char * str, ** pstr = &str;
		
		str = ((char *)ptrh_htyp_iterate(&ptrh, &iter, (int **)&num_));
		
		return (str) ? (const char **)pstr : NULL;
	}
	
	private:
	ptrh_htyp ptrh;
	ptrh_iterator iter;
};

double bench_ins(const std::vector<key_type>& keys, container * cont)
{
	const key_type * key = (const key_type *)keys.data();
	val_type val = 0;
	
	size_t old_ecount = cont->size();
	
	clock_t begin = clock();
	for (size_t i = 0, end = keys.size(); i < end; ++i)
	{
		val = (*(key+i))[0];
		cont->insert(key+i, &val);
	}
	clock_t end = clock();
	
	if ((old_ecount + keys.size()) != cont->size())
	{
		std::string str_err(cont->type());
		str_err += " bad size in bench_ins()";
		equit(str_err.c_str());
	}
	return (double)(end - begin) / CLOCKS_PER_SEC;
}

double bench_lookup_find(const std::vector<key_type>& keys, container * cont)
{
	std::string str_err(cont->type());
	str_err += " is wrong in bench_lookup_find()";
	
	const key_type * key = (const key_type *)keys.data();
	const val_type * pval = NULL;
	
	clock_t begin = clock();
	for (size_t i = 0, end = keys.size(); i < end; ++i)
	{
		pval = cont->lookup(key+i);
		if (pval && !(*pval == (*(key+i))[0]))
			equit(str_err.c_str());
	}
	clock_t end = clock();
	
	return (double)(end - begin) / CLOCKS_PER_SEC;
}

double bench_lookup_not_find(
	const std::vector<key_type>& keys,
	container * cont
)
{
	std::string str_err(cont->type());
	str_err += " is wrong in bench_lookup_not_find()";
	
	const key_type * key = (const key_type *)keys.data();
	
	clock_t begin = clock();
	for (size_t i = 0, end = keys.size(); i < end; ++i)
	{
		if (cont->lookup(key+i))
			equit(str_err.c_str());
	}
	clock_t end = clock();
	
	return (double)(end - begin) / CLOCKS_PER_SEC;
}

double bench_lookup_all(const std::vector<key_type>& vals, container * cont)
{
	const key_type * val = (const key_type *)vals.data();
	
	clock_t begin = clock();
	for (size_t i = 0, end = vals.size(); i < end; ++i)
		cont->lookup(val+i);
	clock_t end = clock();
	
	return (double)(end - begin) / CLOCKS_PER_SEC;
}

double bench_iterate(container * cont, size_t how_many)
{
	const key_type * key = NULL;
	val_type * val = NULL;
	
	std::string str_err(cont->type());
	str_err += " is wrong in bench_iterate()";
	
	if (cont->size() != how_many)
	{
		str_err += " 1: cont size: " + std::to_string(cont->size()) + ", how_many: " + std::to_string(how_many);
		equit(str_err.c_str());
	}
	
	size_t iterated = 0;
	
	clock_t begin = clock();
	cont->iter_init();
	while ((key = cont->iter_next(&val)))
	{
		++iterated;
		if (val && !(*val == (*key)[0]))
			equit(str_err.c_str());
	}
	clock_t end = clock();
	
	if (cont->size() != iterated)
	{
		str_err += " 2: cont size: " + std::to_string(cont->size()) + ", iterated: " + std::to_string(iterated);
		equit(str_err.c_str());
	}
	
	return (double)(end - begin) / CLOCKS_PER_SEC;
}

double bench_rm(const std::vector<key_type>& vals, container * cont)
{
	const key_type * val = (const key_type *)vals.data();
	
	size_t old_ecount = cont->size();
	
	clock_t begin = clock();
	for (size_t i = 0, end = vals.size(); i < end; ++i)
		cont->remove(val+i);
	clock_t end = clock();
	
	if ((old_ecount - vals.size()) != cont->size())
	{
		std::string str_err(cont->type());
		str_err += " bad size in bench_rm()";
		equit(str_err.c_str());
	}
	
	return (double)(end - begin) / CLOCKS_PER_SEC;
}

typedef struct times {
	const char * type;
	double insert;
	double lookup_full_find;
	double remove;
	double lookup_non_full_find;
	double lookup_non_full_not_find;
	double lookup_non_full_all;
	double iteration;
} times;


#define RM_PERCENT (0.4)
void bench_run(
	const std::vector<key_type>& vals,
	std::vector<container *> conts,
	std::vector<times>& acc_times
)
{
	size_t rm_start = RM_PERCENT * vals.size();
	std::vector<key_type> removed(vals.begin() + rm_start, vals.end());
	std::vector<key_type> not_removed(vals.begin(), vals.begin() + rm_start);
	
	for (size_t i = 0, end = conts.size(); i < end; ++i)
	{
		times * tm = &acc_times[i];
		container * cont = conts[i];
		
		tm->type = cont->type();
		tm->insert += bench_ins(vals, cont);
		tm->lookup_full_find += bench_lookup_find(vals, cont);
		tm->remove += bench_rm(removed, cont);
		
		tm->lookup_non_full_find += bench_lookup_find(not_removed, cont);
		tm->lookup_non_full_not_find += bench_lookup_not_find(removed, cont);
		tm->lookup_non_full_all += bench_lookup_all(vals, cont);
		tm->iteration += bench_iterate(cont, not_removed.size());
	}
}

void bench(std::vector<key_type>& vect, size_t repeat)
{	
	std_umap umap;
	std_map omap;
	oa_hash oah;
	ch_hash chh;
	ptr_hash ptrh;
	
	std::vector<container *> conts;
	conts.push_back(&oah);
	conts.push_back(&chh);
	conts.push_back(&ptrh);
	conts.push_back(&umap);
	//conts.push_back(&omap);
	
	times tm = {};
	std::vector<times> tms;
	for (size_t i = 0, end = conts.size(); i < end; ++i)
		tms.push_back(tm);
	
	for (size_t i = 0; i < repeat; ++i)
	{
		bench_run(vect, conts, tms);
		
		for (size_t j = 0, end = conts.size(); j < end; ++j)
			conts[j]->clear();
	}
		
	puts("Things;Rep;Type;Insert;Lookup;Remove;LFind;LNotFind;LAll;Iter");
	
	times * ptm = nullptr;
	for (size_t i = 0, end = tms.size(); i < end; ++i)
	{
		ptm = &tms[i];
		printf("%zu;%zu;%s;%.3fs;%.3fs;%.3fs;%.3fs;%.3fs;%.3fs;%.3fs\n",
			vect.size(),
			repeat,
			ptm->type,
			ptm->insert,
			ptm->lookup_full_find,
			ptm->remove,
			ptm->lookup_non_full_find,
			ptm->lookup_non_full_not_find,
			ptm->lookup_non_full_all,
			ptm->iteration
		);
	}
}

int main(int argc, char * argv[])
{
	//equit("Use: cat <string-file> | <prog> [num-repeat]");

	size_t repeat = 1;
	if (argc > 1)
	{
		if (1 != sscanf(argv[1], "%zu", &repeat))
			equit("Use: cat <string-file> | <prog> [num-repeat]");
	}

	std::vector<std::string> vstr;
	std::string line;
	
	while (getline(std::cin, line))
		vstr.push_back(line);
	
	std::vector<key_type> key_vect;
	for (auto& str : vstr)
		key_vect.push_back((key_type)str.c_str());
	
	bench(key_vect, repeat);
	
	return 0;
}
