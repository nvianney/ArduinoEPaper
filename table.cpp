#include "table.h"

Table::Table(TableProperties properties) : properties(properties) {
}

Table::~Table() {
}

void Table::setHeader(std::vector<String> header) {
    this->header = header;
}

void Table::addRow(std::vector<String> row) {
    if (row.size() != properties.cols) {
        Serial.printf("Cannot add row: input size is %d but no. of columns is %d.", row.size(), properties.cols);
        return;
    }

    data.push_back(row);
}

void Table::draw(Renderer *renderer, int x, int y) {
    renderer->setColor(DisplayColor::BLACK);
    renderer->setDrawMode();

    float weightSum = 0;
    for (int i = 0; i < properties.cols; i++) {
        weightSum += properties.weights[i];
    }
    // Header
    float startX = (float) x; // accumulates the width due to varying column widths
    renderer->getFont()->setScale(properties.headerFontScale);
    for (int i = 0; i < properties.cols; i++) {
        float weight = properties.weights[i] / weightSum;
        float width = weight * properties.width;
        int posX = startX + width/2;
        int posY = y;

        renderer->drawText(posX, posY, header[i].c_str(), TextAlignment::CENTER);
        startX += width;
    }

    renderer->drawRect(x, y + properties.headerHeight, properties.width, properties.headerUnderlineThickness);

    const float contentHeight = properties.height - (properties.headerHeight + properties.headerUnderlineThickness + properties.headerSpacing);
    const float startY = y + properties.headerHeight + properties.headerUnderlineThickness + properties.headerSpacing;
    bool invert = false;
    for (int r = 0; r < properties.rows; r++) {
        const int posY = startY + (contentHeight / properties.rows) * r;

        renderer->setDrawMode();

        if (invert) {
            renderer->fillRect(x, posY, properties.width, contentHeight / properties.rows);
        }

        startX = (float) x;
        for (int c = 0; c < properties.cols; c++) {
            const float weight = properties.weights[c] / weightSum;
            const float width = weight * properties.width;
            const int posX = startX + width/2;

            if (invert) {
                renderer->setClearMode();
            }
            renderer->drawText(posX, posY, data[r][c].c_str(), TextAlignment::CENTER);
            startX += width;
        }

        invert = !invert;
    }
}
