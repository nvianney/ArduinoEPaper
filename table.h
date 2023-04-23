#ifndef table_h
#define table_h

#include <Arduino.h>
#include <pgmspace.h>
#include <vector>

#include "font.h"
#include "renderer.h"

struct TableProperties {
    int rows;
    int cols;
    int width;
    int height;
    float *weights = nullptr;

    int headerHeight = 30;
    int headerUnderlineThickness = 1;
    int headerSpacing = 4;

    float headerFontScale = 1.0f;
    float contentFontScale = 1.0f;
};

class Table {
    std::vector<String> header;
    std::vector<std::vector<String> > data;

    TableProperties properties;

public:
    Table(TableProperties properties);
    ~Table();

    void setHeader(std::vector<String> header);
    void addRow(std::vector<String> row);

    void draw(Renderer *renderer, int x, int y);
};

#endif
