#ifndef __MEMTYPE_API_H__
#define __MEMTYPE_API_H__

#include <stdint.h>
#include <stdio.h>

typedef enum {
	NO_ERROR,
	ERROR
} memtype_ret_t;

typedef enum {
	NOT_LOCKED,
	LOCKED
} memtype_locked_t;

typedef struct credential {
	char *name;
	char *user;
	char *hop;
	char *pass;
	char *submit;
} credential_t;

/** Device Info */
typedef struct device_info {
	uint8_t major;
	uint8_t minor;
	uint8_t patch;
	uint16_t credSize;
} device_info_t;

/** Function Prototype Declaration */
memtype_ret_t Memtype_init(void);
memtype_ret_t Memtype_connect(void);
memtype_ret_t Memtype_disconnect(void);
memtype_ret_t Memtype_info(device_info_t * info);
memtype_ret_t Memtype_write(const uint8_t * block, uint16_t len, uint16_t offset);
memtype_ret_t Memtype_read(uint8_t * block, uint16_t len, uint16_t offset);
memtype_ret_t Memtype_write_pin_hash(const uint8_t hash[16]);
memtype_ret_t Memtype_read_pin_hash(uint8_t hash[16]);
memtype_ret_t Memtype_write_keyboard(const uint8_t layout[128]);

/**
 * Sends info command to check if the device is locked or not
 * If the device is locked it stores LOCKED in lock pointer
 * otherwise it stores NOT_LOCKED
 * @param [out] lock - pointer to store the lock status of memtype
 * @return NO_ERROR - Command succesfully sent, data available in lock pointer
 * @return ERROR - Error occourred
 */
memtype_ret_t Memtype_isLocked(memtype_locked_t * lock);

/**
 * Encrypts a list of credentials and return the encrypted list in buff
 *
 */
memtype_ret_t Memtype_encrypt(credential_t * list, uint16_t len, uint8_t * buff, uint16_t size, uint16_t pin);

/**
 * Decrypts a list of credentials and return the encrypted list in buff
 *
 */
memtype_ret_t Memtype_decrypt(credential_t * list, uint16_t len, uint8_t * buff, uint16_t size, uint16_t pin);

#endif				/* MEMTYPE_API_H */
