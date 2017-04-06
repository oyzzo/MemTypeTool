/**
 * @Author: Miguel
 * @File: Memtype Mock
 */

#include "memtype_api.h"
#include "noekeon_api.h"
#include <stdlib.h>
#include <libusb.h>
#include <string.h>

/* Sleep Time includes */
#ifdef _WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#define Sleep(x) usleep((x)*1000)
#endif				/* _WINDOWS */

/* CONFIGURATION defines */
#define PRINTF              printf
#define PRINT_FUNCTION()    PRINTF("Called -- %s\n", __FUNCTION__)
#define USB_WAIT_10ms()     {Sleep(10);}
#define USB_WAIT_50ms()     {Sleep(50);}

/* Local Vars */
static uint16_t vendor_id = 0x1209;
static uint16_t product_id = 0xa033;
static libusb_device_handle *dev;

/* Local Functions */
static uint16_t memtype_encryptedSize(const credential_t * cred);
static uint16_t memtype_encryptedSize(const credential_t * cred)
{
	uint16_t len = 0;

	len += strlen(cred->user) + 1;
	len += strlen(cred->hop) + 1;
	len += strlen(cred->pass) + 1;
	len += strlen(cred->submit) + 1;
	/* len must be multiple of 16 bytes */
	if ((len % 16) != 0) {
		/* add padding */
		len += 16 - (len % 16);
	}

	return len;
}

/**
 * Computes the size of the buffer needed for credential list encryption
 * Usefull to be used to call malloc function
 * Credential Storage format:
 * [name, offset,[Encrypted: user,hop,pass,submit]
 */
static uint16_t memtype_credBuffSize(credential_t * list, uint16_t size);
static uint16_t memtype_credBuffSize(credential_t * list, uint16_t size)
{
	uint16_t i;
	uint16_t bytes = 0;

	for (i = 0; i < size; i++) {
		bytes += strlen(list->name) + 1;
		bytes += 2;	/* offset size */
		bytes += memtype_encryptedSize(list);
		list++;
	}

	return bytes;
}

/**
 * Computes the key for encryption based on pin
 * @param [in] pin - Pn in string format from "0000" to "9999"
 * @param [out] key - Computed key from the incoming pin
 */
static void memtype_pinToKey(const uint8_t pin[4], uint32_t key[4]);

/** 
 * Computes the hash for the memtype 
 * @param [in] pin - Pin from 0 to 9999
 * @param [out] hash - Computed hash from the incoming pin
 */
static void memtype_pinToHash(uint16_t pin, uint8_t hash[16]);

/**
 * Do USB HID Set Report to send a message to memtype
 * followed by USB HID Get Report to receive the answer
 * @param [in,out] msg - Buffer to be sent and the answer from the device
 * @param [in,out] len - Number of bytes to be sent and number of 
 *                       bytes received
 * @return NO_ERROR - Command succesfully sent, data available in msg buffer
 * @return ERROR - Error occourred
 */
static memtype_ret_t memtype_send(uint8_t * msg, uint8_t * len);

/**
 * Do USB HID Get Report to receive data from memtype
 * @param [out] msg - Buffer to store the answer from the device
 * @param [out] len - Number of bytes received
 * @return NO_ERROR - Command succesfully sent, data available in msg buffer
 * @return ERROR - Error occourred
 */
static memtype_ret_t memtype_receive(uint8_t * msg, uint8_t * len);

static void memtype_pinToKey(const uint8_t pin[4], uint32_t key[4])
{
	memcpy(&key[0], pin, 4);
	memcpy(&key[1], pin, 4);
	memcpy(&key[2], pin, 4);
	memcpy(&key[3], pin, 4);
}

static void memtype_pinToHash(uint16_t pin, uint8_t hash[16])
{
	uint32_t key1[4];
	uint32_t key2[4];
	char pin_str[5] = "0000";

	/* Pin Range check */
	if ((pin < 0) || (pin > 9999)) {
		/* BAD pin */
	} else {
		/* Pin to string */
		sprintf(pin_str, "%04d", pin);
	}

	memtype_pinToKey((uint8_t *) pin_str, key1);
	memtype_pinToKey((uint8_t *) pin_str, key2);

	NoekeonEncrypt(key1, key2);
	memcpy(hash, key2, sizeof(key2));
}

