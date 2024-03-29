#include <cstddef>
#include <cstdint>
#include <cstdio>
#include "frame_buffer_config.hpp"
#include "graphics.hpp"
#include "font.hpp"
#include "console.hpp"
#include "pci.hpp"
#include "logger.hpp"
#include "mouse.hpp"

#include "usb/memory.hpp"
#include "usb/device.hpp"
#include "usb/classdriver/mouse.hpp"
#include "usb/xhci/xhci.hpp"
#include "usb/xhci/trb.hpp"

const PixelColor kDesktopBGColor{45, 118, 237};
const PixelColor kDesktopFGColor{255, 255, 255};

char pixel_writer_buf[sizeof(RGBResv8BitPerColorPixelWriter)];
PixelWriter *pixel_writer;
char console_buf[sizeof(Console)];
Console *console;
char mouse_cursor_buf[sizeof(MouseCursor)];
MouseCursor *mouse_cursor;


void SwitchEhci2XhCi(const pci::Device &xhc_dev)
{
    bool intel_ehc_exist = false;

    for (int i = 0; i < pci::num_device; ++i)
    {
        if (pci::devices[i].class_code.Match(0x0cu, 0x03u, 0x20u) /* EHCI*/ && pci::ReadVendorId(pci::devices[i]) == 0x8086)
        {
            intel_ehc_exist = true;
            break;
        }
    }
    if (!intel_ehc_exist)
    {
        Log(kInfo, "SwitchEhci2Xhci: No Intel EHCI\n");
        return;
    }
    uint32_t superspeed_ports = pci::ReadConfReg(xhc_dev, 0xdc); // USB3PRM
    pci::WriteConfReg(xhc_dev, 0xd8, superspeed_ports);          // USB3_PSSEN
    uint32_t ehci2xhci_ports = pci::ReadConfReg(xhc_dev, 0xd4);  // XUSB2PRM
    pci::WriteConfReg(xhc_dev, 0xd0, ehci2xhci_ports);           // XUSB2PR
    Log(kDebug, "SwitchEhci2Xhci: SS = %02, xHCI = %02x\n", superspeed_ports, ehci2xhci_ports);
}

void MouseObserver(int8_t displacement_x, int8_t displacement_y)
{
    mouse_cursor->MoveRelative({displacement_x, displacement_y});
}

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

    const int kFrameWidth = frame_buffer_config.horizontal_resolution;
    const int kFrameHeight = frame_buffer_config.vertical_resolution;

    FillRectangle(*pixel_writer,
                  {0, 0},
                  {kFrameWidth, kFrameHeight - 50},
                  kDesktopBGColor);
    FillRectangle(*pixel_writer,
                  {0, kFrameHeight - 50},
                  {kFrameWidth, 50},
                  {1, 8, 17});
    FillRectangle(*pixel_writer,
                  {0, kFrameHeight - 50},
                  {kFrameWidth / 5, 50},
                  {80, 80, 80});
    DrawRectangle(*pixel_writer,
                  {10, kFrameHeight - 40},
                  {30, 30},
                  {160, 160, 160});
    console = new (console_buf) Console{
        *pixel_writer, kDesktopFGColor, kDesktopBGColor};

    Printk("Welcome to MikanOS!\n");

    SetLogLevel(kDebug);


    SetLogLevel(kInfo);
    mouse_cursor = new (mouse_cursor_buf) MouseCursor{pixel_writer, kDesktopBGColor, {300, 200}};

    auto err = pci::ScanAllBus();
    Log(kInfo, "ScanAllBus: %s\n", err.Name());

    for (int i = 0; i < pci::num_device; ++i)
    {
        const auto &dev = pci::devices[i];
        auto vendor_id = pci::ReadVendorId(dev.bus, dev.device, dev.function);
        auto class_code = pci::ReadClassCode(dev.bus, dev.device, dev.function);
        Log(kInfo, "%d.%d.%d: vend %04x, class %08x, head %02x\n", dev.bus, dev.device, dev.function, vendor_id, class_code, dev.header_type);
    }

    // Search xHC with a preference for made by Intel
    pci::Device *xhc_dev = nullptr;
    for (int i = 0; i < pci::num_device; ++i)
    {
        if (pci::devices[i].class_code.Match(0x0c, 0x03u, 0x30u))
        {
            xhc_dev = &pci::devices[i];
            if (0x8086 == pci::ReadVendorId(*xhc_dev))
                break;
        }
    }
    if (xhc_dev)
    {
        Log(kInfo, "xHC has been found: %d.%d.%d\n", xhc_dev->bus, xhc_dev->device, xhc_dev->function);
    }
    const WithError<uint64_t> xhc_bar = pci::ReadBar(*xhc_dev, 0);
    Log(kDebug, "ReadBar: %s\n", xhc_bar.error.Name());
    const uint64_t xhc_mmio_base = xhc_bar.value & ~static_cast<uint64_t>(0xf);
    Log(kDebug, "xHC mmio_base: %08lx\n", xhc_mmio_base);

    usb::xhci::Controller xhc{xhc_mmio_base};
    if (pci::ReadVendorId(*xhc_dev) == 0x8086) // Intel
    {
        SwitchEhci2XhCi(*xhc_dev);
    }
    {
        auto err = xhc.Initialize();
        Log(kDebug, "xhc.Initialize: %s\n", err.Name());
    }

    Log(kInfo, "xHC starting\n");
    xhc.Run();

    usb::HIDMouseDriver::default_observer = MouseObserver;

    for (int i = 1; i <= xhc.MaxPorts(); ++i)
    {
        auto port = xhc.PortAt(i);
        Log(kDebug, "Port %d: isConnected=%d\n", i, port.IsConnected());
        if (port.IsConnected())
        {
            if (auto err = ConfigurePort(xhc, port))
            {
                Log(kError, "failed to configure port: %s at %s:%d\n", err.Name(), err.File(), err.Line());
                continue;
            }
        }
    }

    while (1)
        if (auto err = ProcessEvent(xhc))
        {
            Log(kError, "Error while ProcessEvent: %s at %s:%d\n", err.Name(), err.File(), err.Line());
        }

    while (1)
        __asm__("hlt");
}

extern "C" void __cxa_pure_virtual()
{
    while (1)
        __asm__("hlt");
}
