#include <stdio.h>
#include <string.h>
#include "test.h"
#include "cstr/cstr.h"

static bool test_cstr_create_destroy_getters(void)
{
	cstr str_, * str = &str_;
	check(str == cstr_create_cap(str, 17));
	check(strcmp(str->data, "") == 0);
	check(str->data);
	check(0 == str->len);
	check(17 == str->cap);
	
	check(cstr_data(str) == str->data);
	check(cstr_len(str) == str->len);
	check(cstr_cap(str) == str->cap);
	check(cstr_is_empty(str));
	
	cstr_destroy(str);
	check(!str->data);
	check(!str->len);
	check(!str->cap);
	
	check(str == cstr_create(str));
	check(str->data);
	check(0 == str->len);
	check(31 == str->cap);
	
	check(cstr_data(str) == str->data);
	check(cstr_len(str) == str->len);
	check(cstr_cap(str) == str->cap);
	check(cstr_is_empty(str));
	
	cstr_destroy(str);
	check(!str->data);
	check(!str->len);
	check(!str->cap);
	
	char bstr[] = "foo bar";
	size_t bstr_len = strlen(bstr);
	
	check(str == cstr_create_str(str, bstr, bstr_len));
	check(strcmp(str->data, bstr) == 0);
	check(str->len == bstr_len);
	check(str->cap == bstr_len);
	check('f' == str->data[0]);
	check('r' == str->data[bstr_len-1]);
	check('\0' == str->data[bstr_len]);
	
	check(cstr_data(str) == str->data);
	check(cstr_len(str) == str->len);
	check(cstr_cap(str) == str->cap);
	check(!cstr_is_empty(str));
	
	check('f' == cstr_get(str, 0));
	check('r' == cstr_get(str, bstr_len-1));
	check('\0' == cstr_get(str, bstr_len));
	
	cstr_destroy(str);
	check(!str->data);
	check(!str->len);
	check(!str->cap);
	
	check(str == cstr_create_str(str, bstr+1, 6));
	check(strcmp(str->data, "oo bar") == 0);
	check(6 == str->len);
	check(6 == str->cap);
	check('o' == cstr_get(str, 0));
	check('r' == cstr_get(str, 5));
	check('\0' == str->data[6]);
	
	cstr_destroy(str);
	check(!str->data);
	check(!str->len);
	check(!str->cap);
	
	return true;
}

static bool test_cstr_resize_shtf_dist_reset(void)
{
	cstr str_, * str = &str_;
	
	char bstr[] = "foo bar";
	size_t bstr_len = strlen(bstr);
	
	check(str == cstr_create_str(str, bstr, bstr_len));
	check(strcmp(str->data, bstr) == 0);
	check(cstr_cap(str) == bstr_len);
	check(cstr_len(str) == bstr_len);
	
	check(!cstr_resize(str, 0));
	check(strcmp(str->data, bstr) == 0);
	check(cstr_cap(str) == bstr_len);
	check(cstr_len(str) == bstr_len);
	
	check(cstr_distance(str, strstr(cstr_data(str), "foo")) == 0);
	check(cstr_distance(str, strstr(cstr_data(str), "oo ")) == 1);
	check(cstr_distance(str, strstr(cstr_data(str), "bar")) == 4);
	
	size_t new_cap = 10;
	check(str == cstr_resize(str, new_cap));
	check(strcmp(str->data, bstr) == 0);
	check(cstr_cap(str) == new_cap);
	check(cstr_len(str) == bstr_len);
	
	check(str == cstr_shrink_to_fit(str));
	check(strcmp(str->data, bstr) == 0);
	check(cstr_cap(str) == bstr_len);
	check(cstr_len(str) == bstr_len);
	
	new_cap = 3;
	check(str == cstr_resize(str, new_cap));
	check(strcmp(str->data, "foo") == 0);
	check(cstr_cap(str) == new_cap);
	check(cstr_len(str) == new_cap);
	
	cstr_reset(str);
	check(strcmp(str->data, "") == 0);
	check(cstr_cap(str) == new_cap);
	check(cstr_len(str) == 0);
	check('\0' == cstr_get(str, 0));
	
	cstr_destroy(str);
	
	return true;
}

