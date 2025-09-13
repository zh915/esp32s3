#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "lcd.h"
#include "spi.h"
#include "iic.h"
#include "xl9555.h"
#include "led.h"

// LED控制相关定义
#define MY_LED_ON()    LED(1)     // 打开LED
#define MY_LED_OFF()   LED(0)     // 关闭LED

// 表情动画帧头文件
#include "excited_in_frames.h"
#include "excited_loop_frames.h"
#include "excited_out_frames.h"
#include "sad_in_frames.h"
#include "sad_loop_frames.h"
#include "sad_out_frames.h"

/**
 * @brief 分块写入一帧 RGB565 数据，防止 SPI 传输过长报错
 */
void lcd_draw_frame(const uint16_t *frame)
{
    const size_t chunk_size = 4096;
    size_t total_size = FRAME_WIDTH * FRAME_HEIGHT * 2;
    const uint8_t *data = (const uint8_t *)frame;

    for (size_t offset = 0; offset < total_size; offset += chunk_size)
    {
        size_t len = (offset + chunk_size <= total_size) ? chunk_size : (total_size - offset);
        lcd_write_data(data + offset, len);
    }
}

/**
 * @brief 播放一段动画
 * 
 * @param frames      帧指针数组
 * @param frame_count 帧数
 * @param delay_ms    每帧延时(ms)
 * @param loop        是否循环播放
 * @param loop_count  循环次数（仅在 loop 为 true 时有效）
 */
void play_expression(const uint16_t **frames, int frame_count, int delay_ms, bool loop, int loop_count)
{
    int current_loop = 0;
    do {
        for (int i = 0; i < frame_count; i++)
        {
            lcd_set_window(0, 0, FRAME_WIDTH - 1, FRAME_HEIGHT - 1);
            lcd_draw_frame(frames[i]);
            vTaskDelay(pdMS_TO_TICKS(delay_ms));
        }
        if (loop) {
            current_loop++;
        }
    } while (loop && (current_loop < loop_count));
}

/**
 * @brief LED闪烁任务
 */
void led_blink_task(void *pvParameters)
{
    while(1) {
        LED_TOGGLE();  // 使用led.h中定义的LED_TOGGLE宏
        vTaskDelay(pdMS_TO_TICKS(500));  // 500ms闪烁频率
    }
}

/**
 * @brief 主程序入口
 */
void app_main(void)
{
    // 初始化外设
    spi2_init();        // SPI2 初始化
    
    // 初始化I2C和XL9555用于控制背光
    i2c_obj_t i2c0 = iic_init(I2C_NUM_0);
    xl9555_init(i2c0);
    
    led_init();         // LED初始化
    lcd_init();         // LCD 初始化
    vTaskDelay(pdMS_TO_TICKS(100));  // 等待LCD稳定
    
    // 初始显示测试
    lcd_clear(RED);     // 先显示红色测试背光
    vTaskDelay(pdMS_TO_TICKS(1000));
    lcd_clear(BLACK);   // 清屏为黑色
    vTaskDelay(pdMS_TO_TICKS(100));

    // LED闪烁任务句柄
    TaskHandle_t led_task_handle = NULL;
    
    while (1)
    {
        // 🎉 兴奋表情 - LED常亮
        MY_LED_OFF();  // LED常亮
        if (led_task_handle != NULL) {
            vTaskDelete(led_task_handle);  // 如果闪烁任务存在，停止它
            led_task_handle = NULL;
        }
        
        play_expression(excited_in_frames, EXCITED_IN_FRAME_COUNT, 100, false, 0);
        play_expression(excited_loop_frames, EXCITED_LOOP_FRAME_COUNT, 100, true, 3);  // 循环3次

        // 等待 3 秒后退出循环
        vTaskDelay(pdMS_TO_TICKS(300));
        play_expression(excited_out_frames, EXCITED_OUT_FRAME_COUNT, 100, false, 0);

        // 😢 难过表情 - LED闪烁
        // 创建LED闪烁任务
        xTaskCreate(
            (TaskFunction_t)led_blink_task,
            "led_blink",
            2048,
            NULL,
            5,
            &led_task_handle
        );
        
        play_expression(sad_in_frames, SAD_IN_FRAME_COUNT, 100, false, 0);
        play_expression(sad_loop_frames, SAD_LOOP_FRAME_COUNT, 100, true, 3);  // 循环3次

        // 等待 3 秒后退出循环
        vTaskDelay(pdMS_TO_TICKS(300));
        play_expression(sad_out_frames, SAD_OUT_FRAME_COUNT, 100, false, 0);
    }
}
