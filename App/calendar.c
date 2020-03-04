
/*
 * calendar.c
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
#include "calendar.h"
/*******************************************************************************
*  Defines
*******************************************************************************/

/*******************************************************************************
*  Common macro
*******************************************************************************/

#define ROLL(obj, cobj, data, param, x, y, cb) 					\
	obj = lv_roller_create(lv_scr_act(), NULL);					\
	lv_roller_set_options(obj, data, LV_ROLLER_MODE_INIFINITE);	\
	lv_roller_set_visible_row_count(obj, 4);					\
	lv_roller_set_fix_width(obj, 100);							\
	lv_obj_align(obj, cobj, param, x, y);						\
	lv_obj_set_event_cb(obj, obj##cb);

#define LABEL(obj, cobj, text, param, x, y)		\
	obj = lv_label_create(lv_scr_act(), NULL);	\
	lv_obj_align(obj, cobj, param, x, y);		\
	lv_label_set_text(obj, text);				\
	fstyle_label(obj);

/*******************************************************************************
*  Typedefs
*******************************************************************************/

/*******************************************************************************
*  Enums
*******************************************************************************/

/*******************************************************************************
*  Struct
*******************************************************************************/
struct CalendarTag{
	lv_calendar_date_t date;
	RTC_TimeTypeDef time;
	RTC_DateTypeDef k_date;
	uint8_t mode; 	// '0' - set mode; '1' - clock mode
};

/*******************************************************************************
*  Imported types
*******************************************************************************/

/*******************************************************************************
*  Constants and variables
*******************************************************************************/
static lv_obj_t *calendar;

static lv_obj_t *day_roll, *month_roll, *year_roll;
static lv_obj_t *hour_roll, *minute_roll, *second_roll;
static lv_obj_t *date_label, *time_label, *clock_label, *btn_done_label;

static lv_obj_t *btn_done;

static lv_task_t *clock_task;

static const char *month = "January\nFebruary\nMarch\nApril\nMay\nJune\nJuly\nAugust\n"
							"September\nOctober\nNovember\nDecember";

static const char *day = "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n"
						 "16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n";

static const char *year = "2000\n2001\n2002\n2003\n2004\n2005\n2006\n2007\n2008\n"
						  "2009\n2010\n2011\n2012\n2013\n2014\n2015\n2016\n2017\n2018\n"
						  "2019\n2020\n";
//static const char *date = "Sunday\nMonday\nTuesday\nWednesday\nThursday\nFriday\nSaturday\n";

static const char *hour = "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n"
						  "13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n";

static const char *min_sec = "01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n"
			 	 	 	 	 "13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n"
			 	 	 	 	 "26\n27\n28\n30\n31\n32\n33\n34\n35\n35\n36\n37\n38\n"
			 	 	 	 	 "39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n"
			 	 	 	 	 "52\n53\n54\n55\n56\n57\n58\n59\n60\n";
static char clock[10];
static c_params_ c_params;

/*******************************************************************************
*  Local functions declarations
*******************************************************************************/
static void calendar_create(void);
static void set_module_create(void);
static void set_module_delete (void);
static void clock_create(void);
static void clock_delete(void);
static void back_to_menu (void);


static void calendar_cb(lv_obj_t * obj, lv_event_t event);
static void clock_cb(lv_task_t * task);

static void day_roll_cb(lv_obj_t * obj, lv_event_t event);
static void month_roll_cb(lv_obj_t * obj, lv_event_t event);
static void year_roll_cb(lv_obj_t * obj, lv_event_t event);

static void hour_roll_cb(lv_obj_t * obj, lv_event_t event);
static void minute_roll_cb(lv_obj_t * obj, lv_event_t event);
static void second_roll_cb(lv_obj_t * obj, lv_event_t event);

static void btn_done_cb(lv_obj_t * obj, lv_event_t event);
/*******************************************************************************
*  Local functions definitions
*******************************************************************************/
static void calendar_create(void){ // TODO Implement ability to choose day of week
	calendar = lv_calendar_create(lv_scr_act(), NULL);
	lv_obj_set_size(calendar, 400, 400);
	lv_obj_align(calendar, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 20, -20);

	lv_calendar_date_t today;
	today.year = 2000;
	today.month = 1;
	today.day = 1;

	lv_calendar_set_today_date(calendar, &today);
	lv_calendar_set_showed_date(calendar, &today);

	lv_obj_set_event_cb(calendar, calendar_cb);
}

static void set_module_create (void){
	LABEL(date_label, calendar, "Set date",  LV_ALIGN_OUT_RIGHT_TOP, 20, 0)
	LABEL(time_label, calendar, "Set time",  LV_ALIGN_OUT_RIGHT_MID, 20, -20)

	ROLL(day_roll, date_label, day, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10, _cb)
	ROLL(month_roll, day_roll, month, LV_ALIGN_OUT_RIGHT_MID, 10, 0, _cb)
	ROLL(year_roll, month_roll, year, LV_ALIGN_OUT_RIGHT_MID, 10, 0, _cb)

	ROLL(hour_roll, time_label, hour, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10, _cb)
	ROLL(minute_roll, hour_roll, min_sec, LV_ALIGN_OUT_RIGHT_MID, 10, 0, _cb)
	ROLL(second_roll, minute_roll, min_sec, LV_ALIGN_OUT_RIGHT_MID, 10, 0, _cb)

	BUTTON_HOR_FIT(btn_done, lv_scr_act(), NULL, NULL, btn_done_label,
			LV_ALIGN_IN_BOTTOM_RIGHT, -165, -15, _cb)
	lv_label_set_text(btn_done_label, LV_SYMBOL_OK " Done");
}

static void set_module_delete (void){

	lv_obj_del(day_roll);
	lv_obj_del(month_roll);
	lv_obj_del(year_roll);
	lv_obj_del(hour_roll);
	lv_obj_del(minute_roll);
	lv_obj_del(second_roll);
	lv_obj_del(date_label);
	lv_obj_del(time_label);
	BUTTON_DEL(btn_done);
}

static void clock_create(void){
	c_params.mode = 1;

	k_GetTime(&c_params.time);
	sprintf(clock, "%02d:%02d:%02d", c_params.time.Hours,c_params.time.Minutes,
									c_params.time.Seconds);

	clock_label =  lv_label_create(lv_scr_act(), NULL);
    lv_label_set_static_text(clock_label, clock);
    lv_obj_set_width(clock_label, 150);
    lv_obj_align(clock_label, calendar, LV_ALIGN_OUT_RIGHT_MID, 5, -50);
    fstyle_clock(clock_label);

    clock_task = lv_task_create(clock_cb, 100, LV_TASK_PRIO_HIGH, NULL);
}

static void clock_delete(void){
	lv_task_del(clock_task);
	lv_obj_del(clock_label);
}

static void back_to_menu (void){

	if(c_params.mode == 0){
		set_module_delete();
	} else {
		clock_delete();
	}
	lv_obj_del(calendar);
}


static void calendar_cb(lv_obj_t * obj, lv_event_t event){
	if(event == LV_EVENT_PRESSED) {
		lv_calendar_date_t *date = NULL;
		date = lv_calendar_get_pressed_date(obj);

		if(date) {
			c_params.date = *date;
			lv_calendar_set_today_date(obj, date);
			lv_calendar_set_showed_date(obj, date);
		}
	}
	if (event == LV_EVENT_VALUE_CHANGED){
		lv_calendar_set_today_date(obj, &c_params.date);
		lv_calendar_set_showed_date(obj, &c_params.date);
	}
}

static void clock_cb(lv_task_t * task){
	k_GetTime(&c_params.time);
	k_GetDate(&c_params.k_date);

	sprintf(clock, "%02d:%02d:%02d", c_params.time.Hours,c_params.time.Minutes,
			c_params.time.Seconds);

	lv_label_set_static_text(clock_label, clock);
}

static void day_roll_cb(lv_obj_t * obj, lv_event_t event){
	if(event == LV_EVENT_VALUE_CHANGED) {
		uint16_t res = (lv_roller_get_selected(obj) + 1);
		c_params.date.day = (int8_t)res;

	}
}

static void month_roll_cb(lv_obj_t * obj, lv_event_t event){
	if(event == LV_EVENT_VALUE_CHANGED) {
		uint16_t res = (lv_roller_get_selected(obj) + 1);
		c_params.date.month = (int8_t)res;
	}
}

static void year_roll_cb(lv_obj_t * obj, lv_event_t event){
	if(event == LV_EVENT_VALUE_CHANGED) {
		uint16_t res = lv_roller_get_selected(obj);
		c_params.date.year = (uint16_t)(res + 2000);
	}
}

static void hour_roll_cb(lv_obj_t * obj, lv_event_t event){
	if(event == LV_EVENT_VALUE_CHANGED) {
		c_params.time.Hours = (uint8_t)(lv_roller_get_selected(obj));
	}
}

static void minute_roll_cb(lv_obj_t * obj, lv_event_t event){
	if(event == LV_EVENT_VALUE_CHANGED) {
		c_params.time.Minutes = (uint8_t)(lv_roller_get_selected(obj) + 1);
	}
}

static void second_roll_cb(lv_obj_t * obj, lv_event_t event){
	if(event == LV_EVENT_VALUE_CHANGED) {
		c_params.time.Seconds = (uint8_t)(lv_roller_get_selected(obj) + 1);
	}
}

static void btn_done_cb(lv_obj_t * obj, lv_event_t event){
	if(event == LV_EVENT_PRESSED) {
		lv_event_send(calendar, LV_EVENT_VALUE_CHANGED, NULL);

		k_SetTime(&c_params.time);

		set_module_delete();
		clock_create();
	}
}

/*******************************************************************************
*  Public functions definitions
*******************************************************************************/
void calendar_start (void){
	calendar_create();
	btn_back_create(calendar, back_to_menu);

	if (c_params.mode) {
		clock_create();
	} else {
		set_module_create();
	}
}
