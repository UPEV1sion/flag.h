#define FLAG_IMPLEMENTATION
#include "flag.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct {
    const char *name;
    char **args;
    int expected_exit;
    const char *expected_out;
} TestCase;

static int run_worker(int argc, char **argv)
{
    bool a, b, c, run_test;
    char *f;
    int64_t n;
    uint64_t u;
    double d;

    Flags flags = {0};
    flag_decl_bool(&flags,   &a,         "-a", "--alpha",    false,         "Flag A");
    flag_decl_bool(&flags,   &b,         "-b", "--beta",     false,         "Flag B");
    flag_decl_bool(&flags,   &c,         "-c", "--gamma",    false,         "Flag C");
    flag_decl_bool(&flags,   &run_test,  NULL, "--run-test", false,         "run-test (ignored)");
    flag_decl_str(&flags,    &f,         "-f", "--file",     "default.txt", "File path");
    flag_decl_int(&flags,    &n,         "-n", "--num",      -1,            "Signed int");
    flag_decl_uint(&flags,   &u,         "-u", "--unum",     0,             "Unsigned int");
    flag_decl_double(&flags, &d,         "-d", "--dbl",      0.0,           "Double");

    int rc = flag_parse(&flags, argc, argv);
    if (rc != 0)
    {
        flag_free(&flags);
        return rc;
    }

    printf("a=%d b=%d c=%d f=%s n=%lld u=%llu d=%.2f seen_f=%d seen_n=%d\n",
           a, b, c,
           f ? f : "(null)",
           (long long)n,
           (unsigned long long)u,
           d,
           flag_seen(&flags, &f),
           flag_seen(&flags, &n));

    flag_free(&flags);
    return 0;
}

static bool exec_case(const char *self_path, const TestCase *tc)
{
    int pipefd[2];
    if(0 != pipe(pipefd))
    {
        perror("pipe");
        return false;
    }

    pid_t pid = fork();
    if(pid < 0)
    {
        perror("fork");
        close(pipefd[0]);
        close(pipefd[1]);
        return false;
    }

    if(0 == pid)
    {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        dup2(pipefd[1], STDERR_FILENO);
        close(pipefd[1]);

        size_t extra = 0;
        while(tc->args[extra]) extra++;
        char **child_argv = malloc((extra + 3) * sizeof(char *));
        assert(child_argv);
        child_argv[0] = (char *)self_path;
        child_argv[1] = "--run-test";
        for (size_t i = 0; i < extra; i++) child_argv[i + 2] = tc->args[i];
        child_argv[extra + 2] = NULL;

        execv(self_path, child_argv);
        _exit(127);
    }

    close(pipefd[1]);
    char buf[4096];
    size_t total = 0;
    ssize_t r;
    while(total + 1 < sizeof(buf) && (r = read(pipefd[0], buf + total, sizeof(buf) - 1 - total)) > 0)
    {
        total += (size_t)r;
    }

    buf[total] = 0;
    close(pipefd[0]);

    int status = 0;
    waitpid(pid, &status, 0);
    int exit_code = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
    bool ok = (exit_code == tc->expected_exit);
    if (ok && tc->expected_out && strstr(buf, tc->expected_out) == NULL) ok = false;

    if(!ok)
    {
        fprintf(stderr, "[FAIL] %s\n  expected exit=%d, got=%d\n  expected substr: %s\n  output: %s\n",
                        tc->name,
                        tc->expected_exit,
                        exit_code,
                        tc->expected_out ? tc->expected_out : "(none)",
                        buf);
        return false;
    }

    printf("[PASS] %s\n", tc->name);
    return true;
}

int main(int argc, char **argv)
{
    if (argc >= 2 && strcmp(argv[1], "--run-test") == 0)
    {
        return run_worker(argc, argv);
    }

    TestCase cases[] = {
        {
            "defaults",
            (char *[]){ NULL },
            0,
            "a=0 b=0 c=0 f=default.txt n=-1 u=0 d=0.00 seen_f=0 seen_n=0"
        },
        {
            "inline short string",
            (char *[]){ "-ftest", NULL },
            0,
            "a=0 b=0 c=0 f=test n=-1 u=0 d=0.00 seen_f=1 seen_n=0"
        },
        {
            "grouped bools + inline string",
            (char *[]){ "-abcfhello.c", NULL },
            0,
            "a=1 b=1 c=1 f=hello.c n=-1 u=0 d=0.00 seen_f=1 seen_n=0"
        },
        {
            "grouped bools + equals + space",
            (char *[]){ "-abf=out.bin", "-n", "42", "-u10", "-d=3.5", NULL },
            0,
            "a=1 b=1 c=0 f=out.bin n=42 u=10 d=3.50 seen_f=1 seen_n=1"
        },
        {
            "negative int inline",
            (char *[]){ "-n-99", NULL },
            0,
            "a=0 b=0 c=0 f=default.txt n=-99 u=0 d=0.00 seen_f=0 seen_n=1"
        },
        {
            "missing value for inline/space string at end",
            (char *[]){ "-abf", NULL },
            1,
            "requires an argument"
        },
        {
            "invalid int value",
            (char *[]){ "-nabc", NULL },
            1,
            "invalid value"
        },
        {
            "invalid int value",
            (char *[]){ "-u-99", NULL },
            1,
            "invalid value"
        },
    };

    size_t num_cases = sizeof(cases) / sizeof(cases[0]);
    size_t passed = 0;
    for (size_t i = 0; i < num_cases; i++) 
    {
        if (exec_case(argv[0], &cases[i])) passed++;
    }

    printf("\n%zu/%zu tests passed.\n", passed, num_cases);
    return (passed == num_cases) ? 0 : 1;

    return 0;
}
