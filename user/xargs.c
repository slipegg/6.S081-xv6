#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h" // MAXARG

#define is_blank(chr) (chr == ' ' || chr == '\t')

int main(int argc, char *argv[])
{
    char buf[2048], ch;
    char *p = buf;
    char *word[MAXARG];
    int cnt;
    int blanks = 0;
    int offset = 0;

    if (argc <= 1)
    {
        fprintf(2, "usage: xargs <command> [argv...]\n");
        exit(1);
    }

    for (cnt = 1; cnt < argc; cnt++)
    {
        word[cnt - 1] = argv[cnt];
    }
    --cnt;

    while (read(0, &ch, 1) > 0)
    {
        if (is_blank(ch))
        {
            blanks++;
            continue;
        }

        if (blanks)
        { // 之前有过空格
            buf[offset++] = 0;

            word[cnt++] = p;
            p = buf + offset;

            blanks = 0;
        }

        if (ch != '\n')
        {
            buf[offset++] = ch;
        }
        else
        {
            word[cnt++] = p;
            p = buf + offset;

            if (!fork())
            {
                exit(exec(word[0], word));
            }
            wait(0);

            cnt = argc - 1;
        }
    }

    exit(0);
}