#include "display.h"

Display::Display(uint16_t width, uint16_t height) {
    buffer = new uint8_t[width * height / 8];
}

Display::~Display() {
    delete buffer;
}

void Display::setPixel(uint16_t x, uint16_t y, bool value) {

}

bool Display::getPixel(uint16_t x, uint16_t y) {

}

inline int Display::loc(uint16_t x, uint16_t y) {
    return ((int) height * y) / 8 + x;
}
