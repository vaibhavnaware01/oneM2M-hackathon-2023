
#ifndef SCRC_OLED_H_
#define SCRC_OLED_H_

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
// #include <Adafruit_SH1106.h>
#include "scrc_constants.h"
#include "scrc_node.h"

void hw_setup_oled();

void display_oled(const struct sensors_data *display_buff, int display_idx);

#endif /* SCRC_OLED_H_ */
