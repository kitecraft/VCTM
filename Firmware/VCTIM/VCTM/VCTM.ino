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
#include <OneWire.h>
#include <DallasTemperature.h>


#define VOLTAGE_PIN A1
#define MEASURED_VREF 4.96
#define CALIBRATED_VOLTAGE_DIVIDER 11.115
#define NUMBER_VOLTAGE_SAMPLES 10

#define CURRENT_PIN A0
#define CURRENT_SENSE_MIDPOINT 513

#define TEMPERATURE_PIN 10

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_MOSI   8
#define OLED_CLK   9
#define OLED_DC    6
#define OLED_CS    5
#define OLED_RESET 7

#define DISPLAY_VOLTAGE_VALUE_Y_OFFSET 16
#define DISPLAY_CURRENT_VALUE_Y_OFFSET 59
#define DISPLAY_TEMP_VALUE_Y_OFFSET 101


Adafruit_SSD1306 g_Display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

ACS712  g_Acs(CURRENT_PIN, MEASURED_VREF, 1023, 100);
OneWire g_OneWire(TEMPERATURE_PIN);
DallasTemperature g_Temp(&g_OneWire);
DeviceAddress g_Thermometer;



void setup() {
	Serial.begin(9600);
	if (!g_Display.begin()) {
		Serial.println(F("SSD1306 allocation failed"));
		for (;;); 
	}
	g_Acs.setMidPoint(CURRENT_SENSE_MIDPOINT);

	g_Temp.begin();
	if (!g_Temp.getAddress(g_Thermometer, 0)) {
		Serial.println("Unable to find address for Device 0");
	}

	g_Display.clearDisplay();
	g_Display.setRotation(1);

	g_Display.setTextColor(WHITE);
	g_Display.setTextSize(1);

	g_Display.drawFastHLine(0, 37, SCREEN_WIDTH, WHITE);
	g_Display.drawFastHLine(0, 80, SCREEN_WIDTH, WHITE);

	g_Display.setTextColor(WHITE);
	
	g_Display.setCursor(19, 0);
	g_Display.print("Volts");

	g_Display.setCursor(9, 44);
	g_Display.print("Current");

	g_Display.setCursor(20, 87);
	g_Display.print("Temp");
	
	g_Display.setTextSize(2);
}


void loop() {	
	UpdateVoltage();
	UpdateCurrent();
	UpdateTemperature();

	delay(100);
  
}

void UpdateTemperature()
{
	char msg[8];
	g_Temp.requestTemperatures();
	float tempC = g_Temp.getTempC(g_Thermometer);

	dtostrf(tempC, 4, 2, msg);
	UpdateDisplay(msg, DISPLAY_TEMP_VALUE_Y_OFFSET);
}

void UpdateCurrent()
{
	int currentCurrent = g_Acs.mA_DC();
	if (currentCurrent < 0) {
		currentCurrent = 0;
	}

	char msg[12];
	if (currentCurrent < 950) {
		sprintf(msg, "%imA", currentCurrent);
	}
	else {
		float f = (float)currentCurrent / 1000.0;
		char buf[8];
		dtostrf(f, 4, 2, buf);
		sprintf(msg, "%sA", buf);
	}

	UpdateDisplay(msg, DISPLAY_CURRENT_VALUE_Y_OFFSET);
}

void UpdateVoltage()
{
	char msg[8];
	dtostrf(GetVoltage(), 4, 2, msg);
	
	UpdateDisplay(msg, DISPLAY_VOLTAGE_VALUE_Y_OFFSET);
}

void UpdateDisplay(char* msg, int yOffset)
{
	uint16_t x1, y1;
	uint16_t w1, h1;
	g_Display.getTextBounds(msg, 0, 0, &x1, &y1, &w1, &h1);

	g_Display.fillRect(2, yOffset, SCREEN_WIDTH - 2, 14, BLACK);
	g_Display.setCursor(62 - w1, yOffset);
	g_Display.print(msg);

	g_Display.display();
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
