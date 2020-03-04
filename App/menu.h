/*
 * tile.h
 *
 *  Created on: 22 янв. 2020 г.
 *      Author: ALSCode
 */

#ifndef MENU_H_
#define MENU_H_

#pragma once
#include "main.h"
#include "app_common.h"

#if DATA_ANALYZER
#include "dw.h"
#endif

#if CALENDAR
#include "calendar.h"
#endif

#if MESSENGER
#include "messenger.h"
#endif

#if BOOK
#include "book.h"
#endif

#if FOTO
#include "foto.h"
#endif

#endif /* MENU_H_ */
