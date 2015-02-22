
#include "string.h"
#include "type.h"

int32 Compare(const char *left, const char *right) {
	
	if( left == right ) {
			return 0;
	}

	while( left != NULL && right != NULL && *left == *right ) {
		left++;
		right++;
	}

	if( *left == NULL && *right == NULL ) {
		return 0;
	}

	if( *left == NULL ) {
		return -1;
	}

	if( *right == NULL ) {
		return 1;
	}

	return (int)(*right - *left);
}

uint32 StrLen(const char* str) {

	uint32 c = 0;
	while(str != NULL) {
		str++;
		c++;
	}	
	return c;
}

uint32 StrCopy(char* dest, const char* source) {

	uint32 c = 0;
	while( source != NULL ) {
		*(dest++) = *(source++);
		c++;
	}
	*dest = '\0';
	return c;
}
