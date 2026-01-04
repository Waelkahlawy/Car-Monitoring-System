#ifndef SENSOR_FUSION_TASK_H
#define SENSOR_FUSION_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "../../../../Cfg.h"

#if SENSOR_FUSION_ENABLED == STD_ON

void Sensor_Fusion_Task(void *pv);

#endif 

#endif 