#ifndef __MEMTYPE_API_H__
#define __MEMTYPE_API_H__

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

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
	} memtype_credential_t;

/** Device Info */
	typedef struct device_info {
		uint8_t major;
		uint8_t minor;
		uint8_t patch;
		uint16_t credSize;
	} memtype_info_t;

/** Function Prototype Declaration */
	memtype_ret_t Memtype_init(void);
	memtype_ret_t Memtype_connect(void);
	memtype_ret_t Memtype_disconnect(void);
	memtype_ret_t Memtype_info(memtype_info_t * info);
	memtype_ret_t Memtype_write(const uint8_t * block, uint16_t len, uint16_t offset);
	memtype_ret_t Memtype_read(uint8_t * block, uint16_t len, uint16_t offset);
	memtype_ret_t Memtype_write_pin_hash(const uint8_t hash[16]);
	memtype_ret_t Memtype_read_pin_hash(uint8_t hash[16]);
	memtype_ret_t Memtype_write_keyboard(const uint8_t layout[128]);
    memtype_ret_t memtype_send(uint8_t * msg, uint8_t * len);
    memtype_ret_t memtype_receive(uint8_t * msg, uint8_t * len);

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
	memtype_ret_t Memtype_encrypt(memtype_credential_t * list, uint16_t len, uint8_t * buff, uint16_t size, uint16_t pin);

/**
 * Decrypts a list of credentials and return the encrypted list in buff
 *
 */
	memtype_ret_t Memtype_decrypt(memtype_credential_t * list, uint16_t len, uint8_t * buff, uint16_t size, uint16_t pin);

/**
 * Computes the size of the buffer needed for credential list encryption
 * Usefull to be used within malloc function
 * Credential Storage format:
 * [name, offset,[Encrypted: user,hop,pass,submit]
 * @param [in] list - list of credentials
 * @param [in] len - number of credentials 
 * @return size of the buffer needed to encrypt the list of credentials
 */
	uint16_t Memtype_credBuffSize(memtype_credential_t * list, uint16_t len);

/**
 * Computes the number of credentials in a encrypted buffer
 * Usefull to be used within malloc function
 * [name, offset,[Encrypted: user,hop,pass,submit]
 * @param [in] buff - Encrypted buffer
 * @param [in] size - Buffer size
 * @return The number of credentials in the buffer
 */
	uint16_t Memtype_credLen(uint8_t * buff, uint16_t size);

/** 
 * Computes the hash for the memtype 
 * @param [in] pin - Pin from 0 to 9999
 * @param [out] hash - Computed hash from the incoming pin
 */
	void Memtype_pinToHash(uint16_t pin, uint8_t hash[16]);
#ifdef __cplusplus
}
#endif
#endif				/* MEMTYPE_API_H */
