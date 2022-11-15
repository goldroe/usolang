#include "common.h"

#include "stb_ds.h"

InternStr *interns;

char *intern_str_range(const char *start, const char *end) {
	size_t length = end - start;
    char *str = NULL;
    arrsetlen(interns, length+1);
	for (int i = 0; i < length; i++) {
		str[i] = *start++;
	}
	str[length] = 0;
	
	for (int i = 0; i < arrlen(interns); i++) {
        InternStr intern = interns[i];
		size_t cmp_len = (length > intern.length) ? length : intern.length;
		if (strncmp(str, intern.string, cmp_len) == 0) {
			return intern.string;
		}
	}
	
	InternStr intern;
	intern.string = str;
	intern.length = length;
	arrpush(interns, intern);
	return intern.string;
}

char *intern_str(const char *str) {
    return intern_str_range(str, str + strlen(str));
}
