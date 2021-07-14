#ifndef binary_matrix_h
#define binary_matrix_h

#include <Arduino.h>

struct BinaryMatrix {
    uint8_t *buffer;
    const uint16_t width;
    const uint16_t height;

    BinaryMatrix(uint16_t width, uint16_t height);
    ~BinaryMatrix();

    void setPixel(uint16_t x, uint16_t y, bool value);
    bool getPixel(uint16_t x, uint16_t y) const;

    void setRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, bool value);

    void clear();

private:
    inline int loc(uint16_t x, uint16_t y) const;
};

#endif
