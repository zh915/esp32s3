#ifndef __EMOTION_PLAY_H__
#define __EMOTION_PLAY_H__

#include "excited_in_frames.h"
#include "excited_loop_frames.h"
#include "excited_out_frames.h"
#include "sad_in_frames.h"
#include "sad_loop_frames.h"
#include "sad_out_frames.h"
#include "lcd.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// 动画播放任务句柄
extern TaskHandle_t emotion_task_handle;
extern SemaphoreHandle_t lcd_mutex;
// 播放开心表情动画
void play_happy_emotion(void);

// 播放难过表情动画
void play_sad_emotion(void);

// 停止动画播放
void stop_emotion_task(void);

#endif