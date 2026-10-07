#include <sys/types.h>
#include <sys/stat.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <time.h>
#include "longfmt.h"

/* A file whose time is within this many seconds of "now" shows hh:mm. */
#define SIXMONTHS ((365 / 2) * 24 * 60 * 60)

/* Execute-position character: x or -, or s/S (t/T) if 'special' is set. */
static char xchar(mode_t m, mode_t xbit, mode_t special, char on, char off)
{
    if (m & special)
        return (m & xbit) ? on : off;
    return (m & xbit) ? 'x' : '-';
}

void format_mode(mode_t m, char *buf)
{
    char t;

    switch (m & S_IFMT) {
    case S_IFDIR:  t = 'd'; break;
    case S_IFCHR:  t = 'c'; break;
    case S_IFBLK:  t = 'b'; break;
    case S_IFREG:  t = '-'; break;
    case S_IFLNK:  t = 'l'; break;
    case S_IFSOCK: t = 's'; break;
    case S_IFIFO:  t = 'p'; break;
#ifdef S_IFWHT
    case S_IFWHT:  t = 'w'; break;
#endif
    default:       t = '?'; break;
    }
#ifdef S_ARCH1
    if (m & S_ARCH1)
        t = 'a';
#endif
#ifdef S_ARCH2
    if (m & S_ARCH2)
        t = 'A';
#endif
    buf[0] = t;
    buf[1] = (m & S_IRUSR) ? 'r' : '-';
    buf[2] = (m & S_IWUSR) ? 'w' : '-';
    buf[3] = xchar(m, S_IXUSR, S_ISUID, 's', 'S');
    buf[4] = (m & S_IRGRP) ? 'r' : '-';
    buf[5] = (m & S_IWGRP) ? 'w' : '-';
    buf[6] = xchar(m, S_IXGRP, S_ISGID, 's', 'S');
    buf[7] = (m & S_IROTH) ? 'r' : '-';
    buf[8] = (m & S_IWOTH) ? 'w' : '-';
    buf[9] = xchar(m, S_IXOTH, S_ISVTX, 't', 'T');
    buf[10] = '\0';
}

void format_date(time_t t, char *buf, size_t len)
{
    static time_t now;
    struct tm *tm;
    const char *fmt;

    if (now == 0)
        now = time(NULL);
    if (t + SIXMONTHS > now && t < now + SIXMONTHS)
        fmt = "%b %e %H:%M";
    else
        fmt = "%b %e  %Y";
    tm = localtime(&t);
    if (tm == NULL || strftime(buf, len, fmt, tm) == 0)
        snprintf(buf, len, "?");
}

void user_name(uid_t uid, int numeric, char *buf, size_t len)
{
    struct passwd *pw = numeric ? NULL : getpwuid(uid);

    if (pw != NULL)
        snprintf(buf, len, "%s", pw->pw_name);
    else
        snprintf(buf, len, "%u", (unsigned)uid);
}

void group_name(gid_t gid, int numeric, char *buf, size_t len)
{
    struct group *gr = numeric ? NULL : getgrgid(gid);

    if (gr != NULL)
        snprintf(buf, len, "%s", gr->gr_name);
    else
        snprintf(buf, len, "%u", (unsigned)gid);
}
