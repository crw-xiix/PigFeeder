#pragma once
#include "pch.h"
#include "crc.h"


#define WIDTH  (8 * sizeof(uint8_t))
#define TOPBIT (1 << (WIDTH - 1))
#define POLYNOMIAL 0xD8
#define crc uint8_t

uint8_t crcSlow(uint8_t const *message, int nBytes) {
	crc  remainder = 0;
	/*
	 * Perform modulo-2 division, a byte at a time.
	 */
	for (int byte = 0; byte < nBytes; ++byte)
	{
		/*
		 * Bring the next byte into the remainder.
		 */
		remainder ^= (message[byte] << (WIDTH - 8));

		/*
		 * Perform modulo-2 division, a bit at a time.
		 */
		for (uint8_t bit = 8; bit > 0; --bit)
		{
			/*
			 * Try to divide the current data bit.
			 */
			if (remainder & TOPBIT)
			{
				remainder = (remainder << 1) ^ POLYNOMIAL;
			}
			else
			{
				remainder = (remainder << 1);
			}
		}
	}

	/*
	 * The final remainder is the CRC result.
	 */
	return (remainder);

}   /* crcSlow() */

