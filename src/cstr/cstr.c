#include "cstr.h"

#include <stdlib.h>

static inline void zero_out(cstr * str)
{
	memset(str, 0, sizeof(*str));
}

cstr * cstr_resize(cstr * str, size_t cap)
{
	if (cap > 0)
    {
        char * result = (char *)realloc(str->data, cap+1);
        if (result)
        {
            str->data = result;
            str->cap = cap;
            if (str->len > cap)
            {
				result[cap] = '\0';
                str->len = cap;
            }
            return str;
        }
        else
			zero_out(str);
    }
    return NULL;
}

cstr * cstr_create_cap(cstr * str, size_t cap)
{
	zero_out(str);
	cstr * ret = cstr_resize(str, cap);
	if (ret)
		*str->data = '\0';
	return ret;
}

cstr * cstr_create_str(cstr * str, const char * src, size_t src_len)
{
	cstr * ret = cstr_create_cap(str, src_len);
	if (ret)
	{
		char * data = str->data;
		memcpy(data, src, src_len);
		data[src_len] = '\0';
		str->len = src_len;
	}
	return ret;
}

void cstr_destroy(cstr * str)
{
	free(str->data);
	zero_out(str);
}

cstr * cstr_push(cstr * str, char ch)
{
	size_t len = str->len;
	size_t cap = str->cap;
	
	if (len >= cap)
	{
		if (!cstr_resize(str, str->cap*2))
			return NULL;
	}
	
	str->data[len] = ch;
	str->data[++str->len] = '\0';
	return str;
}

cstr * cstr_insert(cstr * dest, size_t index, const char * src, size_t len)
{
	size_t cstr_len = dest->len;
	size_t new_len = cstr_len + len;
	size_t cap = dest->cap;
	
	if (new_len > cap)
	{
		do {
			cap *= 2;
		} while (new_len > cap);
		
		if (!cstr_resize(dest, cap))
			return NULL;
	}

	char * data = dest->data;
	char * spot = data+index;
	
	if (index < cstr_len)
		memmove(spot+len, spot, cstr_len-index);
	
	memcpy(spot, src, len);
	data[new_len] = '\0';
	dest->len = new_len;
	return dest;
}

cstr * cstr_erase(cstr * str, size_t index, size_t len)
{
	size_t new_len = str->len - len;
	char * spot = str->data+index;

	memmove(spot, spot+len, new_len);
	str->data[new_len] = '\0';
	str->len = new_len;
	return str;
}

cstr * cstr_copy_over(cstr * dest, const char * str, size_t len)
{
	size_t cap = dest->cap;
	
	if (cap < len)
	{
		do {
			cap *= 2;
		} while (cap < len);
		
		if (!cstr_resize(dest, cap))
			return NULL;
	}

	char * data = dest->data;

	memcpy(data, str, len);
	data[len] = '\0';
	dest->len = len;
	return dest;
}
