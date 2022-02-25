/*
 Name:		VCTM.ino
 Created:	2/23/2022 6:22:27 PM
 Author:	Kitecraft
*/
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "ACS712.h"
#include "Bitter_8.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define VOLTAGE_PIN A0
#define MEASURED_VREF 4.94
#define CALIBRATED_VOLTAGE_DIVIDER 11.115
#define NUMBER_VOLTAGE_SAMPLES 10

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
GFXfont mediumFont = Bitter_VariableFont_wght8pt7b;

ACS712  ACS(A1, 4.94, 1023, 100);

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);
	if (!display.begin()) {
		Serial.println(F("SSD1306 allocation failed"));
		for (;;); 
	}
	//ACS.autoMidPoint();
	ACS.setMidPoint(513);
	Serial.print("Midpoint: ");
	Serial.println(ACS.getMidPoint());

	
	//display.display();
	//delay(2000);
	display.clearDisplay();
	
	
	display.setRotation(0);

	display.setTextColor(WHITE);
	//display.setFont(&mediumFont);
	display.setTextSize(1);

	display.drawFastVLine(43, 0, SCREEN_HEIGHT, WHITE);
	display.drawFastVLine(86, 0, SCREEN_HEIGHT, WHITE);

	display.display();

	display.setRotation(1);
	display.setTextColor(WHITE);
	
	display.setCursor(19, 0);
	display.print("Volts");

	display.setCursor(8, 44);
	display.print("Current");
	
	display.setTextSize(2);
}


void loop() {	
	UpdateVoltage();
	UpdateCurrent();


	delay(100);
  
}

void UpdateCurrent()
{
	char buf[8];
	int currentCurrent = ACS.mA_DC();
	if (currentCurrent < 0) {
		currentCurrent = 0;
	}

	display.fillRect(2, 59, 100, 14, BLACK);

	char msg[12];
	if (currentCurrent < 950) {
		sprintf(msg, "%imA", currentCurrent);
	}
	else {
		float f = (float)currentCurrent / 1000.0;
		dtostrf(f, 4, 2, buf);
		sprintf(msg, "%sA", buf);
	}

	uint16_t x1, y1;
	uint16_t w1, h1;
	display.getTextBounds(msg, 0, 0, &x1, &y1, &w1, &h1);

	//display.setCursor(32 - (w1 / 2) + xOffset, 39);
	display.setCursor(62 - w1, 59);
	//display.setCursor(2, 59);
	display.print(msg);

	display.display();
}

void UpdateVoltage()
{
	char msg[8];
	dtostrf(GetVoltage(), 4, 2, msg);
	
	display.fillRect(2, 16, 100, 14, BLACK);

	uint16_t x1, y1;
	uint16_t w1, h1;
	display.getTextBounds(msg, 0, 0, &x1, &y1, &w1, &h1);

	display.setCursor(62 - w1, 16);
	display.print(msg);
	
	display.display();
}


double GetVoltage()
{
	int sample_count = 0;
	int sum = 0;
	while (sample_count < NUMBER_VOLTAGE_SAMPLES) {
		sum += analogRead(VOLTAGE_PIN);
		sample_count++;
		delay(10);
	}

	return ((((float)sum / (float)NUMBER_VOLTAGE_SAMPLES * MEASURED_VREF) / 1024.0) * CALIBRATED_VOLTAGE_DIVIDER);
}
