// Documentation for ink display: https://www.waveshare.com/w/upload/9/94/7.5inch_HD_e-Paper_B_Datasheet.pdf
// Similar documentation for different display, but with same commands but more clearer explanations:
//   https://cdn-learn.adafruit.com/assets/assets/000/092/748/original/SSD1675_0.pdf?1593792604
// Example implementation: https://github.com/waveshare/e-Paper/blob/master/Arduino/epd7in5b_HD/epd7in5b_HD.cpp

#include "eink_display.h"

EInkDisplay::Config::Config(int width, int height) : width(width), height(height) {}

EInkDisplay::Config::Config(int width, int height, int cs, int dc, int busy, int reset) : width(width), height(height), cs(cs), dc(dc), busy(busy), reset(reset) {}

EInkDisplay::EInkDisplay(EInkDisplay::Config config) : _config(config) {
}

void EInkDisplay::setup() {
    pinMode(_config.cs, OUTPUT);
    pinMode(_config.dc, OUTPUT);
    pinMode(_config.busy, INPUT);
    pinMode(_config.reset, OUTPUT);

    // Data writes are MSB first (D7 -> D0, D0 LSB)
    SPISettings settings = SPISettings(2000000, MSBFIRST, SPI_MODE0);
    SPI.begin();
    SPI.beginTransaction(settings);

    //reset();
    initialize();
}


void EInkDisplay::writeBuffer(uint8_t* buffer, bool black) {
    writeCommand(0x4E); // Reset RAM y-address to 0
    writeData(0x00);
    writeData(0x00);

    writeCommand(0x4F); // Reset RAM y-address to 0
    writeData(0x00);
    writeData(0x00);

    // Determine which RAM to write
    if (black) {
        writeCommand(0x24);
    } else {
        writeCommand(0x26);
    }

    for (uint32_t y = 0; y < _config.height; y++) { // in bits

        // A byte has data of 8 pixels
        for (uint32_t x = 0; x < _config.width / 8; x++) { // in bytes
            uint8_t data = buffer[(_config.width * y) / 8 + x];

            if (black) {
                writeData(~data);
            } else {
                writeData(data);
            }
        }
    }

}

void EInkDisplay::apply() {
    // Apply
    writeCommand(0x22); // display update control 2
    writeData(0xC7);

    writeCommand(0x20); // master activation

    waitNotBusy();
}

void EInkDisplay::writePartial(unsigned char* buffer, int bufX, int bufY, int bufWidth, int bufHeight, bool black) {

    // the address is in bits, but we're sending bytes of data at a time. we need to round down to nearest byte
    // to properly send data
    const int lowerByteX = bufX >> 3; // round down to nearest byte
    const int upperByteX = (bufX + bufWidth + 8 - 1) >> 3; // basically ceil((bufX+bufWidth) / 8)

    const int lowerBitX = lowerByteX << 3; // multiply by 8 for address offset (addr offset in bits)


    for (uint32_t y = bufY; y < bufY + bufHeight; y++) { // in bits

        writeCommand(0x4E); // Set RAM x-addr to start of min x
        writeData(lowerBitX & 0xFF);
        writeData((lowerBitX >> 8) & 0x03);

        writeCommand(0x4F); // Set RAM y-addr to start of min y
        writeData(y & 0xFF);
        writeData((y >> 8) & 0x03);

        // Determine which RAM to write
        if (black) {
            writeCommand(0x24);
        } else {
            writeCommand(0x26);
        }

        // A byte has data of 8 pixels
        for (uint32_t x = lowerByteX; x < upperByteX; x++) { // in bytes
            uint8_t data = buffer[(_config.width * y) / 8 + x];

            if (black) {
                writeData(~data);
            } else {
                writeData(data);
            }
        }
    }
}

void EInkDisplay::clear() {
    reset();
    initialize();

    writeCommand(0x4F); // reset RAM addr to 0
    writeData(0x00);
    writeData(0x00);

    writeCommand(0x24); // write to BW RAM
    for (uint32_t i = 0; i < _config.width * _config.height / 8; i++) {
        writeData(0xFF); // white
    }

    writeCommand(0x26); // write to red RAM top->down
    for (uint32_t i = 0; i < _config.width * _config.height / 8; i++) {
        writeData(0x00); // white
    }

    writeCommand(0x22); // display update control 2
    writeData(0xC7);

    writeCommand(0x20); // display
    delay(10);
    waitNotBusy();
}

void EInkDisplay::sleep() {
    writeCommand(0x10);
    writeData(0x03);
}

void EInkDisplay::wake() {
    reset();
    initialize();
}

void EInkDisplay::writeCommand(uint8_t command) {
    digitalWrite(_config.dc, LOW);
    transferSpi(command);
}

void EInkDisplay::writeData(uint8_t data) {
    digitalWrite(_config.dc, HIGH);
    transferSpi(data);
}

void EInkDisplay::transferSpi(uint8_t data) {
    digitalWrite(_config.cs, LOW);
    SPI.transfer(data);
    digitalWrite(_config.cs, HIGH);
}

void EInkDisplay::waitNotBusy() {
    do {
        delay(10); // 10 ms
    } while (digitalRead(_config.busy));
    delay(200);
}

void EInkDisplay::initialize() {
    writeCommand(0x12); // reset
    waitNotBusy();

    writeCommand(0x46); // Auto write for red
    writeData(0xF7);
    waitNotBusy();

    writeCommand(0x47); // Auto write for white
    writeData(0xF7);
    waitNotBusy();

    writeCommand(0x0C); // Soft start setting
    writeData(0xAE);
    writeData(0xC7);
    writeData(0xC3);
    writeData(0xC0);
    writeData(0x40);   

    writeCommand(0x01); // Set MUX as 527
    writeData(0x0F);
    writeData(0x02);
    writeData(0x00);

    writeCommand(0x11); // Data entry
    writeData(0x03);    // x+, y+, x+ increment

    writeCommand(0x44); // Start/end pos of RAM x
    writeData(0x00); // start at 0
    writeData(0x00);
    writeData(0x6F); // end at 36F_h = 879
    writeData(0x03);

    writeCommand(0x45); // Start/end pos of RAM y
    writeData(0x00);    // start at 0
    writeData(0x00);
    writeData(0x0F);    // end at 20F_h = 527
    writeData(0x02);

    writeCommand(0x3C); // VBD
    writeData(0x01);    // LUT1, for white

    writeCommand(0x18); // Temperature sensor
    writeData(0X80);    // internal

    writeCommand(0x22); // BinaryMatrix update control 2
    writeData(0XB1);	// Load Temperature and waveform setting.

    writeCommand(0x20); // Master activation
    waitNotBusy();

    writeCommand(0x4E); // Initial RAM x
    writeData(0x00);    // addr 0
    writeData(0x00);
    writeCommand(0x4F); // Initial RAM y
    writeData(0x00);    // addr 0
    writeData(0x00);
}

void EInkDisplay::reset() {
    digitalWrite(_config.reset, LOW);
    delay(4);
    digitalWrite(_config.reset, HIGH);
    delay(200);
}

