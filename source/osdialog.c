/* Written by Andrew Belt
 * https://github.com/AndrewBelt/osdialog
 * Distributed under the Creative Commons CC0 Public Domain license
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "osdialog.h"

#if defined _MSC_VER
# if _MSC_VER < 1900
#   include "c99_snprintf.h"
# endif
#endif

#if defined FORTIFY
# include <alloc/fortify.h>
#endif


char* osdialog_strdup(const char* s) {
	return osdialog_strndup(s, strlen(s));
}

char* osdialog_strndup(const char* s, size_t n) {
	char* d = OSDIALOG_MALLOC(n + 1);
	memcpy(d, s, n);
	d[n] = '\0';
	return d;
}

osdialog_filters* osdialog_filters_parse(const char* str) {
	osdialog_filters* filters_head = OSDIALOG_MALLOC(sizeof(osdialog_filters));
	filters_head->next = NULL;

	osdialog_filters* filters = filters_head;
	osdialog_filter_patterns* patterns = NULL;

	const char* text = str;
	while (1) {
		switch (*str) {
			case ':': {
				filters->name = osdialog_strndup(text, str - text);
				filters->patterns = OSDIALOG_MALLOC(sizeof(osdialog_filter_patterns));
				patterns = filters->patterns;
				patterns->next = NULL;
				text = str + 1;
			} break;
			case ',': {
				assert(patterns);
				patterns->pattern = osdialog_strndup(text, str - text);
				patterns->next = OSDIALOG_MALLOC(sizeof(osdialog_filter_patterns));
				patterns = patterns->next;
				patterns->next = NULL;
				text = str + 1;
			} break;
			case ';': {
				assert(patterns);
				patterns->pattern = osdialog_strndup(text, str - text);
				filters->next = OSDIALOG_MALLOC(sizeof(osdialog_filters));
				filters = filters->next;
				filters->next = NULL;
				patterns = NULL;
				text = str + 1;
			} break;
			case '\0': {
				assert(patterns);
				patterns->pattern = osdialog_strndup(text, str - text);
			} break;
			default: break;
		}
		if (!*str)
			break;
		str++;
	}

	return filters_head;
}

void osdialog_filter_patterns_free(osdialog_filter_patterns* patterns) {
	if (!patterns)
		return;
	OSDIALOG_FREE(patterns->pattern);
	osdialog_filter_patterns* next = patterns->next;
	OSDIALOG_FREE(patterns);
	osdialog_filter_patterns_free(next);
}

void osdialog_filters_free(osdialog_filters* filters) {
	if (!filters)
		return;
	OSDIALOG_FREE(filters->name);
	osdialog_filter_patterns_free(filters->patterns);
	osdialog_filters* next = filters->next;
	OSDIALOG_FREE(filters);
	osdialog_filters_free(next);
}
