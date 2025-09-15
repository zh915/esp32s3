#include "emotion_play.h"

TaskHandle_t emotion_task_handle = NULL;
static volatile bool is_playing = false;
SemaphoreHandle_t lcd_mutex = NULL;


// 分块绘制一帧（避免SPI传输过长）
static void lcd_draw_frame(const uint16_t *frame)
{
    const size_t chunk_size = 4096;
    size_t total_size = FRAME_WIDTH * FRAME_HEIGHT * 2; // RGB565格式，每个像素2字节
    const uint8_t *data = (const uint8_t *)frame;

    for (size_t offset = 0; offset < total_size; offset += chunk_size)
    {
        size_t len = (offset + chunk_size <= total_size) ? chunk_size : (total_size - offset);
        lcd_write_data(data + offset, len);
    }
}

// 动画播放通用函数
static void play_expression(const uint16_t **frames, int frame_count, int delay_ms) {
    for (int i = 0; i < frame_count && is_playing; i++) {
        // 申请LCD控制权（最多等待100ms，避免死等）
        if (xSemaphoreTake(lcd_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            lcd_set_window(0, 0, FRAME_WIDTH - 1, FRAME_HEIGHT - 1);
            lcd_draw_frame(frames[i]);
            xSemaphoreGive(lcd_mutex);  // 释放控制权
        }
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}

// 开心动画任务
static void happy_emotion_task(void *param)
{
    is_playing = true;
        // 1. 播放入场帧（必须执行完，不响应退出）
    play_expression((const uint16_t **)excited_in_frames, EXCITED_IN_FRAME_COUNT, 100);
    
    // 2. 循环播放（响应退出信号）
    while (is_playing) {
        play_expression((const uint16_t **)excited_loop_frames, EXCITED_LOOP_FRAME_COUNT, 100);
        // 每次循环后检查退出信号，避免卡帧
        if (!is_playing) break;
    }
    
    // 3. 播放退场帧（必须执行完，保证动画完整性）
    play_expression((const uint16_t **)excited_out_frames, EXCITED_OUT_FRAME_COUNT, 100);
    
    // 4. 任务退出前重置状态
    is_playing = false;
    emotion_task_handle = NULL;  // 关键：标记任务已退出
    vTaskDelete(NULL);
}

// 难过动画任务
static void sad_emotion_task(void *param)
{
    is_playing = true;
    // 播放入场→循环→退场完整流程
    play_expression((const uint16_t **)sad_in_frames, SAD_IN_FRAME_COUNT, 100);
    while (is_playing) // 循环播放，直到音频结束
    {
        play_expression((const uint16_t **)sad_loop_frames, SAD_LOOP_FRAME_COUNT, 100);
    }
    play_expression((const uint16_t **)sad_out_frames, SAD_OUT_FRAME_COUNT, 100);
    is_playing = false;
    emotion_task_handle = NULL;
    vTaskDelete(NULL);
}

// 启动开心动画
void play_happy_emotion(void)
{
    stop_emotion_task(); // 先停止已有动画
    xTaskCreate(
        happy_emotion_task,
        "happy_emotion",
        8192,
        NULL,
        3, // 优先级低于音频任务
        &emotion_task_handle
    );
}

// 启动难过动画
void play_sad_emotion(void)
{
    stop_emotion_task(); // 先停止已有动画
    xTaskCreate(
        sad_emotion_task,
        "sad_emotion",
        8192,
        NULL,
        3, // 优先级低于音频任务
        &emotion_task_handle
    );
}

// 停止动画任务

void stop_emotion_task(void) {
    if (emotion_task_handle != NULL) {
        is_playing = false;  // 通知任务退出循环
        // 等待任务自行删除（最多等待100ms，避免死等）
        for (int i = 0; i < 10; i++) {
            if (emotion_task_handle == NULL) {
                break;  // 任务已退出，跳出循环
            }
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        // 若超时未退出，强制删除任务（避免资源泄漏）
        if (emotion_task_handle != NULL) {
            vTaskDelete(emotion_task_handle);
            emotion_task_handle = NULL;
            printf("强制删除旧动画任务\n");
        }
    }
}