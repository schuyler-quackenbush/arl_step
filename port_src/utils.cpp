#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* utility functions */

void
my_free(void *p)
{
	if (p) {
		free(p);
		p = NULL;
	}
}


/* safe string copy 
 * assumes that d is char array of length n
 */
void 
my_strncpy(char *d, const char *s, int n)
{
	strncpy(d, s, n);
	d[n-1] = 0; /* just in case s is longer than d */
}

/* safe string concatenate 
 * assumes that d is char array of length n
 */
void 
my_strncat(char *d, const char *s, int n)
{
	int i, j;
	for (i=0; i<n; i++) {
		if (d[i] == 0) {
			break;
		}
	}
	j=0;
	for ( ; i<n; i++) {
		d[i] = s[j];
		if (s[j] == 0) {
			break;
		}
		j++;
	}
	d[n-1] = 0; /* just in case s is longer than d */
}

char *my_strdup(const char *s1)
{
	size_t len = strlen(s1);
	char *s2 = (char *)malloc(len+1);
	strcpy(s2, s1);
	return s2;
}

/* portable case-insensitive comparison of up to n characters between two strings */
int my_strnicmp( const char *s1, const char *s2, int n)
{
	int i, status;
	char c1, c2;
	status = 0;
	for (i=0; i<n; i++) {
		if ( (c1 = toupper(s1[i])) == '\0' ||
			(c2 = toupper(s2[i])) == '\0' ) {
				break;
		}
		if ( c1 != c2 ) {
			status = c1 < c2 ? -1 : 1;
			break;
		}
	}
	return status;
}