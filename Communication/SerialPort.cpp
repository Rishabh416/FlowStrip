#include "SerialPort.h"

#include <stdexcept>
#include <string>

SerialPort::SerialPort(const char* portName) {
    handle = CreateFileA(
        portName,
        GENERIC_WRITE,
        0,
        nullptr,
        OPEN_EXISTING,
        0,
        nullptr
    );

    if (handle == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Failed to open serial port");
    }

    DCB settings = {};
    settings.DCBlength = sizeof(DCB);

    if (!GetCommState(handle, &settings)) {
        CloseHandle(handle);
        handle = INVALID_HANDLE_VALUE;
        throw std::runtime_error("Failed to read serial settings");
    }

    settings.BaudRate = CBR_115200;
    settings.ByteSize = 8;
    settings.Parity = NOPARITY;
    settings.StopBits = ONESTOPBIT;
    settings.fBinary = TRUE;
    settings.fOutxCtsFlow = FALSE;
    settings.fOutxDsrFlow = FALSE;
    settings.fOutX = FALSE;
    settings.fInX = FALSE;

    if (!SetCommState(handle, &settings)) {
        CloseHandle(handle);
        handle = INVALID_HANDLE_VALUE;
        throw std::runtime_error("Failed to configure serial port");
    }

    COMMTIMEOUTS timeouts = {};
    timeouts.WriteTotalTimeoutConstant = 1000;

    if (!SetCommTimeouts(handle, &timeouts)) {
        CloseHandle(handle);
        handle = INVALID_HANDLE_VALUE;
        throw std::runtime_error("Failed to configure serial timeouts");
    }
}

SerialPort::~SerialPort() {
    if (handle != INVALID_HANDLE_VALUE) {
        CloseHandle(handle);
    }
}

void SerialPort::send(const std::vector<uint8_t>& data) {
    std::size_t offset = 0;

    while (offset < data.size()) {
        DWORD written = 0;
        DWORD remaining = static_cast<DWORD>(data.size() - offset);

        if (!WriteFile(
                handle,
                data.data() + offset,
                remaining,
                &written,
                nullptr
            ) || written == 0) {
            throw std::runtime_error("Failed to write serial data");
        }

        offset += written;
    }
}
