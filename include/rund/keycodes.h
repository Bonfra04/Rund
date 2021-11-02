#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RK_A        0x00
#define RK_B        0x01
#define RK_C        0x02
#define RK_D        0x03
#define RK_E        0x04
#define RK_F        0x05
#define RK_G        0x06
#define RK_H        0x07
#define RK_I        0x08
#define RK_J        0x09
#define RK_K        0x0A
#define RK_L        0x0B
#define RK_M        0x0C
#define RK_N        0x0D
#define RK_O        0x0E
#define RK_P        0x0F
#define RK_Q        0x10
#define RK_R        0x11
#define RK_S        0x12
#define RK_T        0x13
#define RK_U        0x14
#define RK_V        0x15
#define RK_W        0x16
#define RK_X        0x17
#define RK_Y        0x18
#define RK_Z        0x19
#define RK_a        0x1A
#define RK_b        0x1B
#define RK_c        0x1C
#define RK_d        0x1D
#define RK_e        0x1E
#define RK_f        0x1F
#define RK_g        0x20
#define RK_h        0x21
#define RK_i        0x22
#define RK_j        0x23
#define RK_k        0x24
#define RK_l        0x25
#define RK_m        0x26
#define RK_n        0x27
#define RK_o        0x28
#define RK_p        0x29
#define RK_q        0x2A
#define RK_r        0x2B
#define RK_s        0x2C
#define RK_t        0x2D
#define RK_u        0x2E
#define RK_v        0x2F
#define RK_w        0x30
#define RK_x        0x31
#define RK_y        0x32
#define RK_z        0x33
#define RK_0        0x34
#define RK_1        0x35
#define RK_2        0x36
#define RK_3        0x37
#define RK_4        0x38
#define RK_5        0x39
#define RK_6        0x3A
#define RK_7        0x3B
#define RK_8        0x3C
#define RK_9        0x3D

#define RK_space    0x3E

uint8_t keycode_to_ascii(uint8_t keycode, uint8_t *ascii);

#ifdef __cplusplus
extern "C" }
#endif
