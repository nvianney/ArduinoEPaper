#include "renderer.h"


Renderer::Renderer(int width, int height) :
    redData(BinaryMatrix(width, height)),
    blackData(BinaryMatrix(width, height)),
    display(EInkDisplay(EInkDisplay::Config(width, height))) {
    clearBounds();

    display.setup();

}

void Renderer::drawRect(int x, int y, int width, int height) {
    updateBounds(x, y, x + width, y + height);

    data().setRect(x, y, width, borderWidth, true);                         // top border
    data().setRect(x, y + height - borderWidth, width, borderWidth, true);  // bottom border

    data().setRect(x, y, borderWidth, height, true);                        // left border
    data().setRect(x + width - borderWidth, y, borderWidth, height, true);  // right border
}

void Renderer::fillRect(int x, int y, int width, int height) {
    updateBounds(x, y, x + width, y + height);

    data().setRect(x, y, width, height, 1);
}

void Renderer::drawText(int x, int y, const char *text) {
    if (font == nullptr) {
        Serial.println("Error: font not set");
        return;
    }

    int textX = x;
    int maxHeight = 0;

    int len = String(text).length();
    for (int i = 0; i < len; i++) {
        FontChar c = font->getCharacter(text[i]);
        maxHeight = max((int) c.height + c.yoffset, maxHeight);

        for (int oy = 0; oy < c.height; oy++) {
            for (int ox = 0; ox < c.width; ox++) {
                Pixel p = font->getPixel(c.x + ox, c.y + oy);
                bool active = p.b > 0;

                int outputX = textX + ox;
                int outputY = y + oy;

                data().setPixel(outputX + c.xoffset, outputY + c.yoffset, active);
            }
        }

        textX += c.xadvance;
    }

    updateBounds(x, y, textX, y + maxHeight);
}

void Renderer::setFont(Font &font) {
    this->font = &font;
}

void Renderer::setColor(DisplayColor color) {
    this->color = color;
}

void Renderer::clearAll() {
    blackData.clear();
    redData.clear();
    display.clear();
}

void Renderer::render() {
    if (!dirty) return;
    dirty = false;

    int x = _minX;
    int y = _minY;
    int width = _maxX - _minX;
    int height = _maxY - _minY;

    display.writePartial(blackData.buffer, x, y, width, height, true);
    display.writePartial(redData.buffer, x, y, width, height, false);
    // display.writeBuffer(blackData.buffer, true);
    // display.writeBuffer(redData.buffer, false);

    display.apply();

    clearBounds();
}

void Renderer::updateBounds(int minX, int minY, int maxX, int maxY) {
    _minX = min(minX, _minX);
    _minY = min(minY, _minY);
    _maxX = max(maxX, _maxX);
    _maxY = max(maxY, _maxY);
    dirty = true;
}

void Renderer::clearBounds() {
    _minX = INT_MAX;
    _minY = INT_MAX;
    _maxX = 0;
    _maxY = 0;

}

BinaryMatrix &Renderer::data() {
    if (color == DisplayColor::BLACK) {
        return blackData;
    } else if (color == DisplayColor::RED) {
        return redData;
    } else {
        Serial.println("Error: unknown display color");
    }
}

