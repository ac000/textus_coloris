/*
 * Copyright (c) 2021 Andrew Clayton
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

#include <stdio.h>
#include <stdlib.h>

#include "textus_coloris.h"

#define TC_RED                  "\e[38;5;160m"
#define TC_GREEN                "\e[38;5;40m"
#define TC_BLUE                 "\e[38;5;75m"

#define TC_BOLD			"\e[1m"
#define TC_RST			"\e[0m"

/*
 * Define the colours; name & colour code.
 *
 * Include attributes such as bold in here.
 *
 * https://en.wikipedia.org/wiki/ANSI_escape_code#Colors
 */
static const struct tc_coloris colors[] = {
	{ "RED",		TC_RED		},
	{ "GREEN",              TC_GREEN	},
	{ "BLUE",               TC_BLUE		},

	{ "BOLD",		TC_BOLD		},
	{ "RST",                TC_RST		},

	{}
};

extern void bold_test(void);

int main(void)
{
	char *cstr;

	tc_set_colors(colors, TC_COLORIS_MODE_AUTO);

	tc_print(stdout, "This is #RED#RED#RST# text\n");

	cstr = tc_cstring("This is #BLUE#%s#RST# text\n", "BLUE");
	printf("%s", cstr);
	free(cstr);

	bold_test();

	return 0;
}
