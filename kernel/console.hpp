#pragma once

#include "graphics.hpp"

class Console
{
public:
    static const int kRows = 25, kColumns = 80;

    Console(PixelWriter &writer, const PixelColor &fg_color, const PixelColor &bg_color);
    void PutString(const char *s);

private:
    void NewLine();

    PixelWriter &writer_;
    const PixelColor fg_color_, bg_color_;
    char buffer_[kRows][kColumns + 1]; // Put '\0' at the end of line, so buffer_ needs kColumns + 1 columns
    int cursor_row_, cursor_column_;
};
