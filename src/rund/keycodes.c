#include <rund/keycodes.h>

uint8_t keycode_to_ascii(uint8_t keycode, uint8_t *ascii)
{
    switch (keycode)
    {
    case RK_A: strcpy(ascii, "A"); break;
    case RK_B: strcpy(ascii, "B"); break;
    case RK_C: strcpy(ascii, "C"); break;
    case RK_D: strcpy(ascii, "D"); break;
    case RK_E: strcpy(ascii, "E"); break;
    case RK_F: strcpy(ascii, "F"); break;
    case RK_G: strcpy(ascii, "G"); break;
    case RK_H: strcpy(ascii, "H"); break;
    case RK_I: strcpy(ascii, "I"); break;
    case RK_J: strcpy(ascii, "J"); break;
    case RK_K: strcpy(ascii, "K"); break;
    case RK_L: strcpy(ascii, "L"); break;
    case RK_M: strcpy(ascii, "M"); break;
    case RK_N: strcpy(ascii, "N"); break;
    case RK_O: strcpy(ascii, "O"); break;
    case RK_P: strcpy(ascii, "P"); break;
    case RK_Q: strcpy(ascii, "Q"); break;
    case RK_R: strcpy(ascii, "R"); break;
    case RK_S: strcpy(ascii, "S"); break;
    case RK_T: strcpy(ascii, "T"); break;
    case RK_U: strcpy(ascii, "U"); break;
    case RK_V: strcpy(ascii, "V"); break;
    case RK_W: strcpy(ascii, "W"); break;
    case RK_X: strcpy(ascii, "X"); break;
    case RK_Y: strcpy(ascii, "Y"); break;
    case RK_Z: strcpy(ascii, "Z"); break;
    case RK_a: strcpy(ascii, "a"); break;
    case RK_b: strcpy(ascii, "b"); break;
    case RK_c: strcpy(ascii, "c"); break;
    case RK_d: strcpy(ascii, "d"); break;
    case RK_e: strcpy(ascii, "e"); break;
    case RK_f: strcpy(ascii, "f"); break;
    case RK_g: strcpy(ascii, "g"); break;
    case RK_h: strcpy(ascii, "h"); break;
    case RK_i: strcpy(ascii, "i"); break;
    case RK_j: strcpy(ascii, "j"); break;
    case RK_k: strcpy(ascii, "k"); break;
    case RK_l: strcpy(ascii, "l"); break;
    case RK_m: strcpy(ascii, "m"); break;
    case RK_n: strcpy(ascii, "n"); break;
    case RK_o: strcpy(ascii, "o"); break;
    case RK_p: strcpy(ascii, "p"); break;
    case RK_q: strcpy(ascii, "q"); break;
    case RK_r: strcpy(ascii, "r"); break;
    case RK_s: strcpy(ascii, "s"); break;
    case RK_t: strcpy(ascii, "t"); break;
    case RK_u: strcpy(ascii, "u"); break;
    case RK_v: strcpy(ascii, "v"); break;
    case RK_w: strcpy(ascii, "w"); break;
    case RK_x: strcpy(ascii, "x"); break;
    case RK_y: strcpy(ascii, "y"); break;
    case RK_z: strcpy(ascii, "z"); break;
    case RK_0: strcpy(ascii, "0"); break;
    case RK_1: strcpy(ascii, "1"); break;
    case RK_2: strcpy(ascii, "2"); break;
    case RK_3: strcpy(ascii, "3"); break;
    case RK_4: strcpy(ascii, "4"); break;
    case RK_5: strcpy(ascii, "5"); break;
    case RK_6: strcpy(ascii, "6"); break;
    case RK_7: strcpy(ascii, "7"); break;
    case RK_8: strcpy(ascii, "8"); break;
    case RK_9: strcpy(ascii, "9"); break;
    
    case RK_space: strcpy(ascii, " "); break;
    
    default:
        ascii[0] = '\0';
    }

    
    return strlen(ascii);
}