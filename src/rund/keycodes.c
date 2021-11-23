#include <rund/keycodes.h>

#include <string.h>

char keycode_to_ascii(uint16_t keycode)
{
    switch (keycode)
    {
    case RK_A: return 'A';
    case RK_B: return 'B';
    case RK_C: return 'C';
    case RK_D: return 'D';
    case RK_E: return 'E';
    case RK_F: return 'F';
    case RK_G: return 'G';
    case RK_H: return 'H';
    case RK_I: return 'I';
    case RK_J: return 'J';
    case RK_K: return 'K';
    case RK_L: return 'L';
    case RK_M: return 'M';
    case RK_N: return 'N';
    case RK_O: return 'O';
    case RK_P: return 'P';
    case RK_Q: return 'Q';
    case RK_R: return 'R';
    case RK_S: return 'S';
    case RK_T: return 'T';
    case RK_U: return 'U';
    case RK_V: return 'V';
    case RK_W: return 'W';
    case RK_X: return 'X';
    case RK_Y: return 'Y';
    case RK_Z: return 'Z';
    case RK_a: return 'a';
    case RK_b: return 'b';
    case RK_c: return 'c';
    case RK_d: return 'd';
    case RK_e: return 'e';
    case RK_f: return 'f';
    case RK_g: return 'g';
    case RK_h: return 'h';
    case RK_i: return 'i';
    case RK_j: return 'j';
    case RK_k: return 'k';
    case RK_l: return 'l';
    case RK_m: return 'm';
    case RK_n: return 'n';
    case RK_o: return 'o';
    case RK_p: return 'p';
    case RK_q: return 'q';
    case RK_r: return 'r';
    case RK_s: return 's';
    case RK_t: return 't';
    case RK_u: return 'u';
    case RK_v: return 'v';
    case RK_w: return 'w';
    case RK_x: return 'x';
    case RK_y: return 'y';
    case RK_z: return 'z';
    case RK_0: return '0';
    case RK_1: return '1';
    case RK_2: return '2';
    case RK_3: return '3';
    case RK_4: return '4';
    case RK_5: return '5';
    case RK_6: return '6';
    case RK_7: return '7';
    case RK_8: return '8';
    case RK_9: return '9';
    
    case RK_space: return ' ';
    }
    
    return '\0';
}

wchar_t keycode_to_unicode(uint16_t keycode)
{
    switch (keycode)
    {
    case RK_A: return L'A';
    case RK_B: return L'B';
    case RK_C: return L'C';
    case RK_D: return L'D';
    case RK_E: return L'E';
    case RK_F: return L'F';
    case RK_G: return L'G';
    case RK_H: return L'H';
    case RK_I: return L'I';
    case RK_J: return L'J';
    case RK_K: return L'K';
    case RK_L: return L'L';
    case RK_M: return L'M';
    case RK_N: return L'N';
    case RK_O: return L'O';
    case RK_P: return L'P';
    case RK_Q: return L'Q';
    case RK_R: return L'R';
    case RK_S: return L'S';
    case RK_T: return L'T';
    case RK_U: return L'U';
    case RK_V: return L'V';
    case RK_W: return L'W';
    case RK_X: return L'X';
    case RK_Y: return L'Y';
    case RK_Z: return L'Z';
    case RK_a: return L'a';
    case RK_b: return L'b';
    case RK_c: return L'c';
    case RK_d: return L'd';
    case RK_e: return L'e';
    case RK_f: return L'f';
    case RK_g: return L'g';
    case RK_h: return L'h';
    case RK_i: return L'i';
    case RK_j: return L'j';
    case RK_k: return L'k';
    case RK_l: return L'l';
    case RK_m: return L'm';
    case RK_n: return L'n';
    case RK_o: return L'o';
    case RK_p: return L'p';
    case RK_q: return L'q';
    case RK_r: return L'r';
    case RK_s: return L's';
    case RK_t: return L't';
    case RK_u: return L'u';
    case RK_v: return L'v';
    case RK_w: return L'w';
    case RK_x: return L'x';
    case RK_y: return L'y';
    case RK_z: return L'z';
    case RK_0: return L'0';
    case RK_1: return L'1';
    case RK_2: return L'2';
    case RK_3: return L'3';
    case RK_4: return L'4';
    case RK_5: return L'5';
    case RK_6: return L'6';
    case RK_7: return L'7';
    case RK_8: return L'8';
    case RK_9: return L'9';
    
    case RK_space: return L' ';
    }
    
    return L'\0';
}
