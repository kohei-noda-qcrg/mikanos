#include <cstddef>
#include <cstdint>
#include "frame_buffer_config.hpp"

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

struct PixelColor
{
    uint8_t r, g, b;
};

class PixelWriter
{
public:
    PixelWriter(const FrameBufferConfig &config) : config_{config} {} // Constructor, copy config to config_ member
    virtual ~PixelWriter() = default;                                 // Destructor
    virtual void Write(int x, int y, const PixelColor &c) = 0;        // = 0; → Pure virtual function

protected:
    uint8_t *PixelAt(int x, int y)
    {
        // 4 byte per pixel
        return config_.frame_buffer + 4 * (config_.pixels_per_scan_line * y + x);
    }

private:
    const FrameBufferConfig &config_;
};

class RGBResv8BitPerColorPixelWriter : public PixelWriter
{
public:
    using PixelWriter::PixelWriter;

    virtual void Write(int x, int y, const PixelColor &c) override
    {
        auto p = PixelAt(x, y);
        p[0] = c.r;
        p[1] = c.g;
        p[2] = c.b;
        // p[3] is reserved byte
    }
};

class BGRResv8BitPerColorPixelWriter : public PixelWriter
{
public:
    using PixelWriter::PixelWriter;

    virtual void Write(int x, int y, const PixelColor &c) override
    {
        auto p = PixelAt(x, y);
        p[0] = c.b;
        p[1] = c.g;
        p[2] = c.r;
        // p[3] is reserved byte
    }
};

void *operator new(size_t size, void *buf)
{
    return buf;
}

void operator delete(void *obj) noexcept
{
}

char pixel_writer_buf[sizeof(RGBResv8BitPerColorPixelWriter)];
PixelWriter *pixel_writer;

extern "C" void KernelMain(const FrameBufferConfig &frame_buffer_config)
{
    switch (frame_buffer_config.pixel_format)
    {
    case kPixelRGBResv8BitPerColor:
        pixel_writer = new (pixel_writer_buf) RGBResv8BitPerColorPixelWriter{frame_buffer_config};
        break;
    case kPixelBGRResv8BitPerColor:
        pixel_writer = new (pixel_writer_buf) BGRResv8BitPerColorPixelWriter{frame_buffer_config};
        break;
    }
    for (int x = 0; x < frame_buffer_config.horizontal_resolution; ++x)
        for (int y = 0; y < frame_buffer_config.vertical_resolution; ++y)
            pixel_writer->Write(x, y, {255, 255, 255});
    for (int x = 0; x < 200; ++x)
        for (int y = 0; y < 100; ++y)
            pixel_writer->Write(x, y, {0, 255, 0});
    while (1)
        __asm__("hlt");
}
