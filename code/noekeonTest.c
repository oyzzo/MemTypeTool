#include "noekeon_api.h"
#include <stdio.h>

int main(void)
{
	int i;
	uint32_t key[] = { 0, 1, 5, 6 };
	uint32_t txt[] = { 1, 2, 3, 4 };

	printf("Before: ");
	for (i = 0; i < 4; i++) {
		printf("%08X ", txt[i]);
	}
	NoekeonEncrypt(key, txt);
	printf("\nEncrypted: ");
	for (i = 0; i < 4; i++) {
		printf("%08X ", txt[i]);
	}

	NoekeonDecrypt(key, txt);
	printf("\nDecrypted: ");
	for (i = 0; i < 4; i++) {
		printf("%08X ", txt[i]);
	}
	printf("\n");
	return 0;
}
