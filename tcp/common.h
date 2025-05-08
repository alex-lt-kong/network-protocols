#ifndef COMMON_H
#define COMMON_H

#define RW_BUF_SIZE 64
#define MSG_BUF_SIZE 65432
#define PORT 43210


#include <time.h>

static size_t iso_dt_len = sizeof("1970-01-01T00:00:00Z");

char *get_iso_datetime(char *buf) {
    time_t now;
    time(&now);
    strftime(buf, iso_dt_len, "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));
    return buf;
}
#endif // COMMON_H
