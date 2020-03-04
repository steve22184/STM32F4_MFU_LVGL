/*
 * fatfs_storage.c
 *
 *  Created on: 19 янв. 2020 г.
 *      Author: ALScode
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*******************************************************************************
*  Includes
*******************************************************************************/
#include "fatfs_storage.h"

/*******************************************************************************
*  Defines
*******************************************************************************/

/*******************************************************************************
*  Common macro
*******************************************************************************/

/*******************************************************************************
*  Imported types
*******************************************************************************/

/*******************************************************************************
*  Constants and variables
*******************************************************************************/

/*******************************************************************************
*  Typedefs
*******************************************************************************/

/*******************************************************************************
*  Local functions declarations
*******************************************************************************/

/*******************************************************************************
*  Local functions definitions
*******************************************************************************/

/*******************************************************************************
*  Public functions definitions
*******************************************************************************/
uint32_t get_list_file (const char* dir_name, char files[], const char* suf){

	lv_fs_res_t res;
	uint32_t counter = 0, index = 0, num_files = 0;

	/* Open directory */
	res = lv_fs_dir_open(&dir, dir_name);

	if (res == FR_OK)
	{
		for (;;)
		{
			res = lv_fs_dir_read(&dir, NULL);

			if (res != FR_OK || fno.fname[0] == 0){
				break;
			}
			if (fno.fname[0] == '.'){
				continue;
			}

			if (!(fno.fattrib & AM_DIR))
			{
				do {
					counter++;
				} while (fno.fname[counter] != 0x2E); /* . */

				if (num_files < MAX_NUM_FILES)
				{
					if ((fno.fname[counter + 1] == suf[0]) && (fno.fname[counter + 2] == suf[1]) && (fno.fname[counter + 3] == suf[2]))
					{
						if(sizeof(fno.fname) <= (FILE_NAME_SIZE + 1))
						{
							strncpy (&files[index], fno.fname, counter + 4);
							index += (counter + 4);
							files[index++] = '\n';
							num_files++;
						}
					}
				}
				counter = 0;
			}
		}
	}
	return index;
}

uint32_t open_read_txt (const char* file_path, char buf[], uint32_t size){
	uint32_t bytes_read;

	if (lv_fs_open(&file, file_path, FA_READ|FA_WRITE) != FR_OK){ APP_ERROR() }
	if (lv_fs_read (&file, buf, size, &bytes_read) != FR_OK){ APP_ERROR() }
	if (lv_fs_close (&file) != FR_OK){ APP_ERROR() }

	return bytes_read;
}

uint32_t change_page (const char* file_path, char buf[], uint32_t size, FSIZE_t fptr){
	uint32_t bytes_read;

	if (lv_fs_open(&file, file_path, FA_READ|FA_WRITE) != FR_OK){ APP_ERROR() }
	if (lv_fs_seek (&file, fptr) != FR_OK){ APP_ERROR() }
	if (lv_fs_read (&file, buf, size, &bytes_read) != FR_OK){ APP_ERROR() }
	if (lv_fs_close (&file) != FR_OK){ APP_ERROR() }

	return bytes_read;
}


