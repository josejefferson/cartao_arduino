/**
 * Desenvolvido por Jefferson Dantas
 * Este código mostra um gráfico na tela do LCD
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

const int16_t speed = 100; // Velocidade de atualização dos dados
const float chartMax = 100; // Maior valor na escala do gráfico
const float chartMin = 0; // Menor valor na escala do gráfico
const int8_t chartPadding = 10; // Espaçamento dentro da área do gráfico
const int8_t chartScale = 14; // Escala vertical do gráfico
const int16_t chartDistance = 20; // Distância horizontal entre os pontos
const int16_t chartXOffset = 60; // Posição X da área do gráfico
const int16_t chartYOffset = 1; // Posição Y da área do gráfico
const int16_t chartWidth = 415; // Largura da área do gráfico
const int16_t chartHeight = 316; // Altura da área do gráfico
const int16_t chartWidthX = chartXOffset + chartWidth;
const int16_t chartHeightY = chartYOffset + chartHeight;
const int16_t chartBackground = 0x2124; // Cor de fundo do gráfico
const int16_t chartHGridColor = 0x9492; // Cor da grade horizontal do gráfico
const int16_t chartVGridColor = 0x52AA; // Cor da grade vertical do gráfico
const bool showValueBox = true; // Exibe uma caixa com o valor atual

// Não alterar os valores abaixo
const float delta = chartMax - chartMin; // Variação entre o maior valor e o menor (não alterar)
const int16_t chartPXOffset = chartXOffset + chartPadding; // Posição X da área do gráfico + chartPadding (não alterar)
const int16_t chartPYOffset = chartYOffset + chartPadding; // Posição Y da área do gráfico + chartPadding (não alterar)
const int16_t chartPWidth = chartWidth - chartPadding * 2; // Largura da área do gráfico + chartPadding (não alterar)
const int16_t chartPHeight = chartHeight - chartPadding * 2; // Altura da área do gráfico + chartPadding (não alterar)
const int8_t dataQuantity = chartPWidth / chartDistance + 1; // Quantidade de dados que cabem no gráfico (não alterar)
int16_t data[dataQuantity]; // Dados do gráfico

void setup() {
  // Setup the LCD
  TFT.InitLCD();
  TFT.setFont(SmallFont);
}

void loop() {
  drawBackground(); // Desenha o fundo do LCD
  drawChartBox(); // Desenha a caixa do gráfico
  drawChartScale(); // Desenha a escala vertical do gráfico
  while (1) {
    drawHorizontalGrid(); // Desenha a Grade Horizontal
    drawVerticalGrid(); // Desenha a Grade Vertical
    drawChartData(); // Desenha os dados do gráfico
    eraseChartData(); // Apaga os dados do gráfico
  }
}

// Desenha o fundo do LCD
void drawBackground() {
  TFT.setColor(0, 0, 0);
  TFT.fillRect(0, 0, TFT.getDisplayXSize() - 1, TFT.getDisplayYSize() - 1);
}

// Desenha a caixa do gráfico
void drawChartBox() {
  TFT.setColor(255, 255, 255);
  TFT.drawRect(chartXOffset - 1, chartYOffset - 1, chartWidthX + 2, chartHeightY + 2); // Borda do gráfico
  TFT.setColor(0, 0, 0);
  TFT.fillRect(chartXOffset, chartYOffset, chartWidthX, chartHeightY); // Fundo do gráfico
}

// Desenha a escala vertical do gráfico
void drawChartScale() {
  TFT.setColor(255, 255, 255);
  TFT.drawLine(chartXOffset - 5, chartPYOffset, chartXOffset - 5, chartYOffset + chartHeight - chartPadding); // Linha vertical da escala do gráfico
  for (int8_t i = 0; i < (chartPHeight / chartScale + 1); i++) {
    // Linhas horizontais da escala do gráfico
    TFT.drawLine(
      chartXOffset - 5,
      chartYOffset + chartHeight - chartPadding - chartScale * i,
      chartXOffset - 10,
      chartYOffset + chartHeight - chartPadding - chartScale * i
    );

    // Textos da escala do gráfico
    String text = String(round(delta / chartPHeight * (i * chartScale + chartMin)));
    for (int8_t j = text.length(); j < 6; j++) text = " " + text;
    TFT.print(text, chartXOffset - 60, chartYOffset + chartHeight - chartPadding - chartScale * i - 4);
  }
}

// Desenha a Grade Horizontal
void drawHorizontalGrid() {
  TFT.setColor(50, 50, 50);
  for (int8_t i = (chartPadding == 0 ? 1 : 0); i < (chartHeight / chartScale); i++) {
    TFT.drawLine(
      chartXOffset,
      chartYOffset + chartHeight - chartPadding - chartScale * i,
      chartXOffset + chartWidth - 1,
      chartYOffset + chartHeight - chartPadding - chartScale * i
    );
  }
}

// Desenha a Grade Vertical
void drawVerticalGrid() {
  TFT.setColor(50, 50, 50);
  for (int8_t i = 0; i < dataQuantity; i++) {
    TFT.drawLine(
      chartPXOffset + chartDistance * i,
      chartYOffset,
      chartPXOffset + chartDistance * i,
      chartYOffset + chartHeight - 1
    );
  }
}

// Desenha os dados do gráfico
void drawChartData() {
  int16_t prevValue;
  for (int8_t column = 0;; column++) {
    const float value = random(chartMin, chartMax + 1);
    drawValueBox(value);
    data[column] = value;
    if (column == 0) {
      prevValue = value;
      continue;
    }
    const int16_t x1 = chartPXOffset + (column - 1 < 0 ? 0 : column - 1) * chartDistance;
    const int16_t y1 = chartPYOffset + chartPHeight - chartPHeight / delta * prevValue + chartMin;
    const int16_t x2 = chartPXOffset + column * chartDistance;
    const int16_t y2 = chartPYOffset + chartPHeight - chartPHeight / delta * value + chartMin;
    if (column * chartDistance > chartPWidth) break;
    TFT.setColor(255, 255, 0);
    TFT.drawLine(x1, y1, x2, y2);
    TFT.setColor(255, 0, 0);
    TFT.fillCircle(x1, y1, 2);
    TFT.fillCircle(x2, y2, 2);
    prevValue = value;
    delay(speed);
  }
}

// Apaga os dados do gráfico
void eraseChartData() {
  for (int8_t i = 1; i < dataQuantity; i++) {
    const int16_t prevValue = data[i - 1];
    const float value = data[i];
    const int16_t x1 = chartPXOffset + (i - 1 < 0 ? 0 : i - 1) * chartDistance;
    const int16_t y1 = chartPYOffset + chartPHeight - chartPHeight / delta * prevValue + chartMin;
    const int16_t x2 = chartPXOffset + i * chartDistance;
    const int16_t y2 = chartPYOffset + chartPHeight - chartPHeight / delta * value + chartMin;
    TFT.setColor(0, 0, 0);
    TFT.drawLine(x1, y1, x2, y2);
    TFT.fillCircle(x1, y1, 2);
    TFT.fillCircle(x2, y2, 2);
  }
}

// Desenha a caixa com o valor atual
void drawValueBox(float value) {
  if (!showValueBox) return;
  const int16_t width = 50;
  const int16_t height = 20;
  const int16_t x = 473 - width;
  const int16_t y = 314 - height;
  TFT.setColor(255, 255, 255);
  TFT.setBackColor(0, 0, 255);
  TFT.drawRect(x, y, x + width, y + height);
  TFT.setColor(0, 0, 255);
  TFT.fillRect(x + 1, y + 1, x + 1 + width - 2, y + 1 + height - 2);
  TFT.setColor(255, 255, 255);
  TFT.print(String(value), x + 4, y + height / 2 - 3);
}