/**
 Noekeon Python implementation
 Based on Noekeon specification
*/

#include "noekeon_api.h"
#include <stdio.h>

/* Nr */
#define NUMBER_OF_ROUNDS 16
#define ROTL32(v, n)    (0xFFFFFFFF & (((v) << (n)) | ((v) >> (32 - (n)))))
#define ROTR32(v, n)    (0xFFFFFFFF & (((v) >> (n)) | ((v) << (32 - (n)))))

/* Local Private Functions */
void Round(const uint32_t * const k, uint32_t * const a, uint32_t K1, uint32_t K2);
void Gamma(uint32_t * const a);
void Theta(const uint32_t * const k, uint32_t * const a);
void Pi1(uint32_t * const a);
void Pi2(uint32_t * const a);

void Round(const uint32_t * const k, uint32_t * const a, uint32_t K1, uint32_t K2)
{
	a[0] ^= K1;
	Theta(k, a);
	a[0] ^= K2;
	Pi1(a);
	Gamma(a);
	Pi2(a);
}

void Gamma(uint32_t * const a)
{
	uint32_t tmp;

	/* first non-linear step in gamma */
	a[1] ^= ~a[3] & ~a[2];
	a[0] ^= a[2] & a[1];

	/* linear step in gamma */
	tmp = a[3];
	a[3] = a[0];
	a[0] = tmp;
	a[2] ^= a[0] ^ a[1] ^ a[3];

	/* last non-linear step in gamma */
	a[1] ^= ~a[3] & ~a[2];
	a[0] ^= a[2] & a[1];

}

void Theta(const uint32_t * const k, uint32_t * const a)
{
	uint32_t tmp;

	tmp = a[0] ^ a[2];
	tmp ^= ROTR32(tmp, 8) ^ ROTL32(tmp, 8);
	a[1] ^= tmp;
	a[3] ^= tmp;

	a[0] ^= k[0];
	a[1] ^= k[1];
	a[2] ^= k[2];
	a[3] ^= k[3];

	tmp = a[1] ^ a[3];
	tmp ^= ROTR32(tmp, 8) ^ ROTL32(tmp, 8);
	a[0] ^= tmp;
	a[2] ^= tmp;

}

void Pi1(uint32_t * const a)
{
	a[1] = ROTL32(a[1], 1);
	a[2] = ROTL32(a[2], 5);
	a[3] = ROTL32(a[3], 2);
}

void Pi2(uint32_t * const a)
{
	a[1] = ROTR32(a[1], 1);
	a[2] = ROTR32(a[2], 5);
	a[3] = ROTR32(a[3], 2);
}

/* Workingkey format is 4 elements of 32 bit size.
 * State format is 4 elements of 32 bit size.
 * State corresponds to plainText
 */
void NoekeonEncrypt(const uint32_t * const key, uint32_t * const data)
{
	uint32_t Roundct[] = { 0x80, 0x1B, 0x36, 0x6C,
		0xD8, 0xAB, 0x4D, 0x9A,
		0x2F, 0x5E, 0xBC, 0x63,
		0xC6, 0x97, 0x35, 0x6A, 0xD4
	};
	int i;

	for (i = 0; i < NUMBER_OF_ROUNDS; i++) {
		Round(key, data, Roundct[i], 0);
	}
	data[0] ^= Roundct[NUMBER_OF_ROUNDS];
	Theta(key, data);
}

/* Workingkey format is 4 elements of 32 bit size.
 * State format is 4 elements of 32 bit size.
 * State corresponds to cipherText
 */
void NoekeonDecrypt(const uint32_t * const key, uint32_t * const data)
{
	uint32_t Roundct[] = { 0x80, 0x1B, 0x36, 0x6C,
		0xD8, 0xAB, 0x4D, 0x9A,
		0x2F, 0x5E, 0xBC, 0x63,
		0xC6, 0x97, 0x35, 0x6A, 0xD4
	};
	uint32_t null_vector[] = { 0, 0, 0, 0 };
	uint32_t inv_key[4];
	int i;

	inv_key[0] = key[0];
	inv_key[1] = key[1];
	inv_key[2] = key[2];
	inv_key[3] = key[3];

	Theta(null_vector, inv_key);
	for (i = NUMBER_OF_ROUNDS; i > 0; i--) {
		Round(inv_key, data, 0, Roundct[i]);
	}
	Theta(inv_key, data);
	data[0] ^= Roundct[0];
}

#if 0
void main(void)
{
	int i;
	uint32_t key[] = { 0, 0, 0, 0 };
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
}
#endif