static bool test_cstr_push(void)
{
	cstr str_, * str = &str_;
	check(str == cstr_create_cap(str, 1));
	
	check(cstr_cap(str) == 1);
	check(cstr_len(str) == 0);
	
	check(str == cstr_push(str, 'f'));
	check(strcmp(cstr_data(str), "f") == 0);
	check(cstr_cap(str) == 1);
	check(cstr_len(str) == 1);
	
	check(str == cstr_push(str, 'o'));
	check(strcmp(cstr_data(str), "fo") == 0);
	check(cstr_cap(str) == 2);
	check(cstr_len(str) == 2);
	
	check(str == cstr_push(str, 'o'));
	check(strcmp(cstr_data(str), "foo") == 0);
	check(cstr_cap(str) == 4);
	check(cstr_len(str) == 3);
	
	check(str == cstr_push(str, ' '));
	check(strcmp(cstr_data(str), "foo ") == 0);
	check(cstr_cap(str) == 4);
	check(cstr_len(str) == 4);
	
	check(str == cstr_push(str, 'b'));
	check(strcmp(cstr_data(str), "foo b") == 0);
	check(cstr_cap(str) == 8);
	check(cstr_len(str) == 5);
	
	check(str == cstr_push(str, 'a'));
	check(strcmp(cstr_data(str), "foo ba") == 0);
	check(cstr_cap(str) == 8);
	check(cstr_len(str) == 6);
	
	check(str == cstr_push(str, 'r'));
	check(strcmp(cstr_data(str), "foo bar") == 0);
	check(cstr_cap(str) == 8);
	check(cstr_len(str) == 7);
	
	cstr_destroy(str);
	return true;
}

static bool test_cstr_append(void)
{
	cstr str_, * str = &str_;
	check(str == cstr_create_cap(str, 1));
	check(cstr_cap(str) == 1);
	check(cstr_len(str) == 0);
	
	char bstr[] = "The";
	size_t bstr_len = strlen(bstr);
	
	check(str == cstr_append(str, bstr, bstr_len));
	check(strlen(str->data) == str->len);
	check(strcmp(str->data, bstr) == 0);
	check(str->len == bstr_len);
	check(4 == str->cap);
	
	check(str == cstr_append(str, " quick", 6));
	check(strcmp(str->data, "The quick") == 0);
	check(strlen(str->data) == str->len);
	check(9 == str->len);
	check(16 == str->cap);
	
	cstr_destroy(str);
	
	check(str == cstr_create_str(str, bstr, bstr_len));
	check(strcmp(str->data, bstr) == 0);
	check(cstr_cap(str) == bstr_len);
	check(cstr_len(str) == bstr_len);
	
	check(str == cstr_append(str, " quick", 6));
	check(strcmp(str->data, "The quick") == 0);
	check(strlen(str->data) == str->len);
	check(9 == str->len);
	check(12 == str->cap);
	
	check(str == cstr_append(str, " brown fox", strlen(" brown fox")));
	check(strcmp(str->data, "The quick brown fox") == 0);
	check(strlen(str->data) == str->len);
	check(19 == str->len);
	check(24 == str->cap);
	
	check(str == cstr_append(str, " jumps over", strlen(" jumps")));
	check(strcmp(str->data, "The quick brown fox jumps") == 0);
	check(strlen(str->data) == str->len);
	check(25 == str->len);
	check(48 == str->cap);
	
	cstr_destroy(str);
	
	check(str == cstr_create(str));
	check(str == cstr_append(str, bstr, bstr_len));
	check(strlen(str->data) == str->len);
	check(strcmp(str->data, bstr) == 0);
	check(str->len == bstr_len);
	check(31 == str->cap);
	
	check(str == cstr_append(str, " quick", 6));
	check(strcmp(str->data, "The quick") == 0);
	check(strlen(str->data) == str->len);
	check(9 == str->len);
	check(31 == str->cap);
	
	check(str == cstr_append(str, " brown fox", strlen(" brown fox")));
	check(strcmp(str->data, "The quick brown fox") == 0);
	check(strlen(str->data) == str->len);
	check(19 == str->len);
	check(31 == str->cap);
	
	check(str == cstr_append(str, " jumps over", strlen(" jumps")));
	check(strcmp(str->data, "The quick brown fox jumps") == 0);
	check(strlen(str->data) == str->len);
	check(25 == str->len);
	check(31 == str->cap);
	
	cstr_destroy(str);
	
	check(str == cstr_create_cap(str, 1));
	check(cstr_cap(str) == 1);
	check(cstr_len(str) == 0);
	
	check(str == cstr_append(str, "The quick brown fox", 19));
	check(strcmp(str->data, "The quick brown fox") == 0);
	check(strlen(str->data) == str->len);
	check(19 == str->len);
	check(32 == str->cap);
	
	check(str == cstr_append(str, "The quick brown fox", 0));
	check(strcmp(str->data, "The quick brown fox") == 0);
	check(strlen(str->data) == str->len);
	check(19 == str->len);
	check(32 == str->cap);
	
	check(str == cstr_append(str, "", 0));
	check(strcmp(str->data, "The quick brown fox") == 0);
	check(strlen(str->data) == str->len);
	check(19 == str->len);
	check(32 == str->cap);
	
	cstr_destroy(str);
	
	return true;
}

