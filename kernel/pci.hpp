#pragma once

#include <cstdint>

#include "error.hpp"

namespace pci
{
    /** @brief CONFIG_ADDRESS register I/O port address*/
    const uint16_t kConfigAddress = 0x0cf8;
    /** @brief CONFIG_DATA register I/O port address*/
    const uint16_t kConfigData = 0x0cfc;

    /** @brief Class code for PCI device*/
    struct ClassCode
    {
        uint8_t base, sub, interface;

        /** @brief Returns true when 2 base classes are equivalent.*/
        bool Match(uint8_t b) { return b == base; }
        /** @brief Returns true when 2 base classes and sub classes are equivalent.*/
        bool Match(uint8_t b, uint8_t s) { return Match(b) && s == sub; }
        /** @brief Returns true when 2 base classes and sub classes and interfaces are equivalent.*/
        bool Match(uint8_t b, uint8_t s, uint8_t i) { return Match(b, s) && i == interface; }
    };

    /** @brief Store the basic data to oparate a PCI device.
     *
     * bus, device, function are required to specify the device.
     *
     */
    struct Device
    {
        uint8_t bus, device, function, header_type;
        ClassCode class_code;
    };

    /** @brief Write specified integer to kConfigAddress*/
    void WriteAddress(uint32_t address);
    /** @brief Write specified integer to kConfigData*/
    void WriteData(uint32_t value);
    /** @brief Read 32bit integer from CONFIG_DATA register*/
    uint32_t ReadData();

    /** @brief Read the vendor ID register (for All header type)*/
    uint16_t ReadVendorId(uint8_t bus, uint8_t device, uint8_t function);
    /** @brief Read the device ID register (for All header type)*/
    uint16_t ReadDeviceId(uint8_t bus, uint8_t device, uint8_t function);
    /** @brief Read the header type register (for All header type)*/
    uint8_t ReadHeaderType(uint8_t bus, uint8_t device, uint8_t function);
    /** @brief Read the class code register (for All header type)
     *
     * The following is the structure of the returned 32-bit integer
     *   - 32:24 : Base class
     *   - 23:16 : Sub class
     *   - 15:8  : Interface
     *   - 7:0   : Revision ID
     */
    uint32_t ReadClassCode(uint8_t bus, uint8_t device, uint8_t function);

    /** @brief Read the bus number register (for header type 1)
     *
     * The following is the structure of the returned 32-bit integer
     *   - 32:16 : Sub ordinate bus number
     *   - 15:8  : Secondary bus number
     *   - 7:0   : Revision ID
     */
    uint32_t ReadBusNumbers(uint8_t bus, uint8_t device, uint8_t function);

    /** @brief Returns true for a single function*/
    bool IsSingleFunctionDevice(uint8_t header_type);

    /** @brief All PCI devices found by the ScanAllBus() function*/
    inline std::array<Device, 32> devices;
    /** @brief The number of valid elements of devices*/
    inline int num_device;

    /** @brief Search all PCI devices and save to devices
     *
     * Scan PCI devices recursively from bus 0 and save to devices
     * Then set the number of devices found to num_devices
     */
    Error ScanAllBus();
}
