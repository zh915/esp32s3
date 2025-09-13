#ifndef __EXCITED_LOOP_FRAMES_H__
#define __EXCITED_LOOP_FRAMES_H__

#include <stdint.h>

#define FRAME_WIDTH 240
#define FRAME_HEIGHT 240
#define EXCITED_LOOP_FRAME_COUNT 8

extern const uint16_t excited_loop_frame_0[FRAME_WIDTH * FRAME_HEIGHT];
extern const uint16_t excited_loop_frame_1[FRAME_WIDTH * FRAME_HEIGHT];
extern const uint16_t excited_loop_frame_2[FRAME_WIDTH * FRAME_HEIGHT];
extern const uint16_t excited_loop_frame_3[FRAME_WIDTH * FRAME_HEIGHT];
extern const uint16_t excited_loop_frame_4[FRAME_WIDTH * FRAME_HEIGHT];
extern const uint16_t excited_loop_frame_5[FRAME_WIDTH * FRAME_HEIGHT];
extern const uint16_t excited_loop_frame_6[FRAME_WIDTH * FRAME_HEIGHT];
extern const uint16_t excited_loop_frame_7[FRAME_WIDTH * FRAME_HEIGHT];

extern const uint16_t* excited_loop_frames[8];
#endif
