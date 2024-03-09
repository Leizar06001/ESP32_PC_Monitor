#include <TFT_eSPI.h>
#include <LiquidCrystal.h>

#include <map>

#define LCD_BACK 40

TFT_eSPI tft = TFT_eSPI();
LiquidCrystal LCD(4, 16, 17, 5, 18, 19);

int test = 0;

// #define TFT_BLACK       0x0000      /*   0,   0,   0 */
// #define TFT_NAVY        0x000F      /*   0,   0, 128 */
// #define TFT_DARKGREEN   0x03E0      /*   0, 128,   0 */
// #define TFT_DARKCYAN    0x03EF      /*   0, 128, 128 */
// #define TFT_MAROON      0x7800      /* 128,   0,   0 */
// #define TFT_PURPLE      0x780F      /* 128,   0, 128 */
// #define TFT_OLIVE       0x7BE0      /* 128, 128,   0 */
// #define TFT_LIGHTGREY   0xD69A      /* 211, 211, 211 */
// #define TFT_DARKGREY    0x7BEF      /* 128, 128, 128 */
// #define TFT_BLUE        0x001F      /*   0,   0, 255 */
// #define TFT_GREEN       0x07E0      /*   0, 255,   0 */
// #define TFT_CYAN        0x07FF      /*   0, 255, 255 */
// #define TFT_RED         0xF800      /* 255,   0,   0 */
// #define TFT_MAGENTA     0xF81F      /* 255,   0, 255 */
// #define TFT_YELLOW      0xFFE0      /* 255, 255,   0 */
// #define TFT_WHITE       0xFFFF      /* 255, 255, 255 */
// #define TFT_ORANGE      0xFDA0      /* 255, 180,   0 */
// #define TFT_GREENYELLOW 0xB7E0      /* 180, 255,   0 */
// #define TFT_PINK        0xFE19      /* 255, 192, 203 */ //Lighter pink, was 0xFC9F
// #define TFT_BROWN       0x9A60      /* 150,  75,   0 */
// #define TFT_GOLD        0xFEA0      /* 255, 215,   0 */
// #define TFT_SILVER      0xC618      /* 192, 192, 192 */
// #define TFT_SKYBLUE     0x867D      /* 135, 206, 235 */
// #define TFT_VIOLET      0x915C      /* 180,  46, 226 */
#define TFT_DDARKGREY    0x39E7      /* 76, 76, 76 */

void setup() {
  initDisplays();

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  LCD.begin(16, 2);
  LCD.clear();

  Serial.begin(115200);
  
  LCD.setCursor(0, 0);
  LCD.print("Awaiting data...");
  
  delay(1000);
  drawBackground();
}

std::map<String, String> readSerial();

void loop() {
  std::map<String, String> prev;
  while (1) {
    if (Serial.available() > 0){
      std::map<String, String> datas = readSerial();
      drawUI(datas, prev);
      LCD.setCursor(0, 0);
      LCD.print("    " + datas["time"] + "    ");
      prev = datas;
    }
    delay(20);
  }
}

void drawUI(std::map<String, String> datas, std::map<String, String> prev){
  if (datas["CPUuse"] != prev["CPUuse"])
    drawPercGauge(79, 66, 45, datas["CPUuse"], 40, 320, interpolateColors(datas["CPUuse"].toInt() * 0.01, 0x7bdf, TFT_RED), TFT_DDARKGREY);
  if (datas["CPUpower"] != prev["CPUpower"])
    tftText(65, 65, 5, 1, 1, TFT_CYAN, datas["CPUpower"] + " W");
  
  if (datas["GPUuse"] != prev["GPUuse"])
    drawPercGauge(241, 66, 45, datas["GPUuse"], 40, 320, interpolateColors(datas["GPUuse"].toInt() * 0.01, 0x7bdf, TFT_RED), TFT_DDARKGREY);
  if (datas["GPUpower"] != prev["GPUpower"])
    tftText(227, 65, 5, 1, 1, TFT_CYAN, datas["GPUpower"] + " W");


  if (datas["CPUtemp"] != prev["CPUtemp"]){
    tftVBar(134, 26, 10, 65, datas["CPUtemp"].toInt() / 90.0, interpolateColors(datas["CPUtemp"].toInt() / 90.0, 0x7bdf, TFT_RED));
    tftText(133, 96, 2, 1, 1, TFT_CYAN, datas["CPUtemp"]);
  }
  if (datas["GPUhot"] != prev["GPUhot"]){
    tftVBar(176, 26, 10, 65, (datas["GPUhot"].toInt() - 8) / 90.0, interpolateColors((datas["GPUhot"].toInt() - 8) / 90.0, 0x7bdf, TFT_RED));
    tftText(175, 96, 2, 1, 1, TFT_CYAN, String(datas["GPUhot"].toInt() - 8));
  }

  if (datas["CPUclock"] != prev["CPUclock"])
    tftText(10, 123, 15, 1, 1, TFT_CYAN, "Clock: " + datas["CPUclock"] + " MHz");
  if (datas["GPUclock"] != prev["GPUclock"])
    tftText(172, 123, 15, 1, 1, TFT_CYAN, "Clock: " + datas["GPUclock"] + " MHz");

  if (datas["CPUfan"] != prev["CPUfan"])
    drawFanGauge(25, 32, 19, datas["CPUfan"], 0, 225, interpolateColors(datas["CPUfan"].toInt() * 0.0005, 0x7bdf, 0xd3df), TFT_DDARKGREY);
  if (datas["GPUfan"] != prev["GPUfan"])
    drawFanRevGauge(295, 32, 19, datas["GPUfan"], 135, 360, interpolateColors(datas["GPUfan"].toInt() * 0.0005, 0x7bdf, 0xd3df), TFT_DDARKGREY);
  

  if (datas["RAMuse"] != prev["RAMuse"]){
    tftHBar(10, 163, 90, 10, datas["RAMuse"].toInt() / 16000.0, interpolateColors(datas["RAMuse"].toInt() / 16000.0, 0x7bdf, TFT_RED));
    tftText(110, 165, 7, 1, 1, TFT_CYAN, datas["RAMuse"] + " M");
  }

  if (datas["GPUmem"] != prev["GPUmem"]){
    tftHBar(172, 163, 90, 10, datas["GPUmem"].toInt() / 16000.0, interpolateColors(datas["GPUmem"].toInt() / 16000.0, 0x7bdf, TFT_RED));
    tftText(272, 165, 7, 1, 1, TFT_CYAN, datas["GPUmem"] + " M");
  }

  drawFans(datas, prev);
}

