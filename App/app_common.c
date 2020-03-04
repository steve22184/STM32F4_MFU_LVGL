/*
 * app_common.c
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
#include "app_common.h"

/*******************************************************************************
*  Defines
*******************************************************************************/

/*******************************************************************************
*  Common macro
*******************************************************************************/

/*-Book-----------------------------------------------------------------------*/

#define STYLE_MENU(style, style_parent, col, token)	lv_style_t* token##style (void){	\
	lv_style_copy(&style, &style_parent);			\
	style.body.main_color = lv_color_hex(col);		\
	style.body.grad_color = lv_color_hex(col);		\
	style.body.border.color = lv_color_hex(col);	\
	style.body.border.opa = LV_OPA_70;				\
	style.body.opa = LV_OPA_70;						\
	style.text.font = &lv_font_roboto_28;			\
	style.text.color = LV_COLOR_BLACK;				\
	return &style;									\
}

/*******************************************************************************
*  Imported types
*******************************************************************************/
void menu_start(void);

/*******************************************************************************
*  Constants and variables
*******************************************************************************/
static lv_obj_t *btn_back, *label;
static func back_to_menu;
static lv_style_t style_label, style_text;

/*-Title----------------------------------------------------------------------*/
static lv_style_t style_title;

/*-Calendar-------------------------------------------------------------------*/
static lv_style_t style_clock;

/*-Book-----------------------------------------------------------------------*/
static lv_style_t style_page_act, style_page_ina, style_sb, style_menu_rel,
				  style_menu_pr;

/*-Messenger------------------------------------------------------------------*/
static lv_style_t style_rel, style_pr, style_red, style_green, style_off;


/*******************************************************************************
*  Typedefs
*******************************************************************************/

/*******************************************************************************
*  Local functions declarations
*******************************************************************************/
static void btn_back_cb (lv_obj_t * obj, lv_event_t event);
/*******************************************************************************
*  Local functions definitions
*******************************************************************************/
BTN_BACK_CB()

/*******************************************************************************
*  Public functions definitions
*******************************************************************************/
/*-Common--------------------------------------------------------------------------*/

lv_obj_t* btn_back_create (lv_obj_t *obj, func f){
	back_to_menu = f;
	BUTTON_HOR_FIT(btn_back, lv_scr_act(), NULL, obj, label,
					LV_ALIGN_OUT_TOP_LEFT, 1, 0, _cb)
	lv_label_set_text(label, LV_SYMBOL_LEFT " Back");
	return btn_back;
}

void fstyle_label(lv_obj_t *obj){
	lv_style_copy(&style_label, &lv_style_pretty );
	style_label.text.font = &lv_font_roboto_28;
	lv_label_set_style(obj, LV_LABEL_STYLE_MAIN, &style_label);
}

void fstyle_text(lv_obj_t *obj){
	lv_style_copy(&style_text, &lv_style_pretty );
	style_text.text.font = &lv_font_roboto_28;
	lv_ta_set_style(obj, LV_TA_STYLE_BG, &style_text);
}

/*-Title--------------------------------------------------------------------------*/
lv_style_t* fstyle_title(void){
	lv_style_copy(&style_title, &lv_style_pretty);
	style_title.text.color = LV_COLOR_BLUE;
	return &style_title;
}

/*-Calendar--------------------------------------------------------------------------*/

void fstyle_clock(lv_obj_t *obj){

		lv_style_copy(&style_clock, &lv_style_pretty);
		style_clock.text.font = &calistoda_84;
		style_clock.text.color = LV_COLOR_BLUE;
		style_clock.text.opa = LV_OPA_70;
		lv_label_set_style(obj, LV_LABEL_STYLE_MAIN, &style_clock);
}
/*-Book--------------------------------------------------------------------------*/
STYLE_MENU(style_menu_rel, lv_style_btn_rel, 0xCDAA7D, f)
STYLE_MENU(style_menu_pr, lv_style_btn_pr, 0x8B7355, f)

lv_style_t* fstyle_page_act(void){	/*Create active page style*/

	lv_style_copy(&style_page_act, &lv_style_pretty);
	style_page_act.body.main_color = lv_color_hex(0xEEC591);
	style_page_act.body.grad_color = lv_color_hex(0xEEC591);
	style_page_act.body.border.color = lv_color_hex(0xEEC591);
	style_page_act.body.border.opa = LV_OPA_COVER;
	style_page_act.body.opa = LV_OPA_COVER;
	style_page_act.text.font = &lv_font_roboto_28;
	style_page_act.text.color = LV_COLOR_BLACK;
	return &style_page_act;
}

lv_style_t* fstyle_page_ina(void){
	lv_style_copy(&style_page_ina, &style_page_act);
	style_page_ina.text.opa = LV_OPA_30;
	return &style_page_ina;
}
lv_style_t* fstyle_sb(void){	/*Create scrollbar style*/

	lv_style_copy(&style_sb, &style_menu_rel);
	style_sb.body.border.width = 1;
	style_sb.body.radius = LV_RADIUS_CIRCLE;
	style_sb.body.padding.right = 10;
	style_sb.body.padding.bottom =10;
	style_sb.body.padding.inner = 50;	/*Scrollbar width*/
	return &style_sb;
}

/*-Messenger------------------------------------------------------------------*/
lv_style_t* fstyle_off(void){
	lv_style_copy(&style_off, &lv_style_pretty_color);
	style_off.body.radius = LV_RADIUS_CIRCLE;
	style_off.body.main_color = LV_COLOR_MAKE(0xff, 0xef, 0xdb);
	style_off.body.grad_color = LV_COLOR_MAKE(0xcd, 0xc0, 0xb0);
	style_off.body.border.color = LV_COLOR_MAKE(0xfa, 0xeb, 0xd7);
	style_off.body.border.width = 2;
	style_off.body.border.opa = LV_OPA_30;
	style_off.body.shadow.color = LV_COLOR_MAKE(0xfd, 0xf5, 0xe6);
	style_off.body.shadow.width = 4;
	return &style_off;
}
lv_style_t* fstyle_green(void){
	lv_style_copy(&style_green, &style_off);
	style_green.body.main_color = LV_COLOR_MAKE(0x00, 0x8b, 0x00);
	style_green.body.grad_color = LV_COLOR_MAKE(0x00, 0x64, 0x00);
	style_green.body.border.color = LV_COLOR_MAKE(0x22, 0x8b, 0x22);
	style_green.body.shadow.color = LV_COLOR_MAKE(0x00, 0xcd, 0x00);
	return &style_green;
}
lv_style_t* fstyle_red(void){
	lv_style_copy(&style_red, &style_off);
	style_red.body.main_color = LV_COLOR_MAKE(0xb5, 0x0f, 0x04);
	style_red.body.grad_color = LV_COLOR_MAKE(0x50, 0x07, 0x02);
	style_red.body.border.color = LV_COLOR_MAKE(0xfa, 0x0f, 0x00);
	style_red.body.shadow.color = LV_COLOR_MAKE(0xb5, 0x0f, 0x04);
	return &style_red;
}

lv_style_t* fstyle_rel(void){
	lv_style_copy(&style_rel, &lv_style_btn_rel);
	style_rel.body.radius = 0;
	style_rel.body.border.width = 1;
	style_rel.text.font = &lv_font_roboto_28;
	return &style_rel;
}
lv_style_t* fstyle_pr(void){
	lv_style_copy(&style_pr, &lv_style_btn_pr);
	style_pr.body.radius = 0;
	style_pr.body.border.width = 1;
	style_pr.text.font = &lv_font_roboto_28;
	return &style_pr;
}
