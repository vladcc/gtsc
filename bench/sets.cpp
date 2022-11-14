#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>

#include <iostream>
#include <string>
#include <vector>

#include <set>
#include <unordered_set>

#include "oa_set/oa_set.h"
#include "ch_set/ch_set.h"
#include "ptr_set/ptr_set.h"

#define key_type char *

#include "gtsc_set_types.h"

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

static int cmp_key_type_ptrs(const void * k1, const void * k2)
{
	key_type s1 = (key_type)k1;
    key_type s2 = (key_type)k2;
    return strcmp(s1, s2);
}

static size_t hash_key_type_ptrs(const void * key)
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
	chs_get_hash hash;
	
    size_t operator()(key_type n) const
    {
        return hash((const void *)&n);
    }
};

struct Equal_key_type
{
	Equal_key_type() : cmp(cmp_key_type) {}
	chs_compar cmp;
	
    bool operator()(key_type lhs, key_type rhs) const
    {
        return (cmp((const void *)&lhs, (const void *)&rhs) == 0);
    }
};

struct Less_key_type
{
	Less_key_type() : cmp(cmp_key_type) {}
	chs_compar cmp;
	
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
	virtual void insert(const key_type * key) = 0;
	virtual const key_type * lookup(const key_type * key) = 0;
	virtual const char * type(void) = 0;
	virtual void remove(const key_type * key) = 0;
	virtual void clear(void) = 0;
	virtual void iter_init(void) = 0;
	virtual const key_type * iter_next() = 0;
	virtual bool cmp(void * a, void * b) = 0;
	virtual size_t size(void) = 0;
};

class std_set : public container
{	
	public:
	bool cmp(void * a, void * b) override
	{
		return (cmp_key_type(a, b) == 0);
	}
	void insert(const key_type * key) override
	{
		oset.insert(*((key_type*)key));
	}
	const key_type * lookup(const key_type * key) override
	{
		const auto & it = oset.find(*((key_type*)key));
		return (it != oset.end()) ? (const key_type*)&(*it) : NULL;
	}
	void remove(const key_type * key) override
	{
		oset.erase(*((key_type*)key));
	}
	void clear(void) override
	{
		oset.clear();
	}
	const char * type(void) override
	{
		return "std::set";
	}
	size_t size(void) override
	{
		return oset.size();
	}
	
	void iter_init(void) override
	{
		iter = oset.begin();
	}
	const key_type * iter_next() override
	{
		const key_type * pkey = NULL;
		if (iter != oset.end())
		{
			pkey = (const key_type *)&iter;
			++iter;
		}
		return pkey;
	}
	
	private:
	std::set<key_type, Less_key_type> oset;
	std::set<key_type, Less_key_type>::iterator iter;
};

class std_uset : public container
{	
	public:
	std_uset()
	{
		uset.reserve(32);
	}
	bool cmp(void * a, void * b) override
	{
		return (cmp_key_type(a, b) == 0);
	}
	void insert(const key_type * key) override
	{
		uset.insert(*((key_type*)key));
	}
	const key_type * lookup(const key_type * key) override
	{
		const auto & it = uset.find(*((key_type*)key));
		return (it != uset.end()) ? (const key_type *)&(*it) : NULL;
	}
	void remove(const key_type * key) override
	{
		uset.erase(*((key_type*)key));
	}
	void clear(void) override
	{
		uset.clear();
	}
	const char * type(void) override
	{
		return "std::unord_set";
	}
	size_t size(void) override
	{
		return uset.size();
	}
	
	void iter_init(void) override
	{
		iter = uset.begin();
	}
	const key_type * iter_next() override
	{
		const key_type * pkey = NULL;
		if (iter != uset.end())
		{
			pkey = (const key_type *)&iter;
			++iter;
		}
		return pkey;
	}
	
	private:
	std::unordered_set<key_type, Hash_key_type, Equal_key_type> uset;
	std::unordered_set<key_type, Hash_key_type, Equal_key_type>::iterator iter;
};

