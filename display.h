#ifndef display_h
#define display_h

class Display {
    uint8_t *buffer;
    uint16_t width;
    uint16_t height;

public:
    Display(uint16_t width, uint16_t height);
    ~Display();

    void setPixel(uint16_t x, uint16_t y, bool value);
    bool getPixel(uint16_t x, uint16_t y);

private:
    inline int loc(uint16_t x, uint16_t y);
};

#endif