static bool test_cstr_insert(void)
{
	cstr str_, * str = &str_;
	check(str == cstr_create(str));
	check(cstr_cap(str) == 31);
	check(cstr_len(str) == 0);
	check(strcmp(cstr_data(str), "") == 0);
	
	memset(str->data, 'x', str->cap);
	
	check(str == cstr_insert(str, 0, "foo", 3));
	check(cstr_cap(str) == 31);
	check(cstr_len(str) == 3);
	check(strcmp(cstr_data(str), "foo") == 0);
	
	check(str == cstr_insert(str, 1, " ", 1));
	check(cstr_cap(str) == 31);
	check(cstr_len(str) == 4);
	check(strcmp(cstr_data(str), "f oo") == 0);
	
	check(str == cstr_insert(str, 0, " ", 0));
	check(cstr_cap(str) == 31);
	check(cstr_len(str) == 4);
	check(strcmp(cstr_data(str), "f oo") == 0);
	
	check(str == cstr_insert(str, 1, "", 0));
	check(cstr_cap(str) == 31);
	check(cstr_len(str) == 4);
	check(strcmp(cstr_data(str), "f oo") == 0);
	
	check(str == cstr_insert(str, 0, "", 0));
	check(cstr_cap(str) == 31);
	check(cstr_len(str) == 4);
	check(strcmp(cstr_data(str), "f oo") == 0);
	
	check(str == cstr_insert(str, 0, "ba ", 3));
	check(cstr_cap(str) == 31);
	check(cstr_len(str) == 7);
	check(strcmp(cstr_data(str), "ba f oo") == 0);
	
	check(str == cstr_insert(str, 2, "r", 1));
	check(cstr_cap(str) == 31);
	check(cstr_len(str) == 8);
	check(strcmp(cstr_data(str), "bar f oo") == 0);
	
	cstr_destroy(str);
	
	check(str == cstr_create_cap(str, 1));
	check(cstr_cap(str) == 1);
	check(cstr_len(str) == 0);
	check(strcmp(cstr_data(str), "") == 0);
	
	check(str == cstr_insert(str, 0, "foo", 3));
	check(cstr_cap(str) == 4);
	check(cstr_len(str) == 3);
	check(strcmp(cstr_data(str), "foo") == 0);
	
	check(str == cstr_insert(str, 1, " ", 1));
	check(cstr_cap(str) == 4);
	check(cstr_len(str) == 4);
	check(strcmp(cstr_data(str), "f oo") == 0);
	
	check(str == cstr_insert(str, 0, " ", 0));
	check(cstr_cap(str) == 4);
	check(cstr_len(str) == 4);
	check(strcmp(cstr_data(str), "f oo") == 0);
	
	check(str == cstr_insert(str, 1, "", 0));
	check(cstr_cap(str) == 4);
	check(cstr_len(str) == 4);
	check(strcmp(cstr_data(str), "f oo") == 0);
	
	check(str == cstr_insert(str, 0, "", 0));
	check(cstr_cap(str) == 4);
	check(cstr_len(str) == 4);
	check(strcmp(cstr_data(str), "f oo") == 0);
	
	check(str == cstr_insert(str, 0, "ba ", 3));
	check(cstr_cap(str) == 8);
	check(cstr_len(str) == 7);
	check(strcmp(cstr_data(str), "ba f oo") == 0);
	
	check(str == cstr_insert(str, 2, "r", 1));
	check(cstr_cap(str) == 8);
	check(cstr_len(str) == 8);
	check(strcmp(cstr_data(str), "bar f oo") == 0);
	
	cstr_destroy(str);
	
	return true;
}

