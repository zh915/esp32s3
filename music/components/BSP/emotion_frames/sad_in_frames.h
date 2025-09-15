#ifndef __SAD_IN_FRAMES_H__
#define __SAD_IN_FRAMES_H__

#include <stdint.h>

#define FRAME_WIDTH 240
#define FRAME_HEIGHT 240
#define SAD_IN_FRAME_COUNT 8

extern const uint16_t sad_in_frame_0[FRAME_WIDTH * FRAME_HEIGHT];
extern const uint16_t sad_in_frame_1[FRAME_WIDTH * FRAME_HEIGHT];
extern const uint16_t sad_in_frame_2[FRAME_WIDTH * FRAME_HEIGHT];
extern const uint16_t sad_in_frame_3[FRAME_WIDTH * FRAME_HEIGHT];
extern const uint16_t sad_in_frame_4[FRAME_WIDTH * FRAME_HEIGHT];
extern const uint16_t sad_in_frame_5[FRAME_WIDTH * FRAME_HEIGHT];
extern const uint16_t sad_in_frame_6[FRAME_WIDTH * FRAME_HEIGHT];
extern const uint16_t sad_in_frame_7[FRAME_WIDTH * FRAME_HEIGHT];

extern const uint16_t* sad_in_frames[8];
#endif
