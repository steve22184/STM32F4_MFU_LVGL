
/*
 * menu.c
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

#include "menu.h"

/*******************************************************************************
*  Defines
*******************************************************************************/

/*******************************************************************************
*  Common macro
*******************************************************************************/

#define LIST_BTN_CB(token, cb, start) 							\
		static void token##cb(lv_obj_t * obj, lv_event_t event)	\
		{														\
			if(event == LV_EVENT_PRESSED) {						\
				lv_obj_del(list);								\
				token##start();									\
			}													\
		}
/*******************************************************************************
*  Constants and variables
*******************************************************************************/

static lv_obj_t *list;

#if CALENDAR
static lv_obj_t *list_calendar;
#endif
#if DATA_ANALYZER
static lv_obj_t *list_dw;
#endif
#if MESSENGER
static lv_obj_t *list_msg;
#endif
#if BOOK
static lv_obj_t *list_book;
#endif
#if FOTO
static lv_obj_t *list_foto;
#endif
/*******************************************************************************
*  Typenames
*******************************************************************************/

/*******************************************************************************
*  Local functions declarations
*******************************************************************************/
#if DATA_ANALYZER
static void dw_cb(lv_obj_t * obj, lv_event_t event);
#endif
#if CALENDAR
static void calendar_cb(lv_obj_t * obj, lv_event_t event);
#endif
#if MESSENGER
static void msg_cb(lv_obj_t * obj, lv_event_t event);
#endif
#if BOOK
static void book_cb(lv_obj_t * obj, lv_event_t event);
#endif
#if FOTO
static void foto_cb(lv_obj_t * obj, lv_event_t event);
#endif
/*******************************************************************************
*  Local functions definitions
*******************************************************************************/
#if DATA_ANALYZER
LIST_BTN_CB(dw, _cb, _start)

#endif
#if CALENDAR
LIST_BTN_CB(calendar, _cb, _start)
#endif

#if MESSENGER
LIST_BTN_CB(msg, _cb, _start)
#endif

#if BOOK
LIST_BTN_CB(book, _cb, _start)
#endif

#if FOTO
LIST_BTN_CB(foto, _cb, _start)
#endif
/*******************************************************************************
*  Public functions definitions
*******************************************************************************/
extern void menu_start(void);

void menu_start (void){

	list = lv_list_create(lv_scr_act(), NULL);
	lv_obj_set_size(list, 400, 400);
	lv_obj_set_pos(list, 200, 40);
	lv_list_set_sb_mode(list, LV_SB_MODE_AUTO);

#if CALENDAR
	list_calendar = lv_list_add_btn(list, NULL, LV_SYMBOL_LIST " Calendar");
	lv_obj_set_event_cb(list_calendar, calendar_cb);
#endif
#if DATA_ANALYZER
	list_dw = lv_list_add_btn(list, NULL, LV_SYMBOL_EYE_OPEN " Data Analyzer");
	lv_obj_set_event_cb(list_dw, dw_cb);
#endif
#if MESSENGER
	list_msg = lv_list_add_btn(list, NULL, LV_SYMBOL_EDIT " Messenger");
	lv_obj_set_event_cb(list_msg, msg_cb);
#endif
#if BOOK
	list_book = lv_list_add_btn(list, NULL, LV_SYMBOL_FILE " Pocketbook");
	lv_obj_set_event_cb(list_book, book_cb);
#endif
#if FOTO
	list_foto = lv_list_add_btn(list, NULL, LV_SYMBOL_IMAGE " Photo");
	lv_obj_set_event_cb(list_foto, foto_cb);
#endif
}