static bool test_cstr_erase(void)
{
	cstr str_, * str = &str_;
	check(str == cstr_create_str(str, "The quick brown fox jumps over", 25));
	check(cstr_cap(str) == 25);
	check(cstr_len(str) == 25);
	check(strcmp(cstr_data(str), "The quick brown fox jumps") == 0);
	
	check(str == cstr_erase(str, 0, 4));
	check(cstr_cap(str) == 25);
	check(cstr_len(str) == 21);
	check(strcmp(cstr_data(str), "quick brown fox jumps") == 0);
	
	check(str == cstr_erase(str, 5, 6));
	check(cstr_cap(str) == 25);
	check(cstr_len(str) == 15);
	check(strcmp(cstr_data(str), "quick fox jumps") == 0);
	
	check(str == cstr_erase(str, 10, 6));
	check(cstr_cap(str) == 25);
	check(cstr_len(str) == 9);
	check(strcmp(cstr_data(str), "quick fox") == 0);
	
	check(str == cstr_erase(str, 0, 9));
	check(cstr_cap(str) == 25);
	check(cstr_len(str) == 0);
	check(strcmp(cstr_data(str), "") == 0);
	
	cstr_destroy(str);
	
	return true;
}

static bool test_cstr_copy_over(void)
{
	cstr stra_, * stra = &stra_;
	check(stra == cstr_create_str(stra, "foo bar", 7));
	check(cstr_cap(stra) == 7);
	check(cstr_len(stra) == 7);
	check(strcmp(cstr_data(stra), "foo bar") == 0);
	
	cstr strb_, * strb = &strb_;
	check(strb == cstr_create(strb));
	check(cstr_cap(strb) == 31);
	check(cstr_len(strb) == 0);
	check(strcmp(cstr_data(strb), "") == 0);
	
	check(strb == cstr_copy_over(strb, cstr_data(stra), cstr_len(stra)));
	check(cstr_cap(strb) == 31);
	check(cstr_len(strb) == 7);
	check(strcmp(cstr_data(strb), "foo bar") == 0);
	cstr_destroy(strb);
	
	check(strb == cstr_create_cap(strb, 1));
	check(cstr_cap(strb) == 1);
	check(cstr_len(strb) == 0);
	check(strcmp(cstr_data(strb), "") == 0);
	
	check(strb == cstr_copy_over(strb, cstr_data(stra), cstr_len(stra)));
	check(cstr_cap(strb) == 8);
	check(cstr_len(strb) == 7);
	check(strcmp(cstr_data(strb), "foo bar") == 0);
	cstr_destroy(strb);
	
	check(strb == cstr_create_str(strb, "baz", 3));
	check(cstr_cap(strb) == 3);
	check(cstr_len(strb) == 3);
	check(strcmp(cstr_data(strb), "baz") == 0);
	
	check(strb == cstr_copy_over(strb, cstr_data(stra), cstr_len(stra)));
	check(cstr_cap(strb) == 12);
	check(cstr_len(strb) == 7);
	check(strcmp(cstr_data(strb), "foo bar") == 0);
	cstr_destroy(strb);
	
	check(strb == cstr_create_str(strb, "baz zig zag", 11));
	check(cstr_cap(strb) == 11);
	check(cstr_len(strb) == 11);
	check(strcmp(cstr_data(strb), "baz zig zag") == 0);
	
	check(strb == cstr_copy_over(strb, cstr_data(stra), cstr_len(stra)));
	check(cstr_cap(strb) == 11);
	check(cstr_len(strb) == 7);
	check(strcmp(cstr_data(strb), "foo bar") == 0);
	cstr_destroy(strb);
	
	cstr_destroy(stra);
	
	return true;
}