static memtype_ret_t memtype_send(uint8_t * msg, uint8_t * len)
{
	memtype_ret_t ret = NO_ERROR;
	uint16_t reportId = 2;

    /** Usb HID Set Report */
    /** dev_handle, bmRequestType, bmRequest, wValue, wIndex, data, wLength, timeout */
	if (libusb_control_transfer(dev, 0x20, 0x09, 0x0300 | reportId, 0, msg, *len, 5000) != *len) {
		PRINTF("Error USB HID set report\n");
		ret = ERROR;
	}
	/* Active wait to let memtype do his stuff */
	USB_WAIT_10ms();

    /** Usb HID Get Report */
    /** dev_handle, bmRequestType, bmRequest, wValue, wIndex, data, wLength, timeout */
	if (libusb_control_transfer(dev, 0xA0, 0x01, 0x0300 | reportId, 0, msg, *len, 5000) != *len) {
		PRINTF("Error USB HID get report\n");
		ret = ERROR;
	}
	/* Active wait to let memtype do his stuff */
	USB_WAIT_10ms();

	PRINT_FUNCTION();
	return ret;
}

static memtype_ret_t memtype_receive(uint8_t * msg, uint8_t * len)
{
	memtype_ret_t ret = NO_ERROR;
	uint16_t reportId = 2;

    /** Usb HID Get Report */
    /** dev_handle, bmRequestType, bmRequest, wValue, wIndex, data, wLength, timeout */
	if (libusb_control_transfer(dev, 0xA0, 0x01, 0x0300 | reportId, 0, msg, *len, 5000) != *len) {
		PRINTF("Error USB HID get report\n");
	}
	/* WAIT 10ms */
	USB_WAIT_10ms();

	PRINT_FUNCTION();
	return ret;
}

memtype_ret_t Memtype_init(void)
{
	const struct libusb_version *version = libusb_get_version();

	/* Print LibUSB version */
	printf("LibUSB version -- %d.%d.%d.%d %s\n",
	       version->major, version->minor, version->micro, version->nano, version->rc);

	PRINT_FUNCTION();
	return NO_ERROR;
}

memtype_ret_t Memtype_connect(void)
{
	static char usbInit = 0;
	memtype_ret_t ret = NO_ERROR;

	/* Init libusb */
	if (!usbInit) {
		libusb_init(NULL);
		usbInit = 1;
	}

	/* Open device directly */
	dev = libusb_open_device_with_vid_pid(NULL, vendor_id, product_id);
	/* libusb_claim_interface ??? */
	if (dev == NULL) {
		PRINTF("Could not open Memtype\n");
		ret = ERROR;
	}

	PRINT_FUNCTION();
	return ret;
}

memtype_ret_t Memtype_disconnect(void)
{
	memtype_ret_t ret = NO_ERROR;

	/* Close USB connection */
	if (dev != NULL) {
		libusb_close(dev);
	} else {
		PRINTF("Could not close Memtype\n");
		ret = ERROR;
	}

	PRINT_FUNCTION();
	return ret;
}

memtype_ret_t Memtype_info(device_info_t * info)
{
	memtype_ret_t ret;
	uint8_t msg[8] = { 5, 0, 0, 0, 0, 0, 0, 0 };
	uint8_t len = sizeof(msg);

	/* Send CMD */
	ret = memtype_send(msg, &len);

	if ((info != NULL) && (msg[0] == 5)) {
		info->major = msg[1];
		info->minor = msg[2];
		info->patch = msg[3];
		info->credSize = (msg[5] << 8) + msg[4];
	} else {
		ret = ERROR;
	}

	PRINT_FUNCTION();
	return ret;
}

