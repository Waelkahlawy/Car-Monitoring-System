#ifndef PREDICTIVE_MAINTENANCE_TASK_H
#define PREDICTIVE_MAINTENANCE_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "../../../../Cfg.h"

#if PREDICTIVE_MAINTENANCE_ENABLED == STD_ON

void Predictive_Maintenance_Task(void *pv);

#endif /* PREDICTIVE_MAINTENANCE_ENABLED */

#endif /* PREDICTIVE_MAINTENANCE_TASK_H */