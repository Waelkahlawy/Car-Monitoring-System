#ifndef RTOS_MANAGER_H
#define RTOS_MANAGER_H

/* ================= Includes ================= */
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

/* ================= System Events ================= */

/* Ultrasonic */
#define EVENT_DOOR_PROXIMITY     (1 << 0)
/* IMU / Engine */
#define EVENT_ENGINE_VIBRATION   (1 << 1)
#define EVENT_OVER_TEMP          (1 << 2)
/* Light */
#define EVENT_LOW_LIGHT          (1 << 3)
/* Temperature & Humidity */
#define EVENT_HIGH_TEMP          (1 << 4)
#define EVENT_HIGH_HUM           (1 << 5)
/* GPS */
#define EVENT_GPS_FIX            (1 << 6)
#define EVENT_GPS_LOST           (1 << 7)
#define EVENT_ENGINE_FAULT      (1 << 8)


/* ================= Shared Queues ================= */
// extern QueueHandle_t SensorQueue;

extern QueueHandle_t GsmAlertQueue;

extern SemaphoreHandle_t UartMutex;

/* ================= Event Groups ================= */
extern EventGroupHandle_t NotificationEventGroup;

/* ================= RTOS Init API ================= */
void RTOS_Manager_Init(void);

#endif /* RTOS_MANAGER_H */