memtype_ret_t Memtype_write(const uint8_t * block, uint16_t len, uint16_t offset)
{
	memtype_ret_t ret;
	uint8_t *buff;
	uint8_t msg[8] = { 3, 0, 0, 0, 0, 0, 0, 0 };
	uint8_t l = sizeof(msg);
	uint8_t i;

	/* Prepare command for offset and size */
	msg[1] = offset & 0xFF;
	msg[2] = (offset >> 8) & 0xFF;
	msg[3] = len & 0xFF;
	msg[4] = (len >> 8) & 0xFF;

	/* Prepare Buffer to dynamic allocated memory */
	buff = (uint8_t *) malloc(len);

	if ((block != NULL) && (buff != NULL) && ((len + offset) < 2048)) {
		/* Send CMD */
		ret = memtype_send(msg, &l);
		memcpy(buff, block, len);

		if (msg[0] == 3) {
			/* Send Pin Hash */
			for (i = 0; i < len; i += 8) {
				l = 8;
				memtype_send(&buff[i], &l);
				USB_WAIT_50ms();
			}
			/* Compare buff and hash to be equal */
			if (memcmp(buff, block, len) != 0) {
				ret = ERROR;
			}

		} else {
			ret = ERROR;
		}

	} else {
		ret = ERROR;
	}

	/* Free dynamic allocated memory */
	free(buff);

	PRINT_FUNCTION();
	return ret;
}

memtype_ret_t Memtype_read(uint8_t * block, uint16_t len, uint16_t offset)
{
	PRINT_FUNCTION();
	return NO_ERROR;
}

memtype_ret_t Memtype_write_pin_hash(const uint8_t hash[16])
{
	memtype_ret_t ret;
	uint8_t buff[16];
	uint8_t msg[8] = { 7, 0, 0, 0, 0, 0, 0, 0 };
	uint8_t len = sizeof(msg);
	uint8_t i;

	if (hash != NULL) {
		/* Send CMD */
		ret = memtype_send(msg, &len);
		memcpy(buff, hash, sizeof(buff));

		if (msg[0] == 7) {
			/* Send Pin Hash */
			for (i = 0; i < sizeof(buff); i += 8) {
				len = 8;
				memtype_send(&buff[i], &len);
				USB_WAIT_50ms();
			}
			/* Compare buff and hash to be equal */
			if (memcmp(buff, hash, sizeof(buff)) != 0) {
				ret = ERROR;
			}

		} else {
			ret = ERROR;
		}

	} else {
		ret = ERROR;
	}
	PRINT_FUNCTION();
	return ret;
}

memtype_ret_t Memtype_read_pin_hash(uint8_t hash[16])
{
	memtype_ret_t ret;
	uint8_t msg[8] = { 8, 0, 0, 0, 0, 0, 0, 0 };
	uint8_t len = sizeof(msg);
	uint8_t i;

	if (hash != NULL) {
		/* Send CMD */
		ret = memtype_send(msg, &len);

		if (msg[0] == 8) {
			/* Read Pin Hash */
			for (i = 0; i < 16; i += 8) {
				len = 8;
				memtype_receive(&hash[i], &len);
			}

		} else {
			ret = ERROR;
		}

	} else {
		ret = ERROR;
	}
	PRINT_FUNCTION();
	return ret;
}

memtype_ret_t Memtype_write_keyboard(const uint8_t layout[128])
{
	memtype_ret_t ret;
	uint8_t buff[128];
	uint8_t msg[8] = { 9, 0, 0, 0, 0, 0, 0, 0 };
	uint8_t len = sizeof(msg);
	uint8_t i;

	if (layout != NULL) {
		/* Send CMD */
		ret = memtype_send(msg, &len);
		memcpy(buff, layout, sizeof(buff));

		if (msg[0] == 9) {
			/* Send Keyboard Layout */
			for (i = 0; i < sizeof(buff); i += 8) {
				len = 8;
				memtype_send(&buff[i], &len);
				USB_WAIT_50ms();
			}
			/* Compare buff and layout to be equal */
			if (memcmp(buff, layout, sizeof(buff)) != 0) {
				ret = ERROR;
			}

		} else {
			ret = ERROR;
		}

	} else {
		ret = ERROR;
	}

	PRINT_FUNCTION();
	return ret;
}

