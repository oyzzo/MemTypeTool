#include "memtype_api.h"
#include "noekeon_api.h"
#include <stdlib.h>
#include <libusb.h>
#include <string.h>

/* CONFIGURATION defines */
#define PRINTF              printf
#define PRINT_FUNCTION()    PRINTF("Called -- %s\n", __FUNCTION__)

int main(void)
{
	/* Just reads libusb version */
	Memtype_init();

	/* Open device USB port */
	Memtype_connect();

	/* IsLocked example: */
	memtype_locked_t lock;

	if (Memtype_isLocked(&lock) == NO_ERROR) {
		PRINTF("Device LOCK status: %s\n", (lock == LOCKED) ? "LOCKED" : "NOT_LOCKED");
	}

	/* Read Info example: */
	device_info_t info;

	if (Memtype_info(&info) == NO_ERROR) {
		PRINTF("Memtype Version: %03d.%03d.%03d CZ:%d\n", info.major, info.minor, info.patch, info.credSize);
	} else {
		PRINTF("Memtype_info err\n");
	}

	/* Keyboard example */
	uint8_t layout[128] =
	    { 0, 0, 0, 0, 0, 0, 0, 0, 42, 43, 40, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 44,
		158, 159, 96, 161, 162, 163, 45, 165, 166, 176, 48, 54, 56, 55, 164, 39, 30, 31, 32, 33, 34, 35, 36, 37,
		38, 183, 182,
		3, 167, 131, 173, 84, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147,
		148, 149, 150,
		151, 152, 153, 154, 155, 156, 157, 101, 109, 102, 175, 184, 47, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
		15, 16, 17, 18,
		19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 100, 67, 103, 97, 0
	};
	if (Memtype_write_keyboard(layout) == NO_ERROR) {
		PRINTF("Memtype Keyboard write\n");
	} else {
		PRINTF("Memtype_write_keyboard err\n");
	}

    /** Write Credential Example */
	uint16_t pin = 0;

	credential_t clist[] = {
		{"Area0x30", "usr", "\t", "pwd", "\n"}
		,
		{"Area0x31", "usr", "\t", "pwd", "\n"}
		,
		{"Area0x32", "usr", "\t", "pwd", "\n"}
		,
		{"Area0x33", "usr", "\t", "pwd", "\n"}
		,
		{"Area0x34", "usr", "\t", "pwd", "\n"}
		,
	};
	uint16_t clen = sizeof(clist) / sizeof(credential_t);
	uint16_t buffsize = Memtype_credBuffSize(clist, sizeof(clist) / sizeof(credential_t));
	uint8_t *cred_buff = malloc(buffsize);

	Memtype_encrypt(clist, clen, cred_buff, buffsize, pin);
	free(cred_buff);

    /** Read Credentials Example */
	buffsize = 2048;
	cred_buff = malloc(buffsize);
	Memtype_read(cred_buff, buffsize, 0);
	clen = Memtype_credLen(cred_buff, buffsize);
	credential_t *list = (credential_t *) malloc(sizeof(credential_t) * clen);

	Memtype_decrypt(list, clen, cred_buff, buffsize, pin);

	int i;

	for (i = 0; i < clen; i++) {
		PRINTF("name:%s user:%s pass:%s\n", list[i].name, list[i].user, list[i].pass);
	}
	free(list);
	free(cred_buff);

	/* Read/Write Pin example */
	uint8_t hash_w[16];
	uint8_t hash_r[16];

	Memtype_read_pin_hash(hash_r);
	Memtype_pinToHash(pin, hash_w);
	Memtype_write_pin_hash(hash_w);
	if (memcmp(hash_w, hash_r, 16) != 0) {
		PRINTF("Hash Write != Hash Read\n");
	}

	/* Disconnect example */
	Memtype_disconnect();

	return 0;
}