void drawBackground() {
  // CPU
  tft.drawRoundRect(0, 4, 158, 141, 5, TFT_CYAN);
  tftText(15, 0, 11, 1, 1, TFT_CYAN, "Ryzen 5600x");

  // RAM
  tft.drawRoundRect(0, 151, 158, 30, 5, TFT_CYAN);
  tftText(15, 147, 3, 1, 1, TFT_CYAN, "RAM");

  // GPU
  tft.drawRoundRect(162, 4, 158, 141, 5, TFT_CYAN);
  tftText(177, 0, 7, 1, 1, TFT_CYAN, "RX 6800");

  // VRAM
  tft.drawRoundRect(162, 151, 158, 30, 5, TFT_CYAN);
  tftText(177, 147, 4, 1, 1, TFT_CYAN, "VRAM");

  // FANS
  tft.drawRoundRect(0, 185, 320, 55, 5, TFT_CYAN);
  tftText(23, 227, 0, 0, 1, TFT_WHITE, "Mok");
  tftText(91, 227, 0, 0, 1, TFT_WHITE, "F1");
  tftText(155, 227, 0, 0, 1, TFT_WHITE, "F2");
  tftText(219, 227, 0, 0, 1, TFT_WHITE, "F3");
  tftText(277, 227, 0, 0, 1, TFT_WHITE, "Back");
}

void drawFans(std::map<String, String> datas, std::map<String, String> prev){
  if (datas["FAN1"] != prev["FAN1"])
    drawFanGauge(32, 217, 26, datas["FAN1"], 50, 310, interpolateColors(datas["FAN1"].toInt() * 0.0005, TFT_BLUE, TFT_CYAN), TFT_DDARKGREY);
  if (datas["FAN3"] != prev["FAN3"])
    drawFanGauge(96, 217, 26, datas["FAN3"], 50, 310, interpolateColors(datas["FAN3"].toInt() * 0.0005, TFT_BLUE, TFT_CYAN), TFT_DDARKGREY);
  if (datas["FAN4"] != prev["FAN4"])
    drawFanGauge(160, 217, 26, datas["FAN4"], 50, 310, interpolateColors(datas["FAN4"].toInt() * 0.0005, TFT_BLUE, TFT_CYAN), TFT_DDARKGREY);
  if (datas["FAN5"] != prev["FAN5"])
    drawFanGauge(224, 217, 26, datas["FAN5"], 50, 310, interpolateColors(datas["FAN5"].toInt() * 0.0005, TFT_BLUE, TFT_CYAN), TFT_DDARKGREY);
  if (datas["FAN6"] != prev["FAN6"])
    drawFanGauge(288, 217, 26, datas["FAN6"], 50, 310, interpolateColors(datas["FAN6"].toInt() * 0.0005, TFT_BLUE, TFT_CYAN), TFT_DDARKGREY);
}

void drawFanGauge(int x, int y, int radius, String value, float startAngle, float endAngle, uint32_t gaugeColor, uint32_t bgColor) {
  float calc_val = value.toInt() / 1800.0;
  if (calc_val <= 0) calc_val = 0.01;

  float angleRange = endAngle - startAngle;
  float angle = startAngle + (angleRange * calc_val);
  angle = fminf(fmaxf(angle, startAngle), endAngle); // Clamp angle within range

  int len = value.length() * 3;
  tftCenterText(x, y - 3, 4, 1, 1, TFT_CYAN, value);
  tft.drawSmoothArc(x, y, radius, radius * 0.75, startAngle, endAngle, bgColor, bgColor, 1);
  tft.drawSmoothArc(x, y, radius, radius * 0.75, startAngle, angle, gaugeColor, bgColor, 1);
}

