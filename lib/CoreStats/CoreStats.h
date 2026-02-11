/**
 * @file CoreStats.h
 * @brief This file defines the CoreStats available.
 */

#ifndef CORESTATS_H
#define CORESTATS_H

#include "Stats.h"
#include "Activities.h"
#include "FreeRTOSConfig.h"
#include <freertos/portmacro.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_timer.h>

#ifdef __cplusplus
extern "C" {
#endif

void StatsUpdate_Task(void* pvParameters);

#ifdef __cplusplus
}
#endif

#endif // CORESTATS_H