static bool test_cstr_substr(void)
{
	cstr stra_, * stra = &stra_;
	check(stra == cstr_create_str(stra, "The quick brown", 15));
	check(cstr_cap(stra) == 15);
	check(cstr_len(stra) == 15);
	check(strcmp(cstr_data(stra), "The quick brown") == 0);
	
	cstr strb_, * strb = &strb_;
	check(strb == cstr_create(strb));
	check(cstr_cap(strb) == 31);
	check(cstr_len(strb) == 0);
	check(strcmp(cstr_data(strb), "") == 0);
	
	check(strb == cstr_substr(strb, cstr_data(stra), 0, 3));
	check(cstr_cap(strb) == 31);
	check(cstr_len(strb) == 3);
	check(strcmp(cstr_data(strb), "The") == 0);
	cstr_destroy(strb);
	
	check(strb == cstr_create_cap(strb, 1));
	check(cstr_cap(strb) == 1);
	check(cstr_len(strb) == 0);
	check(strcmp(cstr_data(strb), "") == 0);
	
	check(strb == cstr_substr(strb, cstr_data(stra), 4, 5));
	check(cstr_cap(strb) == 8);
	check(cstr_len(strb) == 5);
	check(strcmp(cstr_data(strb), "quick") == 0);
	cstr_destroy(strb);
	
	check(strb == cstr_create_str(strb, "foo", 3));
	check(cstr_cap(strb) == 3);
	check(cstr_len(strb) == 3);
	check(strcmp(cstr_data(strb), "foo") == 0);
	
	check(strb == cstr_substr(strb, cstr_data(stra), 4, 5));
	check(cstr_cap(strb) == 6);
	check(cstr_len(strb) == 5);
	check(strcmp(cstr_data(strb), "quick") == 0);
	cstr_destroy(strb);
	
	check(strb == cstr_create_str(strb, "foo bar", 7));
	check(cstr_cap(strb) == 7);
	check(cstr_len(strb) == 7);
	check(strcmp(cstr_data(strb), "foo bar") == 0);
	
	check(strb == cstr_substr(strb, cstr_data(stra), 10, 5));
	check(cstr_cap(strb) == 7);
	check(cstr_len(strb) == 5);
	check(strcmp(cstr_data(strb), "brown") == 0);
	cstr_destroy(strb);
	
	check(strb == cstr_create_str(strb, "foo bar", 7));
	check(cstr_cap(strb) == 7);
	check(cstr_len(strb) == 7);
	check(strcmp(cstr_data(strb), "foo bar") == 0);
	
	check(strb == cstr_substr(
		strb,
		cstr_data(stra),
		cstr_distance(stra, strstr(cstr_data(stra), "quick brown")),
		strlen("quick brown"))
	);
	check(cstr_cap(strb) == 14);
	check(cstr_len(strb) == strlen("quick brown"));
	check(strcmp(cstr_data(strb), "quick brown") == 0);
	cstr_destroy(strb);
	
	cstr_destroy(stra);
	return true;
}

