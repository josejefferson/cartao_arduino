/**
 * Desenvolvido por Jefferson Dantas
 * Este código mostra um relógio na tela do LCD
 */

#include <UTFT.h>

// Pinos do LCD
#define TFT_CS 40
#define TFT_CD 38
#define TFT_WR 39
#define TFT_RD 44
#define TFT_RST 41

// Declare which fonts we will be using
extern uint8_t SmallFont[];

UTFT TFT(ILI9481, 38, 39, 40, 41);

const int16_t centerX = 320 / 2; // Centro X do relógio
const int16_t centerY = 240 / 2; // Centro Y do relógio
const int16_t radius = 80; // Raio do relógio
const int16_t clockHourLineSize = 5; // Tamanho do traço das horas
const int16_t hourPointerSize = 35; // Tamanho do ponteiro das horas
const int16_t minutePointerSize = 50; // Tamanho do ponteiro dos minutos
const int16_t secondPointerSize = 60; // Tamanho do ponteiro dos segundos
const int16_t clockNumberDistance = radius - 13; // Distância dos números do relógio
const int16_t clockBorderColor = 0xFFFF; // Cor da borda do relógio
const int16_t clockHourLineColor = 0xFFFF; // Cor da linha das horas
const int16_t clockNumberColor = 0xFFFF; // Cor dos números
const int16_t clockHourPointerColor = 0xFFFF; // Cor do ponteiro das horas
const int16_t clockMinutePointerColor = 0xFFFF; // Cor do ponteiro dos minutos
const int16_t clockSecondPointerColor = 0xF000; // Cor do ponteiro dos segundos
const bool showValueBox = true; // Mostra o horário digital

// Não alterar
int lastHour = 0;
int lastMinute = 0;
int lastSecond = 0;

void setup() {
  // Setup the LCD
  TFT.InitLCD();
  TFT.setFont(SmallFont);
}

void loop() {
  drawBackground();
  drawClockCircle();
  drawClockHourLines();
  drawClockNumbers();

  while (1) {
    unsigned long totalMilliseconds = millis();
    unsigned long totalSeconds = totalMilliseconds / 1000;
    unsigned long totalMinutes = totalSeconds / 60;
    unsigned long totalHours = totalMinutes / 60;
    unsigned int seconds = totalSeconds % 60;
    unsigned int minutes = totalMinutes % 60;
    unsigned int hours = totalHours % 12;

    if (lastHour != hours) drawHoursPointer(lastHour, true);
    if (lastMinute != minutes) drawMinutesPointer(lastMinute, true);
    if (lastSecond != seconds) drawSecondsPointer(lastSecond, true);
    drawHoursPointer(hours, false);
    drawMinutesPointer(minutes, false);
    drawSecondsPointer(seconds, false);

    // drawValueBox(seconds, minutes, hours);
    lastHour = hours;
    lastMinute = minutes;
    lastSecond = seconds;

    // delay(millis() % 1000); // Na vida real
    delay(100); // No simulador
  }
}

// Desenha o fundo do LCD
void drawBackground() {
  TFT.setColor(0, 0, 0);
  TFT.fillRect(0, 0, TFT.getDisplayXSize() - 1, TFT.getDisplayYSize() - 1);
}

// Desenha o círculo do relógio
void drawClockCircle() {
  TFT.setColor(255, 255, 255);
  TFT.drawCircle(centerX, centerY, radius);
}

// Desenha as linhas das horas
void drawClockHourLines() {
  for (int8_t i = 0; i < 12; i++) {
    float x1 = radius * cos(radians(i * 30));
    float y1 = radius * sin(radians(i * 30));
    float x2 = (radius - clockHourLineSize) * cos(radians(i * 30));
    float y2 = (radius - clockHourLineSize) * sin(radians(i * 30));
    TFT.setColor(255, 255, 255);
    TFT.drawLine(
      centerX + x1,
      centerY + y1,
      centerX + x2,
      centerY + y2
    );
  }
}

// Desenha os números das horas
void drawClockNumbers() {
  for (int8_t i = 1; i <= 12; i++) {
    float x = centerX + clockNumberDistance * cos(radians(i * 30 - 90));
    float y = centerY + clockNumberDistance * sin(radians(i * 30 - 90));
    TFT.setColor(255, 255, 255);
    TFT.print(String(i), x - 3, y - 4);
  }
}

// Desenha ou apaga o ponteiro das horas
void drawHoursPointer(int hour, bool erase) {
  float x = centerX + hourPointerSize * cos(radians(hoursToAngle(hour)));
  float y = centerY + hourPointerSize * sin(radians(hoursToAngle(hour)));
  if (erase) {
    TFT.setColor(0, 0, 0);
  } else {
    TFT.setColor(255, 255, 255);
  }
  TFT.drawLine(centerX, centerY, x, y);
}

// Desenha ou apaga o ponteiro dos minutos
void drawMinutesPointer(int minute, bool erase) {
  float x = centerX + minutePointerSize * cos(radians(minutesToAngle(minute)));
  float y = centerY + minutePointerSize * sin(radians(minutesToAngle(minute)));
  if (erase) {
    TFT.setColor(0, 0, 0);
  } else {
    TFT.setColor(255, 255, 255);
  }
  TFT.drawLine(centerX, centerY, x, y);
}

// Desenha ou apaga o ponteiro dos segundos
void drawSecondsPointer(int second, bool erase) {
  float x = centerX + secondPointerSize * cos(radians(secondsToAngle(second)));
  float y = centerY + secondPointerSize * sin(radians(secondsToAngle(second)));
  if (erase) {
    TFT.setColor(0, 0, 0);
  } else {
    TFT.setColor(255, 0, 0);
  }
  TFT.drawLine(centerX, centerY, x, y);
}

// Converte horas para ângulo
int hoursToAngle(int hour) {
  return hour * 30 - 90;
}

// Converte minutos para ângulo
int minutesToAngle(int minute) {
  return minute * 6 - 90;
}

// Converte segundos para ângulo
int secondsToAngle(int second) {
  return second * 6 - 90;
}

// // Desenha a caixa com o valor atual
// void drawValueBox(unsigned int seconds, unsigned int minutes, unsigned int hours) {
//   if (!showValueBox) return;
//   if (hours == lastHour && minutes == lastMinute && seconds == lastSecond) return;

//   String strSeconds = String(seconds);
//   String strMinutes = String(minutes);
//   String strHours = String(hours == 0 ? 12 : hours);

//   while (strSeconds.length() < 2) strSeconds = "0" + strSeconds;
//   while (strMinutes.length() < 2) strMinutes = "0" + strMinutes;
//   while (strHours.length() < 2) strHours = "0" + strHours;

//   const int16_t width = 57;
//   const int16_t height = 20;
//   const int16_t x = 320 - width;
//   const int16_t y = 240 - height;
//   TFT.drawRect(x, y, width, height, ILI9341_WHITE);
//   TFT.fillRect(x + 1, y + 1, width - 2, height - 2, ILI9341_BLUE);
//   TFT.setCursor(x + 4, y + height / 2 - 3);
//   TFT.print(strHours);
//   TFT.print(":");
//   TFT.print(strMinutes);
//   TFT.print(":");
//   TFT.print(strSeconds);
// }