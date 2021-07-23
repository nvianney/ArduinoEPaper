#ifndef font_h
#define font_h

#include <Arduino.h>
#include <unordered_map>

struct FontInfo {
    uint16_t fontSize;
    uint8_t bitField;
    uint8_t charSet;
    uint16_t stretchH;
    uint8_t aa;
    uint8_t paddingUp;
    uint8_t paddingRight;
    uint8_t paddingDown;
    uint8_t paddingLeft;
    uint8_t spacingHoriz;
    uint8_t spacingVert;
    uint8_t outline;
    char *fontName;
};

struct FontCommon {
    uint16_t lineHeight;
    uint16_t base;
    uint16_t scaleW;
    uint16_t scaleH;
    uint16_t pages;
    uint8_t bitField;
    uint8_t alphaChnl;
    uint8_t redChnl;
    uint8_t greenChnl;
    uint8_t blueChnl;
};

struct FontChar {
    uint32_t id;
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    uint16_t xoffset;
    uint16_t yoffset;
    uint16_t xadvance;
    uint8_t page;
    uint8_t chnl;
};

class Font {

    FontInfo fontInfo;
    FontCommon fontCommon;

    // Though the descriptor defines id as a 32-bit integer, we will only use characters 0-127.
    std::unordered_map<char, FontChar> characters;

public:
    /**
     * Creates a font class with the specified data.
     * 
     * Variables containing data must be stored on PROGMEM.
     */
    Font(const void *image, const unsigned char *descriptor);

private:
    int parseBlock(const unsigned char *descriptor, int offset, int expectedBlock = -1);
    void parseBlock1(const unsigned char *descriptor, int offset, int size);
    void parseBlock2(const unsigned char *descriptor, int offset, int size);
    void parseBlock3(const unsigned char *descriptor, int offset, int size);
    void parseBlock4(const unsigned char *descriptor, int offset, int size);
    void parseBlock5(const unsigned char *descriptor, int offset, int size);

    bool verifyDescriptorSignature(const unsigned char *descriptor);
};

#endif
