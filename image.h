#ifndef base_image_h 
#define base_image_h

#include <Arduino.h>
#include <pgmspace.h>

template<typename T>
// sizeof(void) = 1
T derefByte(const void *ptr, int byteOffset) {
    if (sizeof(T) == sizeof(uint8_t)) {
        return pgm_read_byte(ptr + byteOffset);
    } else if (sizeof(T) == sizeof(uint16_t)) {
        return pgm_read_word(ptr + byteOffset);
    } else if (sizeof(T) == sizeof(uint32_t)) {
        return pgm_read_dword(ptr + byteOffset);
    }
    Serial.print("Error: unknown type size ");
    Serial.println(sizeof(T));
    return 0;
    // return *( (T*) (ptr + byteOffset));
}

template<typename T>
T derefType(const void *ptr, int offset, int typeSize = sizeof(T)) {
    return derefByte<T>((const char*) ptr, offset * typeSize);
}

struct Pixel {
    const int r;
    const int g;
    const int b;
    const int bytes_per_pixel;

    Pixel(int red, int green, int blue, int bpp);
};

struct Image {
    const unsigned int width;
    const unsigned int height;
    const unsigned int bytes_per_pixel; /* 2: RGB16, 3: RGB, 4: RGBA */
    const unsigned char *pixel_data;

    static Image asImage(const void *imageStruct);

    Image(unsigned int w, unsigned int h, unsigned int bpp, unsigned char *data);

    Pixel pixelAt(int x, int y) const;
};

#endif