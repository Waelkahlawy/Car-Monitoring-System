// #include "aggregate_task.h"
// #include "../../System/RTOS_MANAGER/rtos_manager.h"
// #include "../../System/system_type.h"
// #include "../../Modules_Driver/MQTT//mqtt.h"
// #include "esp_log.h"


// void Aggregation_Task(void *pv)
// {
//     SensorMsg_t msg;
//     char payload[256];

//     while (1)
//     {
//         while (xQueueReceive(SensorQueue, &msg, portMAX_DELAY) == pdPASS)
//         {
//             memset(payload, 0, sizeof(payload));

//             switch (msg.src)
//             {
//                 case SRC_DHT11:
//                     snprintf(payload, sizeof(payload),
//                              "{\"temp\":%d,\"hum\":%d}",
//                              msg.temperature, msg.humidity);
                             
//                       ESP_LOGI("Publish", "payload = %s\n",payload); // Log Trigger pin initialization
                 
//                     Mqtt_Publish(DHT11_TOPIC, payload, 1, 0);
//                     break;

//                 case SRC_IMU:
//                     snprintf(payload, sizeof(payload),
//                              "{\"ax\":%.2f,\"ay\":%.2f,\"az\":%.2f,\"gx\":%.2f,\"gy\":%.2f,\"gz\":%.2f,\"imu_temp\":%.2f}",
//                              msg.accel_x, msg.accel_y, msg.accel_z,
//                              msg.gyro_x, msg.gyro_y, msg.gyro_z,msg.imu_temp);
                             
//                           ESP_LOGI("Publish", "payload = %s\n",payload); // Log Trigger pin initialization
                 
//                     Mqtt_Publish(IMU_TOPIC, payload, 1, 0);
//                     break;

//                 case SRC_ULTRASONIC:
//                     snprintf(payload, sizeof(payload),
//                              "{\"distance\":%.2f}", msg.distance);
//                             ESP_LOGI("Publish", "payload = %s\n",payload); // Log Trigger pin initialization
                 
//                     Mqtt_Publish(MQTT_TOPIC_ESP1_ULTRASONIC, payload, 1, 0);
//                     break;

//                 case SRC_LDR:
//                     snprintf(payload, sizeof(payload),
//                              "{\"light\":%lu}", msg.light);
//                           ESP_LOGI("Publish", "payload = %s\n",payload); // Log Trigger pin initialization
                 
//                     Mqtt_Publish(MQTT_TOPIC_ESP1_LDR, payload, 1, 0);
//                     break;

//                 case SRC_GPS:
//                     snprintf(payload, sizeof(payload),
//                              "{\"lat\":%.6f,\"lon\":%.6f,\"sat\":%d}",
//                              msg.latitude, msg.longitude, msg.satellites);
//                          ESP_LOGI("Publish", "payload = %s\n",payload); // Log Trigger pin initialization
                 
//                     Mqtt_Publish(GPS_TOPIC, payload, 1, 0);
//                     break;

//                 case SRC_GSM:
//                     snprintf(payload, sizeof(payload),
//                              "{\"GSM_Alert\":%s}", msg.gsm_alert);
//                               ESP_LOGI("Publish", "payload = %s\n",payload); // Log Trigger pin initialization
                 
//                     Mqtt_Publish("car/sensors/gsm", payload, 1, 0);
//                     break;

//                 case SRC_FUSION:
//                     snprintf(payload, sizeof(payload),
//                              "{\"fused_lat\":%.6f,\"fused_lon\":%.6f,\"vel_x\":%.2f,\"vel_y\":%.2f}",
//                              msg.fused_lat, msg.fused_lon, 
//                              msg.velocity_x, msg.velocity_y);       
//                     ESP_LOGI("Publish", "payload = %s\n", payload);
//                     Mqtt_Publish(FUSION_TOPIC, payload, 1, 0);
//                     break;

