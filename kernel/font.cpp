#include "font.hpp"
extern const uint8_t _binary_hankaku_bin_start;
extern const uint8_t _binary_hankaku_bin_end;
extern const uint8_t _binary_hankaku_bin_size;

const uint8_t *GetFont(char c)
{
    auto index = 16 * static_cast<unsigned int>(c);
    if (index >= reinterpret_cast<uintptr_t>(&_binary_hankaku_bin_size))
        return nullptr;
    return &_binary_hankaku_bin_start + index;
}

void WriteASCii(PixelWriter &writer, int x, int y, char c, const PixelColor &color)
{
    const uint8_t *font = GetFont(c);
    if (font == nullptr)
        return;
    for (int dy = 0; dy < 16; ++dy)
        for (int dx = 0; dx < 8; ++dx)
        {
            // font[dy] << dx => row: dy, column: dx
            // - 0x80u
            //   - u: unsigned int
            //   - 0x80: 0b1000 0000 => only the top bit is 1
            if ((font[dy] << dx) & 0x80u) // Whether the font[dy][dx] bit is 1 or not
                writer.Write(x + dx, y + dy, color);
        }
}

void WriteString(PixelWriter &writer, int x, int y, const char *s, const PixelColor &color)
{
    for (int i = 0; s[i] != '\0'; ++i)
    {
        WriteASCii(writer, x + 8 * i, y, s[i], color);
    }
}