memtype_ret_t Memtype_isLocked(memtype_locked_t * lock)
{
	memtype_ret_t ret;
	uint8_t msg[8] = { 5, 0, 0, 0, 0, 0, 0, 0 };
	uint8_t len = sizeof(msg);

	/* Send CMD */
	ret = memtype_send(msg, &len);

	if ((lock != NULL)) {
		*lock = (msg[1] == 0xF6) ? (LOCKED) : (NOT_LOCKED);
	} else {
		ret = ERROR;
	}

	PRINT_FUNCTION();
	return ret;
}

memtype_ret_t Memtype_encrypt(credential_t * list, uint16_t len, uint8_t * buff, uint16_t size, uint16_t pin)
{
	memtype_ret_t ret = NO_ERROR;
	uint16_t i, j;
	uint16_t enbytes;
	uint8_t *ptr = buff;
	uint8_t *enstart_ptr;
	uint16_t offset = 0;
	uint32_t key[4];
	char pin_str[5];

	/* Pin Range check */
	if ((pin < 0) || (pin > 9999)) {
		ret = ERROR;
	} else {
		/* Pin to string */
		sprintf(pin_str, "%04d", pin);
		PRINTF("%s -- PIN:%s\n", __FUNCTION__, pin_str);

		/* Compute key */
		memtype_pinToKey((uint8_t *) pin_str, key);

		/* Set Buffer to 0 */
		memset(buff, 0, size);

		/* Loop between credentials */
		for (i = 0; i < len; i++) {
			strcpy((char *)ptr, list->name);
			ptr += strlen(list->name) + 1;

			if (i < (len - 1)) {
				/* Compute offset and add it to buffer */
				offset += memtype_credBuffSize(list, 1);
			} else {
				/* Last offset should be equal to 0 */
				offset = 0;
			}
			*ptr++ = offset & 0xFF;
			*ptr++ = (offset >> 8) & 0xFF;

			/* encrypt starts here */
			enstart_ptr = ptr;
			strcpy((char *)ptr, list->user);
			ptr += strlen(list->user) + 1;
			strcpy((char *)ptr, list->hop);
			ptr += strlen(list->hop) + 1;
			strcpy((char *)ptr, list->pass);
			ptr += strlen(list->pass) + 1;
			strcpy((char *)ptr, list->submit);
			ptr += strlen(list->submit) + 1;

			/* Number of Bytes to encrypt */
			enbytes = memtype_encryptedSize(list);

			/* Point to next credential location */
			ptr = enstart_ptr + enbytes;

			/* Encrypt block by block */
			for (j = 0; j < enbytes; j += 16) {
				NoekeonEncrypt(key, (uint32_t *) enstart_ptr);
				enstart_ptr += 16;
			}

			list++;
		}
	}

	PRINT_FUNCTION();
	return ret;
}

memtype_ret_t Memtype_decrypt(credential_t * list, uint16_t len, uint8_t * buff, uint16_t size, uint16_t pin)
{
	PRINT_FUNCTION();
	return NO_ERROR;
}

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

	/* Credential Example */
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
	uint16_t clistsize = memtype_credBuffSize(clist, sizeof(clist) / sizeof(credential_t));

	PRINTF("Buff ClistSize = %d\n", clistsize);
	uint8_t *cred_buff = malloc(clistsize);

	Memtype_encrypt(clist, clen, cred_buff, clistsize, pin);
	Memtype_write(cred_buff, clistsize, 0);
	free(cred_buff);

	/* Read/Write Pin example */
	uint8_t hash_w[16];
	uint8_t hash_r[16];

	Memtype_read_pin_hash(hash_r);
	memtype_pinToHash(pin, hash_w);
	Memtype_write_pin_hash(hash_w);
	if (memcmp(hash_w, hash_r, 16) != 0) {
		PRINTF("Hash Write != Hash Read\n");
	}

	/* Disconnect example */
	Memtype_disconnect();

	return 0;
}
