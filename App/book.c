
/*
 * book.c
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
#include "book.h"

/*******************************************************************************
*  Defines
*******************************************************************************/
#define PAGE_SIZE 		UINT32_C(600) //symbols, for lv_font_roboto28
#define PAGE_BUF_SIZE	PAGE_SIZE + 1
#define TA_BUF_SIZE		PAGE_BUF_SIZE + 50 // '50' for alighment purposes

#define DDLIST_SIZE		((FILE_NAME_SIZE + 6) * MAX_NUM_FILES + 1)
#define MENU_TIME		UINT32_C(3000) // milliseconds

/*******************************************************************************
*  Common macro
*******************************************************************************/
#define CHANGE_PAGE() \
	book_params.last_read = change_page(book_params.file_path, book_params.page_buf, \
											PAGE_SIZE, book_params.page_ptr);	 		 \
	page_prepare(); 																 \
	lv_ta_set_text(page, book_params.ta_buf);
/*******************************************************************************
*  Typedef
*******************************************************************************/

/*******************************************************************************
*  Struct
*******************************************************************************/
struct BookTag{
	char page_buf[PAGE_BUF_SIZE];
	char ta_buf[TA_BUF_SIZE];
	char ddlist[DDLIST_SIZE];
	char file_path[FILE_NAME_SIZE + 9]; // additional '9' is for "S:/Books/"
	char book_name[FILE_NAME_SIZE + 1]; // additional '1' is for '\0'
	FSIZE_t page_ptr;
	FSIZE_t last_read;
};

/*******************************************************************************
*  Imported types
*******************************************************************************/
void menu_start (void);
/*******************************************************************************
*  Constants and variables
*******************************************************************************/
static lv_obj_t *page, *btn_prev, *btn_next, *btn_menu,
				*btn_back, *label, *ddlist;
static lv_task_t *menu_task;
static book_params_ book_params;

static const char *dir_path = "S:/Books";
/*******************************************************************************
*  Typenames
*******************************************************************************/

/*******************************************************************************
*  Local functions declarations
*******************************************************************************/
static void page_create (void);
static void menu_create (void);
static void back_to_menu(void);
static void page_prepare(void);

static void btn_next_cb(lv_obj_t * obj, lv_event_t event);
static void btn_prev_cb(lv_obj_t * obj, lv_event_t event);
static void btn_menu_cb(lv_obj_t * obj, lv_event_t event);
static void btn_back_cb (lv_obj_t * obj, lv_event_t event);
static void ddlist_cb (lv_obj_t * obj, lv_event_t event);

static void menu_task_cb (lv_task_t * task);

