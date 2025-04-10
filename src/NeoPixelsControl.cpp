#include "NeoPixelsControl.h"

/// @brief Creates a NeoPixel controller
/// @param Name The device name
/// @param Pin Pin to use
/// @param LEDCount The number of LEDs in use
/// @param RGB_Type The type of NeoPixel
/// @param configFile Name of the config file to use
NeoPixelsControl::NeoPixelsControl(String Name, int Pin, int LEDCount, neoPixelType RGB_Type, String configFile) : Actor(Name) {
	config_path = "/settings/act/" + configFile;
	led_config.Pin = Pin;
	led_config.LEDCount = LEDCount;
	led_config.RGB_Type = RGB_Type;
}

/// @brief Starts a NeoPixel controller 
/// @return True on success
bool NeoPixelsControl::begin() {
	// Set description
	Description.actionQuantity = 1;
	Description.type = "output";
	Description.actions = {{"setcolor", 0}};
	leds = new Adafruit_NeoPixel(led_config.LEDCount, led_config.Pin, led_config.RGB_Type);
	// Create settings directory if necessary
	if (!checkConfig(config_path)) {
		// Set defaults
		return saveConfig(config_path, getConfig());
	} else {
		// Load settings
		return setConfig(Storage::readFile(config_path), false);
	}
}

/// @brief Receives an action
/// @param action The action to process 0 to set colors 1 to set brightness
/// @param payload Either an array or RGB(W) values, or a brightness value 0-255
/// @return JSON response with OK
std::tuple<bool, String> NeoPixelsControl::receiveAction(int action, String payload) {
	if (action == 0) {
		// Allocate the JSON document
		JsonDocument doc;
		// Deserialize file contents
		DeserializationError error = deserializeJson(doc, payload);
		// Test if parsing succeeds.
		if (error) {
			Logger.print(F("Deserialization failed: "));
			Logger.println(error.f_str());
			return { false, R"({"Response": "Error"})" };
		}
		if (doc["RGB_Values"][0].size() == 3) {
			// Assign loaded values
			uint8_t RGB_Values[led_config.LEDCount][3];
			for (int i = 0; i < led_config.LEDCount; i++) {
				RGB_Values[i][0] = doc["RGB_Values"][i][0].as<uint8_t>();
				RGB_Values[i][1] = doc["RGB_Values"][i][1].as<uint8_t>();
				RGB_Values[i][2] = doc["RGB_Values"][i][2].as<uint8_t>();
			}
			writePixels(RGB_Values);
		} else if (doc["RGB_Values"][0].size() == 4){
			// Assign loaded values
			uint8_t RGBW_Values[led_config.LEDCount][4];
			for (int i = 0; i < led_config.LEDCount; i++) {
				RGBW_Values[i][0] = doc["RGB_Values"][i][0].as<uint8_t>();
				RGBW_Values[i][1] = doc["RGB_Values"][i][1].as<uint8_t>();
				RGBW_Values[i][2] = doc["RGB_Values"][i][2].as<uint8_t>();
				RGBW_Values[i][3] = doc["RGB_Values"][i][3].as<uint8_t>();
			}
			writePixels(RGBW_Values);
		} else {
			return { false, R"({"Response": "Error: incorrect number of RGB(W) values"})" };
		}
		// Return success
		return { true, R"({"Response": "OK"})" };
	} 
	return { false, R"({"Response": "Error: Unknown action"})" };
}

/// @brief Gets the current config
/// @return A JSON string of the config
String NeoPixelsControl::getConfig() {
	// Allocate the JSON document
	JsonDocument doc;
	// Assign current values
	doc["Name"] = Description.name;
	doc["Pin"] = led_config.Pin;
	doc["LEDCount"] = led_config.LEDCount;
	doc["RGB_Type"] = (uint16_t)led_config.RGB_Type;

	// Create string to hold output
	String output;
	// Serialize to string
	serializeJson(doc, output);
	return output;
}

/// @brief Sets the configuration for this device
/// @param config A JSON string of the configuration settings
/// @param save If the configuration should be saved to a file
/// @return True on success
bool NeoPixelsControl::setConfig(String config, bool save) {
	// Allocate the JSON document
  	JsonDocument doc;
	// Deserialize file contents
	DeserializationError error = deserializeJson(doc, config);
	// Test if parsing succeeds.
	if (error) {
		Logger.print(F("Deserialization failed: "));
		Logger.println(error.f_str());
		return false;
	}
	// Assign loaded values
	Description.name = doc["Name"].as<String>();
	led_config.Pin = doc["Pin"].as<int>();
	led_config.LEDCount = doc["LEDCount"].as<int>();
	led_config.RGB_Type = (neoPixelType)doc["RGB_Type"].as<uint16_t>();
	if (save) {
		if (!saveConfig(config_path, getConfig())) {
			return false;
		}
	}
	return configureOutput();
}

/// @brief Configures the pin for use
/// @return True on success
bool NeoPixelsControl::configureOutput() {
	leds = new Adafruit_NeoPixel(led_config.LEDCount, led_config.Pin, led_config.RGB_Type);
	leds->begin();
	return true;
}

/// @brief Sets the colors of all the LEDs in an RGB strip
/// @param RGB_Values The RGB values
/// @return True on success
bool NeoPixelsControl::writePixels(uint8_t RGB_Values[][3]) {
	for (int i = 0; i < led_config.LEDCount; i++) {
		leds->setPixelColor(i, RGB_Values[i][0], RGB_Values[i][1], RGB_Values[i][2]);
	}
	leds->show();
	return true;
}

/// @brief Sets the colors of all the LEDs in an RGBW strip
/// @param RGBW_Values The RGBW values
/// @return True on success
bool NeoPixelsControl::writePixels(uint8_t RGBW_Values[][4]) {
	for (int i = 0; i < led_config.LEDCount; i++) {
		leds->setPixelColor(i, RGBW_Values[i][0], RGBW_Values[i][1], RGBW_Values[i][2], RGBW_Values[i][3]);
	}
	leds->show();
	return true;
}