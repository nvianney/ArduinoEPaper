#ifndef eink_display_h
#define eink_display_h

#include <SPI.h>
#include <Arduino.h>

class EInkDisplay {

    // Inner structs/classes
public:
    struct Config {
        int width;
        int height;

        int cs = 26;
        int dc = 25;
        int busy = 33;
        int reset = 32;

        Config(int width, int height);
        Config(int width, int height, int cs, int dc, int busy, int reset);
    };

    // Class properties
private:
    EInkDisplay::Config _config;

    // Methods
public:
    EInkDisplay(EInkDisplay::Config config);

    void setup();

    void writeBuffer(unsigned char* buffer, bool black);
    void writePartial(unsigned char* buffer, int bufX, int bufY, int bufWidth, int bufHeight, bool black);

    void clear();

private:
    void writeCommand(uint8_t command);
    void writeData(uint8_t data);

    void transferSpi(uint8_t data);

    void waitNotBusy();

    void reset();

};

#endif
