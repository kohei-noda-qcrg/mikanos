#include "font.hpp"

// 8x16 A
const uint8_t kFontA[16] = {
    0b00000000, //
    0b00011000, //    **
    0b00011000, //    **
    0b00011000, //    **
    0b00011000, //    **
    0b00100100, //   *  *
    0b00100100, //   *  *
    0b00100100, //   *  *
    0b00100100, //   *  *
    0b01111110, //  ******
    0b01000010, //  *    *
    0b01000010, //  *    *
    0b01000010, //  *    *
    0b11100111, // ***  ***
    0b00000000, //
    0b00000000, //
};

void WriteASCii(PixelWriter &writer, int x, int y, char c, const PixelColor &color)
{
    if (c != 'A')
        return;
    for (int dy = 0; dy < 16; ++dy)
        for (int dx = 0; dx < 8; ++dx)
        {
            // kFontA[dy] << dx => row: dy, column: dx
            // - 0x80u
            //   - u: unsigned int
            //   - 0x80: 0b1000 0000 => only the top bit is 1
            if ((kFontA[dy] << dx) & 0x80u) // Whether the kFontA[dy][dx] bit is 1 or not
                writer.Write(x + dx, y + dy, color);
        }
}
