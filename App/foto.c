/*
 * foto.c
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
#include "foto.h"

/*******************************************************************************
*  Defines
*******************************************************************************/


/*******************************************************************************
*  Struct
*******************************************************************************/
typedef struct{
	uint8_t idx;
	uint8_t num;
} FotoIndex;

struct FotoTag{
	char foto_list[FILE_LIST_SIZE];
	char file_path[FILE_NAME_SIZE + 10]; // additional '10' is for "S:/Images/"
	FotoIndex index[MAX_NUM_FILES];
	uint8_t dir_length;
	uint8_t foto_num;
	uint8_t pos;
};

/*******************************************************************************
*  Imported types
*******************************************************************************/
void menu_start (void);
/*******************************************************************************
*  Constants and variables
*******************************************************************************/
static lv_obj_t *foto, *btn_prev, *btn_next, *btn_back;
static foto_params_ foto_params;
static const char *dir_path = "S:/Images";
/*******************************************************************************
*  Typedef
*******************************************************************************/

/*******************************************************************************
*  Local functions declarations
*******************************************************************************/
static void foto_create (void);
static uint8_t get_index (void);
static void get_foto (uint8_t pos);

static void btn_prev_cb(lv_obj_t * obj, lv_event_t event);
static void btn_next_cb(lv_obj_t * obj, lv_event_t event);
static void btn_back_cb(lv_obj_t * obj, lv_event_t event);

/*******************************************************************************
*  Local functions definitions
*******************************************************************************/
static void foto_create (void){

	foto = lv_img_create(lv_scr_act(), NULL);
	lv_obj_align(foto, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);

	BUTTON_TRANSP(btn_prev, LV_ALIGN_IN_BOTTOM_RIGHT, -1, -1, _cb)
	BUTTON_TRANSP(btn_next, LV_ALIGN_IN_TOP_RIGHT, -1, 1, _cb)
	BUTTON_TRANSP(btn_back, LV_ALIGN_IN_TOP_LEFT, 1, 1, _cb)
	lv_obj_move_foreground(btn_next);
	lv_obj_move_foreground(btn_prev);
	lv_obj_move_foreground(btn_back);

	get_foto(foto_params.pos);
	lv_img_set_src(foto, (const void*)foto_params.file_path);
}

static uint8_t get_index (void){
	uint_fast8_t count = 0, idx = 0, list = 0;
	int num = 0;

	while (foto_params.foto_list[count]){

		if (foto_params.foto_list[count] == '\n'){

			foto_params.index[list].idx = idx;
			foto_params.index[list].num = num;

			idx = count + 1;
			num = -1;
			list++;
		}

		foto_params.foto_list[count] = tolower(foto_params.foto_list[count]);
		num++;
		count++;

		if (foto_params.foto_list[count] == '\0'){
			foto_params.index[list].idx = idx;
			foto_params.index[list].num = num;
		}
	}

	return (uint8_t)list;
}

static void get_foto (uint8_t pos){
	static uint32_t zero_idx;

    strncpy(&foto_params.file_path[foto_params.dir_length],
    		&foto_params.foto_list[foto_params.index[pos].idx],
			foto_params.index[pos].num);
    zero_idx = foto_params.dir_length + foto_params.index[pos].num;
    foto_params.file_path[zero_idx] = '\0';
}


static void btn_prev_cb(lv_obj_t * obj, lv_event_t event){
	if (event == LV_EVENT_PRESSED){

		if (foto_params.pos) {	foto_params.pos--; }

		get_foto(foto_params.pos);
		lv_img_cache_invalidate_src(NULL);
		lv_img_set_src(foto, (const void*)foto_params.file_path);
	}
}
static void btn_next_cb(lv_obj_t * obj, lv_event_t event){
	if (event == LV_EVENT_PRESSED){

		if (foto_params.pos != foto_params.foto_num - 1) {	foto_params.pos++; }

		get_foto(foto_params.pos);
		lv_img_cache_invalidate_src(NULL);
		lv_img_set_src(foto, (const void*)foto_params.file_path);
	}
}
static void btn_back_cb(lv_obj_t * obj, lv_event_t event){
	if (event == LV_EVENT_PRESSED){
		lv_obj_del(foto);
		lv_obj_del(btn_prev);
		lv_obj_del(btn_next);
		lv_obj_del(btn_back);

		lv_img_cache_invalidate_src(NULL);

		if (lv_fs_dir_close(&dir) != FR_OK){ APP_ERROR() }

		memset(foto_params.foto_list, 0, sizeof(foto_params.foto_list));

		menu_start();
	}
}

/*******************************************************************************
*  Public functions definitions
*******************************************************************************/
void foto_start (void){
	get_list_file (dir_path, foto_params.foto_list, "BIN");
	foto_params.foto_num = get_index();

	foto_params.dir_length = (uint8_t)strlen(dir_path);
	strncpy(foto_params.file_path, dir_path, foto_params.dir_length);
	foto_params.file_path[foto_params.dir_length++] = '/';

	foto_create();
}

