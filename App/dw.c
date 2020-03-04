
/*
 * dw.c
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
#include "dw.h"

/*******************************************************************************
*  Defines
*******************************************************************************/

struct DWTag{
	uint16_t sample_numbers;
	volatile uint8_t adc_convert_ready; // 1 - conv. complete; 0 - not complete
};

/*******************************************************************************
*  Imported types
*******************************************************************************/

ADC_HandleTypeDef hadc3;
TIM_HandleTypeDef htim2;

/*******************************************************************************
*  Constants and variables
*******************************************************************************/
static lv_obj_t *chart, *btn_once, *btn_repeat, *label;
static lv_chart_series_t* ser;
static lv_task_t * chart_task;

static uint16_t adc_data[MAX_SAMPLE]; 	//array to save raw ADC data*/
static dw_params_ dw_params;

/*******************************************************************************
*  Typenames
*******************************************************************************/

/*******************************************************************************
*  Local functions declarations
*******************************************************************************/
static void adc_restart (void);
static void chart_redraw (void * params);
static void chart_create (void);
static void controls_create (void);
static void back_to_menu (void);

static void chart_cb (lv_task_t * task);
static void btn_once_cb (lv_obj_t * obj, lv_event_t event);
static void btn_repeat_cb (lv_obj_t * obj, lv_event_t event);

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);
/*******************************************************************************
*  Local functions definitions
*******************************************************************************/

static void adc_restart (void){
	HAL_ADC_DeInit(&hadc3);
	HAL_ADC_Init(&hadc3);
	/*start ADC sampling into buffer*/
	HAL_ADC_Start_DMA(&hadc3, (uint32_t*)adc_data, (uint32_t)dw_params.sample_numbers);
}

static void chart_redraw (void * params){
	dw_params_ *dw_params = (dw_params_ *)(params);

	if ( dw_params->adc_convert_ready ){

		dw_params->adc_convert_ready = 0;

		for (uint_fast16_t i = 0; i != dw_params->sample_numbers; ++i){
			adc_data[i] = (uint16_t)( adc_data[i] * 330 / 4096 + 30);
		}

		lv_chart_set_point_count(chart, dw_params->sample_numbers);
		lv_chart_set_points(chart, ser, (lv_coord_t*)adc_data);
		adc_restart ();
	}
}
static void chart_cb (lv_task_t * task){
	chart_redraw(task->user_data);
}

static void chart_create (void){
	chart = lv_chart_create(lv_scr_act(), NULL);
	lv_obj_set_size(chart, 700, 400);
	lv_chart_set_div_line_count(chart, 3, 6);
	lv_obj_align(chart, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 20, -20);
	lv_chart_set_type(chart, LV_CHART_TYPE_LINE);   /*Show line*/
	lv_chart_set_series_opa(chart, LV_OPA_COVER);   /*Opacity of the data series*/
	lv_chart_set_series_width(chart, 2);            /*Line width and point radius*/
	lv_chart_set_range(chart, 0, 400);

	lv_chart_set_point_count(chart, MAX_SAMPLE);	/*number of points in the series*/
	ser = lv_chart_add_series(chart, LV_COLOR_GREEN);

	lv_chart_set_points(chart, ser, (lv_coord_t*)adc_data);
}

static void controls_create (void){

	btn_back_create (chart, back_to_menu);

	/*Btn_once*/
	BUTTON_HOR_FIT(btn_once, lv_scr_act(), NULL, chart, label,
			LV_ALIGN_OUT_TOP_RIGHT, 0, 0, _cb)
	lv_label_set_text(label, "Once");

	/*Btn_repeat*/
	BUTTON_HOR_FIT(btn_repeat, lv_scr_act(), NULL, chart, label,
				LV_ALIGN_OUT_TOP_RIGHT, -105, 0, _cb)
	lv_label_set_text(label, "Repeat");
}

static void back_to_menu (void){

	if (chart_task != NULL){
		lv_task_del(chart_task);
	}
	lv_obj_del(chart);

	BUTTON_DEL(btn_once)
	BUTTON_DEL(btn_repeat)
}


static void btn_once_cb (lv_obj_t * obj, lv_event_t event){
	if(event == LV_EVENT_PRESSED) {
		chart_redraw((void*)&dw_params);
	}
}

static void btn_repeat_cb (lv_obj_t * obj, lv_event_t event){
	static uint8_t toggle = 1;
	if(event == LV_EVENT_PRESSED) {

		if(toggle) {
			chart_task = lv_task_create(chart_cb, 2000, LV_TASK_PRIO_LOW, (void*)&dw_params);
		} else {
			lv_task_del(chart_task);
		}
		toggle = !toggle;
	}
}

/*******************************************************************************
*  Public functions definitions
*******************************************************************************/
void dw_start (void){
	memset((void*)adc_data, 0, sizeof adc_data);
	dw_params.sample_numbers = (uint16_t)MAX_SAMPLE;

	chart_create();
	controls_create();
	adc_restart();
}

void adc_start (void){
	/*start ADC sampling into buffer*/
	uint16_t temp[2] = {0};
	HAL_ADC_Start_DMA(&hadc3, (uint32_t*)temp, (uint32_t)2 );
	/*start TIM which triggers ADC conversions*/
	HAL_TIM_OC_Start(&htim2, TIM_CHANNEL_2);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){

	CLEAR_BIT(hadc->Instance->CR2, ADC_CR2_DMA);
	while (READ_BIT(hadc->Instance->CR2, ADC_CR2_DMA) ) {;}
	SET_BIT(hadc->Instance->CR2, ADC_CR2_DMA);

	dw_params.adc_convert_ready = 1;
}
