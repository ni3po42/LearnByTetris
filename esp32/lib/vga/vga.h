#ifndef __vga_h_
#define __vga_h_

#include <stdint.h>
#include <cstddef>

void vga_init();

void doVgaSignals();

void setPixel(size_t row, size_t col, uint8_t color);

void renderMessage(const char* message, size_t row, size_t col, uint8_t foregroundColor, uint8_t backgroundColor);

void clearVRAM();

void prepareWriteToVRAM();

void completeWriteToVRAM();

#endif
