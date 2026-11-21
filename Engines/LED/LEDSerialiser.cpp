#include "LEDSerialiser.h"

#include <cstdint>


// Serialising our LEDFrame in the following format:
// Header: 0xAA 0x55
// Frame Size: 2 bytes (little endian)
// Pixel Data: 4 bytes per pixel (on, r, g, b)
// Checksum: 1 byte (sum of all bytes in the frame, excluding header and checksum)

std::vector<uint8_t> serialiseLEDFrame(const LEDFrame& frame) {
    std::vector<uint8_t> bytes;
    uint8_t checkSum = 0x00;
    uint16_t frameSize = static_cast<uint16_t>(frame.size()); // 16-bit is overkill for our 144 LEDs, but better practice anyway
    checkSum += static_cast<uint8_t>(frameSize & 0xFF);
    checkSum += static_cast<uint8_t>(frameSize >> 8);

    bytes.push_back(0xAA);
    bytes.push_back(0x55);
    bytes.push_back(static_cast<uint8_t>(frameSize & 0xFF));
    bytes.push_back(static_cast<uint8_t>(frameSize >> 8)); // using little endian

    for (const auto& pixel : frame) {
        bytes.push_back(pixel.on);
        checkSum += pixel.on;
        bytes.push_back(pixel.r);
        checkSum += pixel.r;
        bytes.push_back(pixel.g);
        checkSum += pixel.g;
        bytes.push_back(pixel.b);
        checkSum += pixel.b;
    }
    bytes.push_back(checkSum);
    return bytes;
}