static bool test_cstr_is_eq(void)
{
	cstr stra_, * stra = &stra_;
	check(stra == cstr_create_cap(stra, 1));
	check(cstr_cap(stra) == 1);
	check(cstr_len(stra) == 0);
	check(strcmp(cstr_data(stra), "") == 0);
	
	cstr strb_, * strb = &strb_;
	check(strb == cstr_create_cap(strb, 1));
	check(cstr_cap(strb) == 1);
	check(cstr_len(strb) == 0);
	check(strcmp(cstr_data(strb), "") == 0);
	
	check(cstr_is_eq(stra, strb));
	
	check(stra == cstr_push(stra, 'a'));
	check(cstr_len(stra) == 1);
	check(strcmp(cstr_data(stra), "a") == 0);
	check(!cstr_is_eq(stra, strb));
	
	check(strb == cstr_push(strb, 'b'));
	check(cstr_len(strb) == 1);
	check(strcmp(cstr_data(strb), "b") == 0);
	check(!cstr_is_eq(stra, strb));
	
	strb->data[0] = 'a';
	check(cstr_len(strb) == 1);
	check(strcmp(cstr_data(strb), "a") == 0);
	check(cstr_is_eq(stra, strb));
	
	check(strb == cstr_push(strb, 'b'));
	check(cstr_len(strb) == 2);
	check(strcmp(cstr_data(strb), "ab") == 0);
	check(!cstr_is_eq(stra, strb));
	
	check(stra == cstr_push(stra, 'b'));
	check(cstr_len(stra) == 2);
	check(strcmp(cstr_data(stra), "ab") == 0);
	check(cstr_is_eq(stra, strb));
	
	cstr_destroy(strb);
	cstr_destroy(stra);
	
	check(stra == cstr_create_str(stra, "foo bar", 7));
	check(strb == cstr_create_str(strb, "foo baz", 7));
	check(!cstr_is_eq(stra, strb));
	cstr_destroy(strb);
	cstr_destroy(stra);
	
	check(stra == cstr_create_str(stra, "foo bar", 6));
	check(strb == cstr_create_str(strb, "foo baz", 6));
	check(cstr_is_eq(stra, strb));
	cstr_destroy(strb);
	cstr_destroy(stra);
	
	check(stra == cstr_create_str(stra, "foo bar", 7));
	check(strb == cstr_create_str(strb, "foo baz", 6));
	check(!cstr_is_eq(stra, strb));
	cstr_destroy(strb);
	cstr_destroy(stra);
	
	check(stra == cstr_create_str(stra, "foo bar", 7));
	check(strb == cstr_create_str(strb, "zig zag", 7));
	check(!cstr_is_eq(stra, strb));
	cstr_destroy(strb);
	cstr_destroy(stra);
	
	check(stra == cstr_create_str(stra, "foo bar", 6));
	check(strb == cstr_create_str(strb, "zig zag", 7));
	check(!cstr_is_eq(stra, strb));
	cstr_destroy(strb);
	cstr_destroy(stra);
	
	return true;
}

static ftest tests[] = {
	test_cstr_create_destroy_getters,
	test_cstr_resize_shtf_dist_reset,
	test_cstr_push,
	test_cstr_append,
	test_cstr_insert,
	test_cstr_erase,
	test_cstr_copy_over,
	test_cstr_substr,
	test_cstr_is_eq,
};

//------------------------------------------------------------------------------

int run_tests(void)
{
    int i, end = sizeof(tests)/sizeof(*tests);

    int passed = 0;
    for (i = 0; i < end; ++i)
        if (tests[i]())
            ++passed;

    if (passed != end)
        putchar('\n');

    int failed = end - passed;
    report(passed, failed);
    return failed;
}
//------------------------------------------------------------------------------

int main(void)
{
	return run_tests();
}
//------------------------------------------------------------------------------
