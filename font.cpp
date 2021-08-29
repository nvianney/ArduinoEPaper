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
    offset = parseBlock(descriptor, offset, 5);
}

void Font::setScale(float scale) {
    this->scale = scale;
    fontImage.setScale(scale);
}

FontChar Font::getCharacter(char ch) const {
    if (characters.find(ch) == characters.end()) {
        Serial.println("Character not found: " + ch);
        return FontChar();
    }

    if (this->scale == 1.0f) return characters.at(ch);

    // We have to manipulate the character with the new scaling
    FontChar fc = characters.at(ch);
    fc.x *= scale;
    fc.y *= scale;
    fc.width *= scale;
    fc.height *= scale;
    fc.xoffset *= scale;
    fc.yoffset *= scale;
    fc.xadvance *= scale;

    return fc;
}

int16_t Font::getKerning(char first, char second) const {
    if (kerning.find( (IntPair) {first, second} ) == kerning.end()) {
        return 0;
    }

    return kerning.at( (IntPair) {first, second} ) * scale;
}

Pixel Font::getPixel(int x, int y) const {
    return fontImage.pixelAt(x, y);
}

uint16_t Font::getLineHeight() const {
    return fontCommon.lineHeight * scale;
}

int Font::computeWidth(const char *text) const {
    int width = 0;

    String str = String(text);
    for (int i = 0; i < str.length(); i++) {
        FontChar fc = getCharacter(text[i]);
        width += fc.xadvance;
    }

    return width;
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

        characters[ch.id] = ch;
    }

}

void Font::parseBlock5(const unsigned char *descriptor, int offset, int size) {
    int chars = size / 10;

    for (int c = 0; c < chars; c++) {
        uint32_t first =    pgm_read_dword(descriptor + offset + 0 + c * 10);
        uint32_t second =   pgm_read_dword(descriptor + offset + 4 + c * 10);
        int16_t amount =   pgm_read_word(descriptor + offset + 8 + c * 10);

        kerning[ {first, second} ] = amount;
    }
}

bool Font::verifyDescriptorSignature(const unsigned char *descriptor) {
    if (pgm_read_byte(descriptor + 0) != 66) return false;
    if (pgm_read_byte(descriptor + 1) != 77) return false;
    if (pgm_read_byte(descriptor + 2) != 70) return false;
    if (pgm_read_byte(descriptor + 3) != 3) return false;
    return true;
}
