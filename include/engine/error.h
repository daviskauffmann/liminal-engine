#ifndef ERROR_H
#define ERROR_H

void error_set(const char *fmt, ...);
const char *error_get(void);

#endif
