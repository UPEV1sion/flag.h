#include <stdbool.h>

#define FLAG_IMPLEMENTATION
#include "flag.h"

int main(int argc, char **argv)
{
    Flags flags = {0};

    bool help;
    flag_decl_bool(&flags, &help, "-h", "--help", false, "Display help");

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


    return 0;
}
