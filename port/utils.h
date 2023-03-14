#ifndef __UTILS_H
#define __UTILS_H

void my_free(void *p);
char *my_strdup(const char *s);
void my_strncpy(char *d, const char *s, int n);
void my_strncat(char *d, const char *s, int n);
int my_strnicmp( const char *s1, const char *s2, int n);

#endif /* __UTILS_H */