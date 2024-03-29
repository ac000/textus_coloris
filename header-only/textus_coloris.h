/*
 * textus_coloris.h - colourised console output
 */

/*
 * Copyright (c) 2021-2022 Andrew Clayton
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef _TEXTUS_COLORIS_H_
#define _TEXTUS_COLORIS_H_

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

enum tc_coloris_mode {
	TC_COLORIS_MODE_OFF = 0,
	TC_COLORIS_MODE_ON,
	TC_COLORIS_MODE_AUTO,
};

struct tc_coloris {
	const char *color;
	const char *code;
};

#pragma GCC visibility push(default)

extern char *tc_cstringv(const char *fmt, va_list args)
	__attribute__((format(printf, 1, 0)));
extern char *tc_cstring(const char *fmt, ...)
	__attribute__((format(printf, 1, 2)));
extern int tc_printv(FILE *fp, const char *fmt, va_list args)
	 __attribute__((format(printf, 2, 0)));
extern int tc_print(FILE *fp, const char *fmt, ...)
	__attribute__((format(printf, 2, 3)));
extern void tc_set_colors(const struct tc_coloris *colors,
			  enum tc_coloris_mode mode);

#pragma GCC visibility pop

#ifdef TEXTUS_COLORIS_IMPL

static __thread bool USE_COLOR;
static __thread const struct tc_coloris *tc_colors;

static const char *lookup(const char *color)
{
	const struct tc_coloris *ptr = tc_colors;

	for ( ; ptr->color != NULL; ptr++) {
		if (strcmp(color, ptr->color) == 0)
			return ptr->code;
	}

	return NULL;
}

#define ALLOC_SZ	64
static void srealloc(char **base, size_t extra, char **ptr, size_t *alloc,
		     char **sptr)
{
	void *ret;
	ptrdiff_t len = *ptr - *base;
	ptrdiff_t slen = *sptr - *base;

	if (len + extra < *alloc)
		return;

	ret = realloc(*base, *alloc + ALLOC_SZ);
	if (!ret) {
		free(*base);
		*base = NULL;
		return;
	}
	*base = ret;

	*alloc += ALLOC_SZ;

	*ptr = *base + len;
	*sptr = *base + slen;

	return;
}

#define MAX_COLOR_NAME		32
static char *parser(const char *buf)
{
	char *p = malloc(ALLOC_SZ);
	char *ptr = p;
	char *sptr = p;
	char color[MAX_COLOR_NAME];
	char *cptr = color;
	bool in_color = false;
	size_t alloc = ALLOC_SZ;

	while (*buf) {
		const char *code = "\0";

		*ptr = *buf;

		if (*buf == '#' && !in_color) {
			sptr = ptr;
			cptr = color;
			in_color = true;
			goto next;
		} else if (in_color && *buf != '#') {
			*cptr = *buf;
			cptr++;
			goto next;
		} else if (!in_color) {
			goto next;
		}

		/*
		 * in_color == true and we have a trailing '#', see if
		 * it's a colour code.
		 */
		in_color = false;
		*cptr = '\0';
		if (USE_COLOR)
			code = lookup(color);

		if (code && *code == '\0') {
			ptr = sptr - 1;
		} else if (code) {
			size_t clen = strlen(code);

			ptr = sptr;
			srealloc(&p, clen, &ptr, &alloc, &sptr);
			if (!p)
				return NULL;
			memcpy(ptr, code, clen);
			ptr += clen - 1;
		} else {
			/*
			 * Handle cases like
			 *
			 *   #XXXX##text...
			 *
			 * Note the extra '#' which should appear in the
			 * output string.
			 */
			buf--;
			ptr--;
		}

next:
		srealloc(&p, 1, &ptr, &alloc, &sptr);
		if (!p)
			return NULL;
		buf++;
		ptr++;
	}
	*ptr = '\0';

	return p;
}

static char *cstring(const char *fmt, va_list args)
{
	va_list args2;
	char *buf = NULL;
	char *cstr = NULL;
	size_t buf_sz = 0;
	int len;

	va_copy(args2, args);
	len = vsnprintf(buf, buf_sz, fmt, args);
	if (len < 0)
		goto out_vargs;

	buf_sz = (size_t)len + 1;
	buf = malloc(buf_sz);
	if (!buf)
		goto out_vargs;

	len = vsnprintf(buf, buf_sz, fmt, args2);
	if (len < 0)
		goto out_vargs;

	cstr = parser(buf);

out_vargs:
	free(buf);
	va_end(args2);

	return cstr;
}

/*
 * Colourise a string.
 *
 * Returns a pointer to a colourised version of the given
 * string. User should free(2) the returned pointer.
 */
char *tc_cstringv(const char *fmt, va_list args)
{
	return cstring(fmt, args);
}

/*
 * Colourise a string.
 *
 * Returns a pointer to a colourised version of the given
 * string. User should free(2) the returned pointer.
 */
char *tc_cstring(const char *fmt, ...)
{
	va_list args;
	char *cstr;

	va_start(args, fmt);
	cstr = cstring(fmt, args);
	va_end(args);

	return cstr;
}

/*
 * Print a colourised version of the given string to the
 * specified output stream.
 *
 * Like the v*printf() functions.
 */
int tc_printv(FILE *fp, const char *fmt, va_list args)
{
	char *cstr;
	int len;

	cstr = cstring(fmt, args);
	if (!cstr)
		return -1;

	len = fprintf(fp, "%s", cstr);
	free(cstr);

	return len;
}

/*
 * Print a colourised version of the given string to the
 * specified output stream.
 */
int tc_print(FILE *fp, const char *fmt, ...)
{
	va_list args;
	char *cstr;
	int len;

	va_start(args, fmt);
	cstr = cstring(fmt, args);
	va_end(args);
	if (!cstr)
		return -1;

	len = fprintf(fp, "%s", cstr);
	free(cstr);
	fflush(stdout);

	return len;
}

/*
 * Set the colour map.
 */
void tc_set_colors(const struct tc_coloris *colors, enum tc_coloris_mode mode)
{
	switch (mode) {
	case TC_COLORIS_MODE_OFF:
		USE_COLOR = false;
		break;
	case TC_COLORIS_MODE_ON:
		USE_COLOR = true;
		break;
	case TC_COLORIS_MODE_AUTO:
		if (getenv("NO_COLOR"))
			USE_COLOR = false;
		else
			USE_COLOR = true;
	}

	tc_colors = colors;
}

#endif /* TEXTUS_COLORIS_IMPL */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _TEXTUS_COLORIS_H_ */
