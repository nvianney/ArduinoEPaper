#ifndef renderer_h
#define renderer_h

#include "eink_display.h"
#include "binary_matrix.h"
#include "image.h"

#include "font.h"

enum DisplayColor {
    BLACK, RED
};

enum TextAlignment {
    LEFT, CENTER, RIGHT
};

class Renderer {

    BinaryMatrix redData;
    BinaryMatrix blackData;
    EInkDisplay display;

    DisplayColor color = DisplayColor::BLACK;
    int borderWidth = 1;

    Font *font;

    // Bounds of rendering
    int _minX;
    int _minY;
    int _maxX;
    int _maxY;
    bool dirty = false;

public:
    Renderer(int width, int height);

    void drawRect(int x, int y, int width, int height);
    void fillRect(int x, int y, int width, int height);

    // https://benice-equation.blogspot.com/2016/10/equation-of-rounded-rectangle.html
    void fillRoundRect(int x, int y, int width, int height, int radius);

    void fillCircle(int centerX, int centerY, int radius);

    void drawLine(int x1, int y1, int x2, int y2);

    void drawImage(Image &image, int x, int y);

    void drawText(int x, int y, const char *text, TextAlignment align = TextAlignment::LEFT);
    void setFont(Font &font);

    void setColor(DisplayColor color);

    void clearAll();
    void render();

private:
    void updateBounds(int minX, int minY, int maxX, int maxY);
    void clearBounds();

    BinaryMatrix &data();
};


#endif
