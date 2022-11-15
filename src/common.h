#ifndef COMMON_H
#define COMMON_H

struct InternStr {
	size_t length;
	char *string;
};

char *intern_str_range(const char *start, const char *end);
char *intern_str(const char *str);
#endif
