#include <inttypes.h>

#define size_t  unsigned int

typedef uint16_t u16_t;
typedef uint8_t u8_t;

/* stubs */

void __aeabi_unwind_cpp_pr0(void)
{

}

void __aeabi_unwind_cpp_pr1(void)
{

}

void __libc_init_array(void)
{

}

/* stdlib functions */

char * strncpy(char *dest, const char *src, size_t n)
{
	size_t i;

	for (i = 0 ; i < n && src[i] != '\0' ; i++)
		dest[i] = src[i];

	for ( ; i < n ; i++)
		dest[i] = '\0';

	return dest;
}

size_t strlen(const char *s)
{
	size_t len = 0;
	char *ptr = (char *) s;

	while (*ptr++ != '\0') {
		len += 1;
	}

	return len;
}

void * memcpy(void *dest, const void *src, size_t n)
{
	unsigned char *d = (unsigned char *) dest;
	unsigned char *s = (unsigned char *) src;
	size_t i;

	for (i = 0 ; i < n ; i++) {
		d[i] = s[i];
	}

	return dest;
}

void * memset(void *s, int c, size_t n)
{
	unsigned char *p = (unsigned char *) s;
	unsigned char ch = (unsigned char) c;
	size_t i;

	for (i = 0 ; i < n ; i++)
		p[i] = ch;

	return s;
}

char * strcat(char *dest, const char *src)
{
	size_t dest_len = strlen(dest);
	size_t i;

	for (i = 0 ; src[i] != '\0' ; i++) {
		dest[dest_len + i] = src[i];
	}

	dest[dest_len + i] = '\0';
	return dest;
}

char * strncat(char *dest, const char *src, size_t n)
{
	size_t dest_len = strlen(dest);
	size_t i;

	for (i = 0 ; i < n && src[i] != '\0' ; i++) {
		dest[dest_len + i] = src[i];
	}

	dest[dest_len + i] = '\0';
	return dest;
}