void drawFanRevGauge(int x, int y, int radius, String value, float startAngle, float endAngle, uint32_t gaugeColor, uint32_t bgColor) {
  float calc_val = value.toInt() / 1800.0;
  if (calc_val <= 0) calc_val = 0.01;

  float angleRange = endAngle - startAngle;
  float angle = endAngle - (angleRange * calc_val);
  angle = fminf(fmaxf(angle, startAngle), endAngle); // Clamp angle within range

  int len = value.length() * 3;
  tftCenterText(x, y - 3, 4, 1, 1, TFT_CYAN, value);
  tft.drawSmoothArc(x, y, radius, radius * 0.75, startAngle, endAngle, bgColor, bgColor, 1);
  tft.drawSmoothArc(x, y, radius, radius * 0.75, angle, endAngle, gaugeColor, bgColor, 1);
}

void drawPercGauge(int x, int y, int radius, String value, float startAngle, float endAngle, uint32_t gaugeColor, uint32_t bgColor) {
  float calc_val = value.toInt() / 100.0;
  if (calc_val <= 0) calc_val = 0.01;

  float angleRange = endAngle - startAngle;
  float angle = startAngle + (angleRange * calc_val);
  angle = fminf(fmaxf(angle, startAngle), endAngle); // Clamp angle within range

  int len = value.length() * 3;
  tftText(x - len, y + radius * 0.7, 3, 1, 1, TFT_WHITE, value);
  tft.drawSmoothArc(x, y, radius, radius * 0.75, startAngle, endAngle, bgColor, bgColor, 1);
  tft.drawSmoothArc(x, y, radius, radius * 0.75, startAngle, angle, gaugeColor, bgColor, 1);
}

void tftHBar(int x, int y, int w, int h, float val, int color){
  int pos = w * val;
  tft.fillRect(x, y, pos, h, color);
  tft.fillRect(x + pos, y, w - pos, h, TFT_DDARKGREY);
}

void tftVBar(int x, int y, int w, int h, float val, int color){
  int pos = h * val;
  tft.fillRect(x, y + h - pos, w, pos, color);
  tft.fillRect(x, y, w, h - pos, TFT_DDARKGREY);
}

void tftText(int x, int y, int w, int h, int size, int color, String str){
  tft.setTextColor(color);
  tft.setTextSize(size);
  tft.fillRect(x - 3, y, w * 7 * size, h * 7 * size, TFT_BLACK);
  tft.setCursor(x, y);
  tft.print(str);
}

void tftCenterText(int x, int y, int w, int h, int size, int color, String str){
  tft.setTextColor(color);
  tft.setTextSize(size);
  tft.fillRect(x - w * 3, y, w * 6.5 * size, h * 7 * size, TFT_BLACK);
  tft.setCursor(x - str.length() * 3, y);
  tft.print(str);
}

std::map<String, String> readSerial() {
  String str = Serial.readStringUntil('\n');
  std::map<String, String> datas;

  size_t pos = 0;
  while (pos != std::string::npos){
    size_t labelEnd = str.indexOf(":", pos);
    if (labelEnd == std::string::npos) break;
    String label = str.substring(pos, labelEnd);

    size_t valStart = labelEnd + 1;
    size_t valEnd = str.indexOf(";", valStart);
    String val = str.substring(valStart, valEnd);

    datas[label] = val;

    Serial.printf("%s: %s\n", label, val);

    pos = valEnd + 1;
  }

  return datas;
}

void initDisplays() {
  // // TFT BACKLIGHT
  ledcSetup(0, 1000, 8);
  ledcAttachPin(14, 0);
  ledcWrite(0, 255);
  // LCD BACKLIGHT
  ledcSetup(2, 1000, 8);
  ledcAttachPin(21, 2);
  ledcWrite(2, LCD_BACK);
  // // LCD CONTRAST
  ledcSetup(1, 1000, 8);
  ledcAttachPin(15, 1);
  ledcWrite(1, 80);
}

uint16_t RGBToColor(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

uint16_t interpolateColors(float ratio, uint16_t color1, uint16_t color2) {
    // Extract RGB components of color1
    uint8_t r1 = (color1 >> 11) & 0x1F;
    uint8_t g1 = (color1 >> 5) & 0x3F;
    uint8_t b1 = color1 & 0x1F;

    // Extract RGB components of color2
    uint8_t r2 = (color2 >> 11) & 0x1F;
    uint8_t g2 = (color2 >> 5) & 0x3F;
    uint8_t b2 = color2 & 0x1F;

    // Interpolate RGB values
    uint8_t r = ((r2 - r1) * ratio) + r1;
    uint8_t g = ((g2 - g1) * ratio) + g1;
    uint8_t b = ((b2 - b1) * ratio) + b1;

    // Combine RGB values to get the final color in RGB565 format
    return ((uint16_t)r << 11) | ((uint16_t)g << 5) | b;
}



