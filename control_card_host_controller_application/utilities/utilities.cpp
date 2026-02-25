/*
 * utilities.cpp
 *
 *  Created on: Jan 25, 2026
 *      Author: Amogh MP
 */


#include "main.h"
#include "utilities.h"
#include "includes.h"



void string_to_hex(const char* str, uint8_t* holder, uint8_t size)
{
    if (str != NULL && holder != NULL)
    {
		size_t len = strlen(str);

		for (uint8_t i = 0 ; i < size ; i++)
		{
			if (i * 2 + 1 < len)
			{
				char byteString[3];

				byteString[0] = str[i * 2];
				byteString[1] = str[i * 2 + 1];
				byteString[2] = '\0';

				holder[i] = (uint8_t)strtol(byteString, NULL, 16);
			}
			else
			{
				holder[i] = 0;
			}
		}
    }
}


uint8_t is_buffer_not_empty(const uint8_t* buffer, uint16_t size)
{
    for (uint16_t i = 0 ; i < size ; i++)
    {
        if (buffer[i] != 0)
		{
        	return 1;
		}
    }

    return 0;
}
