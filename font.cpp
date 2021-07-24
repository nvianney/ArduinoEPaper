#include "font.h"

#include <pgmspace.h>

Font::Font(Image image, const unsigned char *descriptor) : fontImage(image) {
    if (!verifyDescriptorSignature(descriptor)) {
        Serial.println("Error: invalid font descriptor file");
        return;
    }

    int offset = 4;
    offset = parseBlock(descriptor, offset, 1);
    offset = parseBlock(descriptor, offset, 2);
    offset = parseBlock(descriptor, offset, 3);
    offset = parseBlock(descriptor, offset, 4);
}

int Font::parseBlock(const unsigned char *descriptor, int offset, int expectedBlock) {
    byte type = pgm_read_byte(descriptor + offset + 0);
    int size = pgm_read_dword(descriptor + offset + 1);

    if (expectedBlock != -1) {
        if (type != expectedBlock) {
            Serial.println("Error: Unexpected block type. Expected is " + String(expectedBlock) + " but actual is " + String(type));
            return size;
        }
    }

    switch(type) {
        case 1:
            parseBlock1(descriptor, offset + 5, size);
            break;
        case 2: 
            parseBlock2(descriptor, offset + 5, size);
            break;
        case 3: 
            parseBlock3(descriptor, offset + 5, size);
            break;
        case 4: 
            parseBlock4(descriptor, offset + 5, size);
            break;
        case 5: 
            parseBlock5(descriptor, offset + 5, size);
            break;
        default:
            Serial.println("Error: unknown block type");
    }


    return offset + 5 + size;
}

FontChar Font::getCharacter(char ch) {
    return characters[ch];
}

Pixel Font::getPixel(int x, int y) {
    return fontImage.pixelAt(x, y);
}

// Font &Font::operator=(const Font &other) {
//     if (this == &other) return *this;
    
// }

void Font::parseBlock1(const unsigned char *descriptor, int offset, int size) {
    FontInfo info;

    info.fontSize = pgm_read_word(descriptor + offset + 0);
    info.bitField = pgm_read_byte(descriptor + offset + 2);
    info.charSet = pgm_read_byte(descriptor + offset + 3);
    info.stretchH = pgm_read_byte(descriptor + offset + 4);
    info.aa = pgm_read_byte(descriptor + offset + 6);
    info.paddingUp = pgm_read_byte(descriptor + offset + 7);
    info.paddingRight = pgm_read_byte(descriptor + offset + 8);
    info.paddingDown = pgm_read_byte(descriptor + offset + 9);
    info.paddingLeft = pgm_read_byte(descriptor + offset + 10);
    info.spacingHoriz = pgm_read_byte(descriptor + offset + 11);
    info.spacingVert = pgm_read_byte(descriptor + offset + 12);
    info.outline = pgm_read_byte(descriptor + offset + 13);

    // TODO: implement font name

    fontInfo = info;
}

void Font::parseBlock2(const unsigned char *descriptor, int offset, int size) {
    FontCommon common;

    common.lineHeight = pgm_read_word(descriptor + offset + 0);
    common.base = pgm_read_word(descriptor + offset + 2);
    common.scaleW = pgm_read_word(descriptor + offset + 4);
    common.scaleH = pgm_read_word(descriptor + offset + 6);
    common.pages = pgm_read_word(descriptor + offset + 8);
    common.bitField = pgm_read_byte(descriptor + offset + 10);
    common.alphaChnl = pgm_read_byte(descriptor + offset + 11);
    common.redChnl = pgm_read_byte(descriptor + offset + 12);
    common.greenChnl = pgm_read_byte(descriptor + offset + 13);
    common.blueChnl = pgm_read_byte(descriptor + offset + 14);

    fontCommon = common;
}

void Font::parseBlock3(const unsigned char *descriptor, int offset, int size) {
    // TODO
}

void Font::parseBlock4(const unsigned char *descriptor, int offset, int size) {
    int chars = size / 20;

    for (int c = 0; c < chars; c++) {
        FontChar ch;

        ch.id =         pgm_read_dword(descriptor + offset + 0 + c * 20);
        ch.x =          pgm_read_word(descriptor + offset + 4 + c * 20);
        ch.y =          pgm_read_word(descriptor + offset + 6 + c * 20);
        ch.width =      pgm_read_word(descriptor + offset + 8 + c * 20);
        ch.height =     pgm_read_word(descriptor + offset + 10 + c * 20);
        ch.xoffset =    pgm_read_word(descriptor + offset + 12 + c * 20);
        ch.yoffset =    pgm_read_word(descriptor + offset + 14 + c * 20);
        ch.xadvance =   pgm_read_word(descriptor + offset + 16 + c * 20);
        ch.page =       pgm_read_byte(descriptor + offset + 18 + c * 20);
        ch.chnl =       pgm_read_byte(descriptor + offset + 19 + c * 20);

        characters[(char) ch.id] = ch;
    }

}

void Font::parseBlock5(const unsigned char *descriptor, int offset, int size) {
}

bool Font::verifyDescriptorSignature(const unsigned char *descriptor) {
    if (pgm_read_byte(descriptor + 0) != 66) return false;
    if (pgm_read_byte(descriptor + 1) != 77) return false;
    if (pgm_read_byte(descriptor + 2) != 70) return false;
    if (pgm_read_byte(descriptor + 3) != 3) return false;
    return true;
}
