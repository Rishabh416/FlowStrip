#pragma once

#include <cstdint>
#include <vector>
#include <windows.h>

class SerialPort {
private:
    HANDLE handle = INVALID_HANDLE_VALUE;

public:
    explicit SerialPort(const char* portName);
    ~SerialPort();

    void send(const std::vector<uint8_t>& data);

    SerialPort(const SerialPort&) = delete;
    SerialPort& operator=(const SerialPort&) = delete;
};
