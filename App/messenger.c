/*
 *messenger.c
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
#include "messenger.h"

/*******************************************************************************
*  Defines
*******************************************************************************/

#define TEXT_AREA(obj, height, cobj, param, x, y)	\
	obj = lv_ta_create(lv_scr_act(), NULL);			\
	lv_obj_set_size(obj, LV_HOR_RES, height);		\
	lv_obj_align(obj, cobj, param, x, y);			\
	lv_ta_set_sb_mode(obj, LV_SB_MODE_AUTO);		\
	fstyle_text(obj);								\
	lv_ta_set_text(obj, "");

/*******************************************************************************
*  Typedef
*******************************************************************************/

/*******************************************************************************
*  Struct
*******************************************************************************/
struct MessengerTag{
	uint8_t buf[256];
 __IO uint8_t transmit;
 __IO uint8_t receive;
 __IO uint8_t error;
 __IO uint16_t num;
};
/*******************************************************************************
*  Imported types
*******************************************************************************/
UART_HandleTypeDef huart6;
/*******************************************************************************
*  Constants and variables
*******************************************************************************/
static lv_obj_t *keyboard, *text_in, *text_out, *btn_uart, *label, *led;

static lv_task_t * msg_task;

static msg_params_ msg_params;

/*******************************************************************************
*  Local functions declarations
*******************************************************************************/
static void keyboard_create (void);
static void text_create (void);
static void control_create (void);
static void back_to_menu (void);

static void keyboard_cb (lv_obj_t * obj, lv_event_t event);
static void btn_uart_cb (lv_obj_t * obj, lv_event_t event);
static void msg_cb (lv_task_t * task);

extern void uart_idle_cb (void);
/*******************************************************************************
*  Local functions definitions
*******************************************************************************/
static void keyboard_create (void){
	/*Create a keyboard and apply the styles*/
	keyboard = lv_kb_create(lv_scr_act(), NULL);
	lv_kb_set_cursor_manage(keyboard, true);
	lv_kb_set_style(keyboard, LV_KB_STYLE_BG, &lv_style_transp_tight);
	lv_kb_set_style(keyboard, LV_KB_STYLE_BTN_REL, fstyle_rel());
	lv_kb_set_style(keyboard, LV_KB_STYLE_BTN_PR, fstyle_pr());
	lv_obj_set_event_cb(keyboard, keyboard_cb);
//	lv_obj_refresh_style(keyboard);
}
static void text_create (void){
	/*Create a text areas. The keyboard and uart will write here*/
	TEXT_AREA(text_out, 88, keyboard, LV_ALIGN_OUT_TOP_LEFT, 0, -5)
	TEXT_AREA(text_in, 87, text_out, LV_ALIGN_OUT_TOP_MID, 0, 0)
	/*Assign the text area to the keyboard*/
	lv_kb_set_ta(keyboard, text_out);
	lv_ta_set_cursor_type(text_in, LV_CURSOR_NONE);
}

static void control_create (void){
	/*Create a led indicator*/
	led  = lv_led_create(lv_scr_act(), NULL);
	lv_led_set_style(led, LV_LED_STYLE_MAIN, fstyle_off());
	lv_obj_align(led, text_in, LV_ALIGN_OUT_TOP_RIGHT, -3, -17);
	lv_led_set_bright(led, 190);
	lv_led_on(led);

	/*Create a uart button*/
	BUTTON_HOR_FIT(btn_uart, lv_scr_act(), NULL, led, label,
				LV_ALIGN_OUT_LEFT_MID, -10, 0, _cb)
	lv_label_set_text(label, LV_SYMBOL_LOOP " UART");
}

static void back_to_menu (){
	lv_task_del(msg_task);

	lv_obj_del(keyboard);
	lv_obj_del(text_in);
	lv_obj_del(text_out);
	lv_obj_del(led);
	BUTTON_DEL(btn_uart);
}

static void keyboard_cb (lv_obj_t * obj, lv_event_t event){
	lv_kb_def_event_cb(obj, event);

	if (event == LV_EVENT_CANCEL){
		lv_ta_set_text(text_out, "");
		lv_led_set_style(led, LV_LED_STYLE_MAIN, fstyle_off());
	}
	if (event == LV_EVENT_APPLY){
		lv_ta_set_text(text_in, "");
	}
}

static void btn_uart_cb (lv_obj_t * obj, lv_event_t event){
	if(event == LV_EVENT_PRESSED) {
		uint16_t size = (uint16_t)lv_ta_get_cursor_pos(text_out);
		HAL_UART_Transmit_IT(&huart6, (uint8_t*)lv_ta_get_text(text_out), size);
	}
}

static void msg_cb (lv_task_t * task){
	if (msg_params.transmit){
		msg_params.transmit = 0;
		lv_led_set_style(led, LV_LED_STYLE_MAIN, fstyle_green());

	} else if (msg_params.receive){
		msg_params.receive = 0;
		msg_params.buf[msg_params.num] = '\0';
		lv_ta_add_text(text_in, (const char*)msg_params.buf);
		lv_ta_add_text(text_in, "\n< ");
		lv_led_set_style(led, LV_LED_STYLE_MAIN, fstyle_green());

	} else if (msg_params.error){
		msg_params.error = 0;
		lv_led_set_style(led, LV_LED_STYLE_MAIN, fstyle_red());
	}
}

void uart_idle_cb (void){
	if ( __HAL_UART_GET_FLAG(&huart6, UART_FLAG_IDLE) ){
		__HAL_UART_CLEAR_IDLEFLAG(&huart6);
		huart6.RxState = HAL_UART_STATE_READY;

		msg_params.receive = 1;
		msg_params.num = (uint16_t)(0xFFFF - huart6.RxXferCount);

		HAL_UART_Receive_IT(&huart6, msg_params.buf, (uint16_t)0xFFFF);
	}
}
/*******************************************************************************
*  Public functions definitions
*******************************************************************************/

void msg_start (void){

	if( ! READ_BIT(huart6.Instance->CR2, USART_CR1_IDLEIE) ){
		__HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE);
	}

	HAL_UART_Receive_IT(&huart6, msg_params.buf, (uint16_t)0xFFFF);

	keyboard_create();
	text_create();
	control_create();
	btn_back_create(text_in, back_to_menu);

	msg_task = lv_task_create(msg_cb, 100, LV_TASK_PRIO_LOW, NULL);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
	msg_params.transmit = 1;
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart){
	msg_params.error = 1;
}
