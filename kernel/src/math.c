
#include <type.h>

uint32 __aeabi_uidiv(uint32 a, uint32 b) {

	if( b == 0 ) {
		return 0;
	}

	uint32 n = 0;
	while( a > b ) {
		n++;
		a -= b;
	}
	return n;
}


uint32 __aeabi_uidivmod(uint32 a, uint32 b) {

	if( b == 0 ) {
		return 0;
	}

	while( a > b ) {
		a -= b;
	}
	return a;
}

uint64 __aeabi_uldivmod(uint64 a, uint64 b) {

	if( b == 0 ) {
		return 0;
	}

	while( a > b ) {
		a -= b;
	}
	return a;
}