/*******************************************************************************
*  Local functions definitions
*******************************************************************************/
void page_create (void){

	/*Create page*/
	page = lv_ta_create(lv_scr_act(), NULL);
    lv_obj_set_size(page, LV_HOR_RES, LV_VER_RES);
    lv_obj_align(page, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_ta_set_sb_mode(page, LV_SB_MODE_DRAG);

    lv_ta_set_style(page, LV_TA_STYLE_BG, fstyle_page_act());
    lv_ta_set_style(page, LV_TA_STYLE_SB, fstyle_sb());
    lv_ta_set_text(page, "");

    BUTTON_TRANSP(btn_prev, LV_ALIGN_IN_TOP_RIGHT, 0, 0, _cb)
    BUTTON_TRANSP(btn_next, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0, _cb)
	BUTTON_TRANSP(btn_menu, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0, _cb)
}

void menu_create (void){

	BUTTON_HOR_FIT(btn_back, lv_scr_act(), NULL, NULL, label, LV_ALIGN_IN_TOP_LEFT, 1, 0, _cb)
	lv_btn_set_style(btn_back, LV_BTN_STYLE_REL, fstyle_menu_rel());
	lv_btn_set_style(btn_back, LV_BTN_STYLE_PR, fstyle_menu_pr());
	lv_label_set_text(label, LV_SYMBOL_LEFT " Back");

	ddlist = lv_ddlist_create(lv_scr_act(), NULL);
	lv_ddlist_set_options(ddlist, book_params.ddlist);

	lv_ddlist_set_fix_width(ddlist, 300);
	lv_ddlist_set_sb_mode(ddlist, LV_SB_MODE_AUTO);
	lv_obj_align(ddlist, NULL, LV_ALIGN_CENTER, 0, -100);

	lv_ta_set_style(ddlist, LV_DDLIST_STYLE_BG, fstyle_menu_rel());
	lv_ta_set_style(ddlist, LV_DDLIST_STYLE_SEL, fstyle_menu_rel());

	lv_obj_set_event_cb(ddlist, ddlist_cb);
}

void back_to_menu(void){
	lv_obj_del(page);
	lv_obj_del(btn_prev);
	lv_obj_del(btn_next);
	lv_obj_del(btn_menu);
	lv_obj_del(ddlist);
	if (lv_fs_dir_close(&dir) != FR_OK){ APP_ERROR() }
	memset(book_params.ddlist, 0, sizeof(book_params.ddlist));
	book_params.page_ptr = 0;
}

static void page_prepare(void){
	FSIZE_t page_count = 0,
			ta_count = 0,
			line_count = 0,
			strings = 0;

	book_params.page_buf[book_params.last_read] = '\0';
	book_params.ta_buf[book_params.last_read] = '\0';

	while (book_params.page_buf[page_count] != '\0'){

		if ( book_params.page_buf[page_count] == '\r' &&
				book_params.page_buf[page_count + 1] == '\n') {

			if ( isalpha( book_params.page_buf[page_count + 2]) &&
					islower( book_params.page_buf[page_count + 2]) ){
				page_count += 2;
				book_params.ta_buf[ta_count] = ' ';
				ta_count++;
				line_count++;
			} else {
				strings++;
				line_count = 0;
			}
		}

		if (line_count >= 50 && book_params.page_buf[page_count] == ' '){
			book_params.ta_buf[ta_count] = book_params.page_buf[page_count];
			ta_count++;
			page_count++;
			book_params.ta_buf[ta_count++] = '\n';
			strings++;
			line_count = 0;
		}

		book_params.ta_buf[ta_count] = book_params.page_buf[page_count];
		page_count++;
		ta_count++;
		line_count++;
	}

	book_params.ta_buf[ta_count] = '\0';
}

void btn_next_cb(lv_obj_t * obj, lv_event_t event){
	if (event == LV_EVENT_PRESSED){
		BSP_LED_Toggle(LED_GREEN);

		book_params.page_ptr += book_params.last_read;
		CHANGE_PAGE()
	}
}
void btn_prev_cb(lv_obj_t * obj, lv_event_t event){
	if (event == LV_EVENT_PRESSED){
		BSP_LED_Toggle(LED_ORANGE);

		if (book_params.page_ptr >= book_params.last_read){
			book_params.page_ptr -= book_params.last_read;
			CHANGE_PAGE()
		}
	}
}

void btn_menu_cb(lv_obj_t * obj, lv_event_t event){
	if (event == LV_EVENT_PRESSED){
		strncpy(book_params.file_path, dir_path, strlen(dir_path));
		get_list_file(dir_path, book_params.ddlist, "TXT");

		lv_ta_set_style(page, LV_TA_STYLE_BG, fstyle_page_ina());
		menu_create();
	}
}

BTN_BACK_CB()

static void ddlist_cb (lv_obj_t * obj, lv_event_t event){

	if (event == (LV_EVENT_VALUE_CHANGED ) ){
		if (menu_task){
			lv_task_reset(menu_task);
		}
		memset(book_params.book_name, 0, FILE_NAME_SIZE + 1);

		size_t dir_length = strlen(dir_path);
		book_params.file_path[dir_length++] = '/';
		memset(&book_params.file_path[dir_length], 0, FILE_NAME_SIZE);

		lv_ddlist_get_selected_str(ddlist, book_params.book_name, FILE_NAME_SIZE + 1);
		size_t name_length = strlen(book_params.book_name);

		if (name_length) {
			strncpy(&book_params.file_path[dir_length], book_params.book_name, name_length);
			book_params.last_read = open_read_txt(book_params.file_path, book_params.page_buf, PAGE_SIZE);
			page_prepare();
			lv_ta_set_text(page, book_params.ta_buf);
		}

	} else if (event == LV_EVENT_PRESSED){
			lv_task_reset(menu_task);

	} else if (event == LV_EVENT_RELEASED){

		if (menu_task == NULL){
			menu_task = lv_task_create(menu_task_cb, MENU_TIME, LV_TASK_PRIO_LOW, NULL);
			lv_task_once(menu_task);
		}
	}
}

static void menu_task_cb (lv_task_t * task){
	lv_obj_del(ddlist);
	BUTTON_DEL(btn_back);
	lv_ta_set_style(page, LV_TA_STYLE_BG, fstyle_page_act());
	menu_task = NULL;
	if (lv_fs_dir_close(&dir) != FR_OK){ APP_ERROR() }
}

/*******************************************************************************
*  Public functions definitions
*******************************************************************************/
void book_start (void){
	page_create();
}