//                 case SRC_PRED_MAINT:
//                 snprintf(payload, sizeof(payload),
//                          "{\"status\":\"%s\",\"confidence\":%.2f,\"rms\":%.3f,\"kurt\":%.3f,\"temp\":%.2f}",
//                          msg.pred_fault ? "FAULTY" : "HEALTHY",
//                          msg.pred_confidence,
//                          msg.pred_rms_vibration,
//                          msg.pred_kurtosis,
//                          msg.pred_median_temp);
//                 ESP_LOGI("Publish", "payload = %s\n", payload);
//                 Mqtt_Publish(PRED_MAINT_TOPIC, payload, 1, 0);
//                     break;
                
//                 default:


//                     break;
                    
//             }
//         }
//     }
// }
// /*
// void Aggregation_Task(void *pv)
// {
//     AggregatedData_t agg_data;
//     SensorMsg_t msg;

//     while(1)
//     {
//         // Reset aggregation count each cycle
//         agg_data.count = 0;

//         // Collect all messages currently in the queue
//         while(uxQueueMessagesWaiting(SensorQueue) > 0 && agg_data.count < MAX_SENSOR_COUNT)
//         {
//             if(xQueueReceive(SensorQueue, &msg, 0) == pdPASS)
//             {
//                 agg_data.sensors[agg_data.count++] = msg;
//             }
//         }

//         // Publish aggregated data via MQTT
//         if(agg_data.count > 0)
//         {
//             // Example: create JSON payload
//             char payload[512];
//             int offset = 0;
//             offset += snprintf(payload + offset, sizeof(payload) - offset, "{");

//             for(uint8_t i = 0; i < agg_data.count; i++)
//             {
//                 SensorMsg_t *s = &agg_data.sensors[i];
//                 switch(s->src)
//                 {
//                     case SRC_DHT11:
//                         offset += snprintf(payload + offset, sizeof(payload) - offset,
//                                            "\"DHT11_%d\":{\"temp\":%d,\"hum\":%d},",
//                                            i, s->temperature, s->humidity);
//                         break;
//                     case SRC_IMU:
//                         offset += snprintf(payload + offset, sizeof(payload) - offset,
//                                            "\"IMU_%d\":{\"ax\":%d,\"ay\":%d,\"az\":%d,\"gx\":%d,\"gy\":%d,\"gz\":%d},",
//                                            i, s->accel_x, s->accel_y, s->accel_z, s->gyro_x, s->gyro_y, s->gyro_z);
//                         break;
//                     case SRC_ULTRASONIC:
//                         offset += snprintf(payload + offset, sizeof(payload) - offset,
//                                            "\"ULTRASONIC_%d\":{\"distance\":%.2f},",
//                                            i, s->distance);
//                         break;
//                     case SRC_LDR:
//                         offset += snprintf(payload + offset, sizeof(payload) - offset,
//                                            "\"LDR_%d\":{\"light\":%lu},", i, s->light);
//                         break;
//                     case SRC_GPS:
//                         offset += snprintf(payload + offset, sizeof(payload) - offset,
//                                            "\"GPS_%d\":{\"lat\":%.6f,\"lon\":%.6f,\"alt\":%.1f},",
//                                            i, s->latitude, s->longitude, s->altitude);
//                         break;
//                     case SRC_GSM:
//                         offset += snprintf(payload + offset, sizeof(payload) - offset,
//                                            "\"GSM_%d\":{\"rssi\":%u},", i, s->gsm_rssi);
//                         break;
//                 }
//             }

//             // Remove trailing comma and close JSON
//             if(offset > 1 && payload[offset - 1] == ',') offset--;
//             snprintf(payload + offset, sizeof(payload) - offset, "}");
               
        
//                  ESP_LOGI(TAG_ULTRASONIC, "payload = %s\n",payload); // Log Trigger pin initialization
                 
//             // Publish
//             Mqtt_Publish("car/sensors/aggregate", payload, 1, 0);
//         }

//         // Wait for next aggregation cycle
//         vTaskDelay(pdMS_TO_TICKS(AGGREGATION_PERIOD_MS));
//     }
// }
// */