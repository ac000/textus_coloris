# Textus Coloris

Simple "library" for doing colourised console output.

# API

```C
enum tc_coloris_mode {
        TC_COLORIS_MODE_OFF,
        TC_COLORIS_MODE_ON,
        TC_COLORIS_MODE_AUTO,
};
```

There are five functions

```C
void tc_set_colors(const struct tc_coloris *colors, enum tc_coloris_mode mode);
```

This is to set the colour map. *mode* can be one of *enum tc_coloris_mode*
which produces the following behaviour

```C
TC_COLORIS_MODE_OFF
```

Forces colour output off..

```C
TC_COLORIS_MODE_ON
```

Forces colour output on, regardless of the [NO\_COLOR](#nocolor) setting.

```C
TC_COLORIS_MODE_AUTO
```

Obey the NO\_COLOR environment variable.


```C
int tc_print(FILE *fp, const char *fmt, ...);
int tc_printv(FILE *fp, const char *fmt, va_list args);
```

These print a colourised output to the given output stream. These are
analogous to the fprintf(3) & vfprintf(3) function.

```C
char *tc_cstring(const char *fmt, ...);
char *tc_cstringv(const char *fmt, va_list args);
```

These return a pointer to a colourised string. You should free(2) this
pointer, NULL will be returned on error. These are sort of analogous to the
asprintf(3) & vasprintf(3) functions.

# Use

Seeing as this is really a bit too simple to make into an actual _DSO_, it is
instead presented as a header-only library.

This is contained under _header-only/_.

There is a simple test program to show basic usage. Essentially just copy
_textus\_coloris.h_ into your project and

```C
#define TEXTUS_COLORIS_IMPL
#include "textus_coloris.h"
```

in **one** of your .c files. If you want to use these functions from any other
source files then just do

```C
#include "textus_coloris.h"
```

in them.

Define a colour map

```C
static const struct tc_coloris colors[] = {
        { "RED",                "\e[38;5;160m" },
        { "GREEN",              "\e[38;5;40m"  },
        { "BLUE",               "\e[38;5;75m"  },

        { "BOLD",               "\e[1m"        },
        { "RST",                "\e[0m"        },

        {}
};
```

then set it with

```C
tc_set_colors(colors, TC_COLORIS_MODE_AUTO);
```

Then you can do stuff like

```C
tc_print(stdout, "Hello World! #BOLD##GREEN#%s#RST#\n", "Hello World!");
```

## split-out

There is also a version with the core code split out into a _.c_ file. You
can just copy _textus\_coloris.[ch]_ into your project and build the .c file
as you do the rest. Then it's like the above but you don't
_#define TEXTUS\_COLORIS\_IMPL_

Otherwise the functionality is the same.

## Examples

There are examples of usage under _header-only/_ & _split-out/_

# NO_COLOR

This obeys the [NO\_COLOR](https://no-color.org/) environment variable.

# Thread safety

This should be thread safe, the colour map pointer is stored in thread local
storage so you should be able to set per thread colour maps.

# License

This licensed under under the MIT license.

See *LICENSE* in the repository root for details.

# Contributing

See *CodingStyle.md* & *Contributing.md*
