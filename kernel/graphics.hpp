#pragma once

#include "frame_buffer_config.hpp"

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
    virtual void Write(int x, int y, const PixelColor &c) override;
};

class BGRResv8BitPerColorPixelWriter : public PixelWriter
{
public:
    using PixelWriter::PixelWriter;
    virtual void Write(int x, int y, const PixelColor &c) override;
};

template <typename T>
struct Vector2D
{
    T x, y;
};

void FillRectangle(PixelWriter &writer, const Vector2D<int> &pos, const Vector2D<int> &size, const PixelColor &c);
void DrawRectangle(PixelWriter &writer, const Vector2D<int> &pos, const Vector2D<int> &size, const PixelColor &c);
