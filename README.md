# flag.h

A small, single-header C flag parser inspired by Go's flag package, with long flags (--foo) added on top.

## API

```c
void flag_decl_bool(  Flags *flags, bool *val,     const char *short_flag, const char *long_flag, bool default_val,     const char *description);
void flag_decl_str(   Flags *flags, char **val,    const char *short_flag, const char *long_flag, char *default_val,    const char *description);
void flag_decl_double(Flags *flags, double *val,   const char *short_flag, const char *long_flag, double default_val,   const char *description);
void flag_decl_int(   Flags *flags, int64_t *val,  const char *short_flag, const char *long_flag, int64_t default_val,  const char *description);
void flag_decl_uint(  Flags *flags, uint64_t *val, const char *short_flag, const char *long_flag, uint64_t default_val, const char *description);

int flag_parse(Flags *flags, int argc, char **argv);
void flag_free(Flags *flags);

void flag_print_info(Flags *flags, const char *header);

bool flag_seen(Flags *flags, void *user_ptr);
``` 

## USAGE

```c
#define FLAG_IMPLEMENTATION
#include "flag.h"
#include <stdio.h>

int main(int argc, char **argv)
{
    bool verbose;
    char *output;
    int64_t count;

    Flags flags = {0};
    flag_decl_bool(&flags, &verbose, "-v", "--verbose", false,     "Enable verbose output");
    flag_decl_str(&flags,  &output,  "-o", "--output",  "out.txt", "Output file path");
    flag_decl_int(&flags,  &count,   "-n", "--count",   1,         "Repeat count");

    if (flag_parse(&flags, argc - 1, argv + 1) != 0)
    {
        flag_print_info(&flags, "USAGE:");
        flag_free(&flags);
        return 1;
    }

    if (flag_seen(&flags, &output))
    {
        printf("Custom output: %s\n", output);
    }

    flag_free(&flags);
    return 0;
}
```

## Supported Syntax

- Types: `bool`, `str` (`char *`), `int` (`int64_t`), `uint` (`uint64_t`), and `double`
- Long Flags: `--name value` or `--name=value`
- Short Flags: `-o value`, `-o=value`, or inline `-ovalue`
- Grouped Flags: `-vo value`, `-vo=value`, or `-vovalue` (boolean flags in the group get set, and a non-boolean flag at the end takes the value)
- Booleans: `-v` or `--verbose` sets `true` optional `true`/`false` works on standalone boolean flags.
