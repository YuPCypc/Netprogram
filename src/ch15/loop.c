#include "lib/common.h"

char rot13(char c)
{
    if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M'))
        return c + 13;
    else if ((c >= 'N' && c <= 'Z') || (c >= 'n' && c <= 'z'))
        return c - 13;
    return c;
}

void loop_echo(int fd)
{
    char buf[MAX_LINE];
    int n;

    while (1)
    {
        n = read(fd, buf, MAX_LINE);
        if (n < 0)
        {
            error(1, errno, "read error");
        }
        else if (n == 0)
        {
            break;
        }
        for (int i = 0; i < n; i++)
        {
            buf[i] = rot13(buf[i]);
        }
        int res = write(fd, buf, n);
        if (res < 0)
        {
            error(1, errno, "write error");
        }
    }
}