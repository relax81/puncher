#pragma once

#include "U8g2lib.h"

extern U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2;

template <typename T>
void dispVar(int x, int y, T var) {
    u8g2.setCursor(x, y);
    u8g2.print(var);
}