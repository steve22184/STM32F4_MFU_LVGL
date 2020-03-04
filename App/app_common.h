/*
 * app_common.h
 *
 *  Created on: 21 янв. 2020 г.
 *      Author: ALSCode
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

#ifndef APP_COMMON_H_
#define APP_COMMON_H_

/*******************************************************************************
*  Includes
*******************************************************************************/
#pragma once
#include "lvgl.h"

/*******************************************************************************
*  Defines
*******************************************************************************/
#if _USE_LFN
#define FILE_NAME_SIZE	255
#else
#define FILE_NAME_SIZE	12
#endif

#define MAX_NUM_FILES	12

#define FILE_LIST_SIZE		((FILE_NAME_SIZE + 6) * MAX_NUM_FILES + 1)
/*******************************************************************************
*  Common macro
*******************************************************************************/
#define APP_ERROR()		while(1){ Error_Handler(); }

#define BUTTON_HOR_FIT(obj, pobj, copy, cobj, label, param, x, y, cb)	\
		obj = lv_btn_create(pobj, copy);								\
		lv_btn_set_fit2(obj, LV_FIT_NONE, LV_FIT_TIGHT);				\
		lv_obj_align(obj, cobj, param, x, y);							\
		label = lv_label_create(obj, NULL);								\
		lv_obj_set_event_cb(obj, obj##cb);

#define BUTTON_DEL(obj)		\
		lv_obj_clean(obj);	\
		lv_obj_del(obj);

#define BTN_BACK_CB()	\
		static void btn_back_cb (lv_obj_t * obj, lv_event_t event){	\
		if(event == LV_EVENT_PRESSED) {								\
			BUTTON_DEL(btn_back)									\
			back_to_menu();											\
			menu_start();											\
			}														\
		}

#define BUTTON_TRANSP(obj, align, x, y, cb) 							\
	obj = lv_btn_create(lv_scr_act(), NULL);					\
	lv_obj_align(obj, NULL, align, x, y);						\
	lv_btn_set_style(obj, LV_BTN_STYLE_REL, &lv_style_transp);	\
    lv_btn_set_style(obj, LV_BTN_STYLE_PR, &lv_style_transp);	\
	lv_obj_set_event_cb(obj, obj##cb);
/*******************************************************************************
*  Constants and variables
*******************************************************************************/

/*******************************************************************************
*  Enums
*******************************************************************************/

/*******************************************************************************
*  Typedefs
*******************************************************************************/
typedef void(*func)(void);
/*******************************************************************************
*  Struct
*******************************************************************************/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*******************************************************************************
*  Public functions declarations
*******************************************************************************/

/*-Common---------------------------------------------------------------------*/
lv_obj_t* btn_back_create (lv_obj_t *obj, func f);
void fstyle_btn(lv_obj_t* btn);
void fstyle_label(lv_obj_t *obj);
void fstyle_text(lv_obj_t *obj);

/*-Title----------------------------------------------------------------------*/
lv_style_t* fstyle_title(void);

/*-Calendar-------------------------------------------------------------------*/
void fstyle_clock(lv_obj_t *obj);
/*-Book-----------------------------------------------------------------------*/
lv_style_t* fstyle_page_act(void);
lv_style_t* fstyle_page_ina(void);
lv_style_t* fstyle_menu_rel(void);
lv_style_t* fstyle_menu_pr(void);
lv_style_t* fstyle_sb(void);

/*-Messenger------------------------------------------------------------------*/
lv_style_t* fstyle_red(void);
lv_style_t* fstyle_green(void);
lv_style_t* fstyle_off(void);
lv_style_t* fstyle_rel(void);
lv_style_t* fstyle_pr(void);

#endif /* APP_COMMON_H_ */
