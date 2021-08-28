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

    data().setRect(x, y, width, borderWidth, pixelValue);                         // top border
    data().setRect(x, y + height - borderWidth, width, borderWidth, pixelValue);  // bottom border

    data().setRect(x, y, borderWidth, height, pixelValue);                        // left border
    data().setRect(x + width - borderWidth, y, borderWidth, height, pixelValue);  // right border
}

void Renderer::fillRect(int x, int y, int width, int height) {
    updateBounds(x, y, x + width, y + height);

    data().setRect(x, y, width, height, pixelValue);
}

void Renderer::fillRoundRect(int x, int y, int width, int height, int radius) {
    int a = (width / 2) - radius;
    int b = (height / 2) - radius;

    for (int row = y; row <= y + height; row++) {
        for (int col = x; col <= x + width; col++) {
            int cx = max(abs(col - (x + width/2)) - a, 0);
            int cy = max(abs(row - (y + height/2)) - b, 0);
            if (cx * cx + cy * cy <= radius * radius) {
                data().setPixel(col, row, pixelValue);
            }
        }
    }

    updateBounds(x, y, x + width * 2, y + height * 2);;
}

void Renderer::fillCircle(int centerX, int centerY, int radius) {
    for (int y = centerY - radius; y <= centerY + radius; y++) {
        for (int x = centerX - radius; x <= centerX + radius; x++) {
            float cx = x - centerX;
            float cy = y - centerY;
            if (cx * cx + cy * cy <= radius * radius) {
                data().setPixel(x, y, pixelValue);
            }
        }
    }

    updateBounds(centerX - radius, centerY - radius, centerX + radius, centerY + radius);
}

void Renderer::drawLine(int x1, int y1, int x2, int y2) {
    // Find equation for y
    float m = ((float) (y2 - y1)) / ((float) (x2 - x1));

    // We'll draw on multiple axes to prevent blank spots on a line due to steep slope

    // Draw by giving input x
    int startX = min(x1, x2);
    int endX = max(x1, x2);
    for (int x = startX; x <= endX; x++) {
        int y = m * (x - x1) + y1;

        data().setPixel(x, y, pixelValue);
    }

    // Draw by giving input y
    int startY = min(y1, y2);
    int endY = max(y1, y2);
    for (int y = startY; y <= endY; y++) {
        int x = (float) (y - y1) / m + x1;

        data().setPixel(x, y, pixelValue);
    }

    updateBounds(x1, y1, x2, y2);
}

void Renderer::drawImage(Image &image, int x, int y) {
    for (int r = 0; r < image.height * image.getScale(); r++) {
        for (int c = 0; c < image.width * image.getScale(); c++) {
            Pixel p = image.pixelAt(c, r);
            data().setPixel(x + c, y + r, (p.b <= 1) & pixelValue);
        }
    }

    updateBounds(x, y, x + image.width, x + image.height);
}

void Renderer::drawText(int x, int y, const char *text, TextAlignment align) {
    if (font == nullptr) {
        Serial.println("Error: font not set");
        return;
    }

    int startX;

    switch(align) {
        case TextAlignment::LEFT:
            startX = x;
            break;
        case TextAlignment::CENTER:
            startX = x - font->computeWidth(text)/2;
            break;
        case TextAlignment::RIGHT:
            startX = x - font->computeWidth(text);
            break;
    }

    int textX = startX;
    int maxHeight = 0;

    int len = String(text).length();
    for (int i = 0; i < len; i++) {
        FontChar c = font->getCharacter(text[i]);
        maxHeight = max((int) c.height + c.yoffset, maxHeight);

        // Apply kerning
        int kerning = 0;
        if (i >= 1) {
            kerning = font->getKerning(text[i-1], text[i]);
        }
        textX += kerning;

        for (int oy = 0; oy < c.height; oy++) {
            for (int ox = 0; ox < c.width; ox++) {
                Pixel p = font->getPixel(c.x + ox, c.y + oy);
                bool active = p.b <= 1;

                int outputX = textX + ox;
                int outputY = y + oy;

                if (active) {
                    data().setPixel(outputX + c.xoffset, outputY + c.yoffset, active & pixelValue);
                }
            }
        }

        textX += c.xadvance;
    }

    updateBounds(startX, y, textX, y + maxHeight);
}

void Renderer::setFont(Font *font) {
    this->font = font;
}

Font *Renderer::getFont() const {
    return this->font;
}

void Renderer::setColor(DisplayColor color) {
    this->color = color;
}

void Renderer::clearAll() {
    blackData.clear();
    redData.clear();
    display.clear();
}

void Renderer::setDrawMode() {
    pixelValue = true;
}

void Renderer::setClearMode() {
    pixelValue = false;
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

