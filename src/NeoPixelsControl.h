/*
* This file and associated .cpp file are licensed under the GPLv3 License Copyright (c) 2025 Sam Groveman
* 
* ArduinoJSON: https://arduinojson.org/
*
* Contributors: Sam Groveman
*/
#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Actor.h>
#include <Adafruit_NeoPixel.h>

/// @brief Class describing a generic output on a GPIO pin
class NeoPixelsControl : public Actor {
	protected:
		/// @brief Output configuration
		struct {
			/// @brief The pin number attached to the output
			int Pin;

			/// @brief The number of LEDs in use
			int LEDCount;

			/// @brief Enables gamma correction of colors for better appearance
			bool gammaCorrection = false;

			/// @brief The NeoPixel type
			neoPixelType RGB_Type;
		} led_config;

		/// @brief Path to configuration file
		String config_path;
		
		/// @brief LED  driver
		Adafruit_NeoPixel* leds;

		bool configureOutput();

		bool writePixels(uint8_t RGB_Values[][3]);
		bool writePixels(uint8_t RGBW_Values[][4]);

	public:
		NeoPixelsControl(String Name, int Pin, int LEDCount, neoPixelType RGB_Type = NEO_GRB + NEO_KHZ800, String configFile = "NeoPixelsControl.json");
		bool begin();
		std::tuple<bool, String> receiveAction(int action, String payload = "");
		String getConfig();
		bool setConfig(String config, bool save);
};