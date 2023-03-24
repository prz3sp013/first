/*
 */
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/uart.h"

#include "esp_log.h"
// #include "led_strip.h"
#include "sdkconfig.h"

#define LED_GPIO_1 2
#define LED_GPIO_2 13

TaskHandle_t task1handle;
TaskHandle_t task2handle;

void task1(void *arg)
{
    gpio_set_direction(LED_GPIO_1, GPIO_MODE_OUTPUT);
    uint8_t sw = 1;
    uint8_t licznik = 0;
    uint16_t dat;
    while (1)
    {
        gpio_set_level(LED_GPIO_1, sw);
        sw ^= 1;

        vTaskDelay(pdMS_TO_TICKS(500));
        dat = uxTaskGetStackHighWaterMark(NULL);
        printf("[stack: %d] - Tas%s : %d \n", dat, (char *)arg, licznik++);
        if (sw == 0)
        {
            printf("LED1 -- OFF\n");
        }
        else
        {
            printf("LED1 -- ON\n");
        }
    }
}

void task2(void *arg)
{
    gpio_set_direction(LED_GPIO_2, GPIO_MODE_OUTPUT);
    uint8_t sw = 1;          // zmienna dla diod led
    uint8_t licznik = 0;     // licznik ilości wykonanej tasku
    uint16_t dat;            // wolne miejsce pozostałe na stosie
    uint8_t kill_count = 10; // licznik gdy 0 zabij task 1
    while (1)
    {
        gpio_set_level(LED_GPIO_2, sw);
        sw ^= 1;

        vTaskDelay(pdMS_TO_TICKS(500));
        dat = uxTaskGetStackHighWaterMark(NULL);
        printf("[stack: %d] - Tas%s : %d \n", dat, (char *)arg, licznik++);

        if (sw == 0)
        {
            printf("LED1 -- OFF\n");
        }
        else
        {
            printf("LED1 -- ON\n");
        }

        if (kill_count)
        {
            kill_count--;
            if (5 == kill_count)
            {
                vTaskDelete(task1handle);
            }
            if (!kill_count)
            {
                vTaskDelete(NULL);
            }
        }
    }
}

void app_main()
{
    uint8_t kill_count_main = 4;
    uart_set_baudrate(0, 115200);
    xTaskCreate(task1, "task1", 2048, "TASK 1", 1, &task1handle);
    xTaskCreate(task2, "task2", 2048, "TASK 2", 1, &task2handle);
    while (1)
    {
        kill_count_main--;
        printf("main task : %d \n", kill_count_main);
        if (!kill_count_main)
        {
            vTaskDelete(NULL);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}