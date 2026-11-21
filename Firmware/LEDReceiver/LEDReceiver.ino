#include <Adafruit_NeoPixel.h>

// Once everyone relevant to this particular system has read through this and is
// okay with the new representation and addressing scheme, we can get rid of the
// other sketches. 

const int LED_PIN = 3;
const int NUM_LEDS = 30;
const unsigned long BAUD_RATE = 115200;

Adafruit_NeoPixel strip(
    NUM_LEDS, 
    LED_PIN, 
    NEO_GRB + NEO_KHZ800
);

enum States {
    WAIT_AA,
    WAIT_55,
    READ_COUNT_LOW,
    READ_COUNT_HIGH,
    READ_PIXELS,
    READ_CHECKSUM
};

struct Pixel {
    uint8_t on;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

States state = WAIT_AA;

uint16_t count = 0;
uint16_t pixelByteCount;
uint8_t checkSum;

Pixel pixelData[NUM_LEDS];

void setup () {
    Serial.begin(BAUD_RATE);

    strip.begin();
    strip.setBrightness(50);
    strip.clear();
    strip.show();
}

void loop () {
    while (Serial.available() > 0) {
        uint8_t incomingByte = Serial.read();
        switch (state) {
            case WAIT_AA:
                if (incomingByte == 0xAA) {
                    state = WAIT_55;
                }
                break;
            case WAIT_55:
                if (incomingByte == 0x55) {
                    state = READ_COUNT_LOW;
                } else if (incomingByte != 0xAA) {
                    state = WAIT_AA;
                }
                break;
            case READ_COUNT_LOW:
                count = incomingByte;
                state = READ_COUNT_HIGH;
                break;
            case READ_COUNT_HIGH:
                count += (incomingByte << 8);
                if (count != NUM_LEDS) {
                    state = WAIT_AA;
                } else {
                    checkSum = 0;
                    checkSum += static_cast<uint8_t>(count & 0xFF);
                    checkSum += static_cast<uint8_t>(count >> 8);

                    pixelByteCount = 0;
                    state = READ_PIXELS;
                }
                break;
            case READ_PIXELS: {
                uint8_t pixelIndex = pixelByteCount / 4;
                uint8_t byteIndex = pixelByteCount % 4;

                switch (byteIndex) {
                    case 0:
                        pixelData[pixelIndex].on = incomingByte;
                        checkSum += incomingByte;
                        break;
                    case 1:
                        pixelData[pixelIndex].red = incomingByte;
                        checkSum += incomingByte;
                        break;
                    case 2:
                        pixelData[pixelIndex].green = incomingByte;
                        checkSum += incomingByte;
                        break;
                    case 3:
                        pixelData[pixelIndex].blue = incomingByte;
                        checkSum += incomingByte;
                        break;
                }
                pixelByteCount++;
                if (pixelByteCount == count * 4) {
                    state = READ_CHECKSUM;
                }
                break;
            }
            case READ_CHECKSUM:
                if (incomingByte == checkSum) {
                    for (uint16_t i = 0; i < count; i++) {
                        if (pixelData[i].on == 1) {
                            strip.setPixelColor(
                                i,
                                pixelData[i].red,
                                pixelData[i].green,
                                pixelData[i].blue
                            );
                        } else {
                            strip.setPixelColor(i, 0, 0, 0);
                        }
                    }
                    strip.show();
                }
                state = WAIT_AA;
                break;
        }
    }
}