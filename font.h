#ifndef font_h
#define font_h

#include <Arduino.h>
#include <unordered_map>

#include "image.h"

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
    int16_t xoffset;
    int16_t yoffset;
    int16_t xadvance;
    uint8_t page;
    uint8_t chnl;
};

template<class T1, class T2>
struct Pair {
    T1 first;
    T2 second;

    bool operator==(const Pair<T1, T2> &other) const {
        return (first == other.first) && (second == other.second);
    }
};

typedef Pair<uint32_t, uint32_t> IntPair;

struct IntHash {
    std::size_t operator() (const IntPair &p) const {
        return (p.first + p.second) + (p.first + p.second + 1) / 2 + p.first;
    }
};

class Font {
    
    FontInfo fontInfo;
    FontCommon fontCommon;

    // Though the descriptor defines id as a 32-bit integer, we will only use characters 0-255.
    std::unordered_map<uint32_t, FontChar> characters;
    std::unordered_map<IntPair, int16_t, IntHash> kerning;


    Image fontImage;
    float scale = 1.0f;

public:
    /**
     * Creates a font class with the specified data.
     * 
     * Variables containing data must be stored on PROGMEM.
     */
    Font(Image image, const unsigned char *descriptor);

    void setScale(float scale);

    FontChar getCharacter(char ch) const;
    int16_t getKerning(char first, char second) const;
    Pixel getPixel(int x, int y) const;

    // Font &operator=(const Font &other);

    int computeWidth(const char *text) const;

private:

    // Loading the font descriptor

    int parseBlock(const unsigned char *descriptor, int offset, int expectedBlock = -1);
    void parseBlock1(const unsigned char *descriptor, int offset, int size);
    void parseBlock2(const unsigned char *descriptor, int offset, int size);
    void parseBlock3(const unsigned char *descriptor, int offset, int size);
    void parseBlock4(const unsigned char *descriptor, int offset, int size);
    void parseBlock5(const unsigned char *descriptor, int offset, int size);
    bool verifyDescriptorSignature(const unsigned char *descriptor);
};

#endif
