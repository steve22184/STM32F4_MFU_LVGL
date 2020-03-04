/*
 * title.c
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
#include "title.h"

/*******************************************************************************
*  Defines
*******************************************************************************/
#define FRAMES_NUM	UINT8_C(22);

/*******************************************************************************
*  Struct
*******************************************************************************/
struct TitleTag{

};

/*******************************************************************************
*  Imported types
*******************************************************************************/
void menu_start (void);
/*******************************************************************************
*  Constants and variables
*******************************************************************************/
static lv_obj_t *lawn, *fly, *label;
static lv_task_t *title_task;

LV_IMG_DECLARE(grass);
extern const lv_img_dsc_t image1, image2, image3, image4, image5,
							image6, image7, image8, image9, image10,
							image11, image12, image13, image14, image15,
							image16, image17, image18, image19, image20,
							image21, image22;
const lv_img_dsc_t *frames[] = {&image1, &image2, &image3, &image4, &image5,
							&image6, &image7, &image8, &image9, &image10,
							&image11, &image12, &image13, &image14, &image15,
							&image16, &image17, &image18, &image19, &image20,
							&image21, &image22};

/*******************************************************************************
*  Typedef
*******************************************************************************/

/*******************************************************************************
*  Local functions declarations
*******************************************************************************/
static void title_create (void);
static void title_cb (void *obj);
static void animation (lv_task_t* task);
/*******************************************************************************
*  Local functions definitions
*******************************************************************************/
static void title_create (void){

	lawn = lv_img_create(lv_scr_act(), NULL);
	lv_obj_align(lawn, NULL, LV_ALIGN_CENTER, -150, -75);
	lv_img_set_src(lawn, &grass);

	fly = lv_img_create(lawn, NULL);
	lv_obj_align(fly, NULL, LV_ALIGN_CENTER, 50, -30);

	label = lv_label_create(lv_scr_act(), NULL);
	lv_obj_align(label, lawn, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
	lv_label_set_text(label, "Developed by @Turunchuk");
	lv_label_set_style(label, LV_LABEL_STYLE_MAIN, fstyle_title());
}

static void animation (lv_task_t* task){
	static uint_fast8_t i = 0, j = 0;
	lv_img_set_src(fly, frames[i++]);
	if (i == 22) { i = 0; j++; }
	if (j == 5){
		lv_async_call(title_cb, NULL);
	}
}

static void title_cb (void *src){
	lv_task_del(title_task);
	lv_obj_clean(lawn);
	lv_obj_del(lawn);
	lv_obj_del(label);
	lv_img_cache_invalidate_src(NULL);
	menu_start();
}
/*******************************************************************************
*  Public functions definitions
*******************************************************************************/
void title_start (void){
	title_create();
	title_task = lv_task_create(animation, 70, LV_TASK_PRIO_LOW, NULL);
}

