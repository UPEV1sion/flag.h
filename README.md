# Header-Only Flag Parser

This flag parser is inspired by GO's flag parser

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

bool flag_seen(Flags *flags, const char *short_flag, const char *long_flag);
``` 

## USAGE

```c
Flags flags = {0};

bool help;
char *delim;
double pi;
int64_t the_unswer;
int64_t the_unswer_unsigned;
flag_decl_bool(&flags, &help, "-h", "--help", false, "Display help");
// ...

if(0 != flag_parse(&flags, argc, argv))
{
    flag_print_usage(&flags, "USAGE");
    return 1;
}

if(help)
{
    flag_print_usage(&flags, "USAGE");
    return 0;
}
```