class oa_set : public container
{
	public:
	oa_set()
	{
		oas_htyp_create(&oas, hash_key_type, cmp_key_type);
	}
	~oa_set()
	{
		oas_htyp_destroy(&oas);
	}
	bool cmp(void * a, void * b) override
	{
		return (cmp_key_type(a, b) == 0);
	}
	void insert(const key_type * key) override
	{
		oas_htyp_insert(&oas, key);
	}
	const key_type * lookup(const key_type * key) override
	{
		return oas_htyp_lookup(&oas, key);
	}
	void remove(const key_type * key) override
	{
		oas_htyp_remove(&oas, key);
	}
	void clear(void) override
	{
		oas_htyp_clear(&oas);
	}
	const char * type(void) override
	{
		return "oa_set";
	}
	size_t size(void) override
	{
		return oas_htyp_entries(&oas);
	}
	
	void iter_init(void) override
	{
		iter = OAS_ITER_INIT;
	}
	const key_type * iter_next() override
	{
		return oas_htyp_iterate(&oas, &iter);
	}
	
	private:
	oas_htyp oas;
	oas_iterator iter;
};

class ch_set : public container
{
	public:
	ch_set()
	{
		chs_htyp_create(&chs, hash_key_type, cmp_key_type);
	}
	~ch_set()
	{
		chs_htyp_destroy(&chs);
	}
	bool cmp(void * a, void * b) override
	{
		return (cmp_key_type(a, b) == 0);
	}
	void insert(const key_type * key) override
	{
		chs_htyp_insert(&chs, key);
	}
	const key_type * lookup(const key_type * key) override
	{
		return chs_htyp_lookup(&chs, key);
	}
	void remove(const key_type * key) override
	{
		chs_htyp_remove(&chs, key);
	}
	void clear(void) override
	{
		chs_htyp_clear(&chs);
	}
	const char * type(void) override
	{
		return "ch_set";
	}
	size_t size(void) override
	{
		return chs_htyp_elems(&chs);
	}
	
	void iter_init(void) override
	{
		chs_iterator_init(&iter);
	}
	const key_type * iter_next() override
	{
		return chs_htyp_iterate(&chs, &iter);
	}
	
	private:
	chs_htyp chs;
	chs_iterator iter;
};

class ptr_set : public container
{
	public:
	ptr_set()
	{
		ptrs_htyp_create(&ptrs, hash_key_type_ptrs, cmp_key_type_ptrs);
	}
	~ptr_set()
	{
		ptrs_htyp_destroy(&ptrs);
	}
	bool cmp(void * a, void * b) override
	{
		return (cmp_key_type_ptrs(a, *((const char **)b)) == 0);
	}
	void insert(const key_type * key) override
	{
		ptrs_htyp_insert(&ptrs, (const char **)*key);
	}
	const key_type * lookup(const key_type * key) override
	{
		return ptrs_htyp_lookup(&ptrs, (const char **)*key);
	}
	void remove(const key_type * key) override
	{
		ptrs_htyp_remove(&ptrs, (const char **)*key);
	}
	void clear(void) override
	{
		ptrs_htyp_clear(&ptrs);
	}
	const char * type(void) override
	{
		return "ptr_set";
	}
	size_t size(void) override
	{
		return ptrs_htyp_elems(&ptrs);
	}
	
	void iter_init(void) override
	{
		ptrs_iterator_init(&iter);
	}
	const key_type * iter_next() override
	{
		return ptrs_htyp_iterate(&ptrs, &iter);
	}
	
	private:
	ptrs_htyp ptrs;
	ptrs_iterator iter;
};

double bench_ins(const std::vector<key_type>& keys, container * cont)
{
	const key_type * key = (const key_type *)keys.data();
	
	size_t old_ecount = cont->size();
	
	clock_t begin = clock();
	for (size_t i = 0, end = keys.size(); i < end; ++i)
		cont->insert(key+i);
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
	const key_type * pkey = NULL;
	
	clock_t begin = clock();
	for (size_t i = 0, end = keys.size(); i < end; ++i)
	{
		pkey = cont->lookup(key+i);
		if (pkey && !cont->cmp(pkey, (key+i)))
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
	while ((key = cont->iter_next()))
		++iterated;
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
	std_uset uset;
//    std_set oset;
	oa_set oas;
	ch_set chs;
	ptr_set ptrs;
	
	std::vector<container *> conts;
	conts.push_back(&oas);
	conts.push_back(&chs);
	conts.push_back(&ptrs);
	conts.push_back(&uset);
//	conts.push_back(&oset);
	
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
