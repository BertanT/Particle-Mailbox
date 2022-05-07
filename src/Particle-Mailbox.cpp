/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "/Users/bertan/Developer/GitRepositories/Particle-Mailbox/Particle-Mailbox/src/Particle-Mailbox.ino"
/*
 * Project Particle Mailbox
 * Description: IoT Mailbox connected to particle cloud!
				Connect it to your favorite messaging service to get notified in the real world!
 * Author: M. Bertan Tarakçıoğlu
 * Date: May the 4th 2022
 */

 // Set system mode
void Update();
void Increment();
void Reverse();
void Scanner(uint32_t color1, uint8_t interval);
void ScannerUpdate();
void FadeUpdate();
uint32_t DimColor(uint32_t color);
void ColorSet(uint32_t color);
uint8_t Red(uint32_t color);
uint8_t Green(uint32_t color);
uint8_t Blue(uint32_t color);
void setup();
void loop();
void setFlag(const char* event, const char* data);
#line 10 "/Users/bertan/Developer/GitRepositories/Particle-Mailbox/Particle-Mailbox/src/Particle-Mailbox.ino"
SYSTEM_MODE(SEMI_AUTOMATIC);
SYSTEM_THREAD(ENABLED);

// Import libraries
#include <neopixel.h>

// Define NeoPixel Constants
#define PIXEL_COUNT 8
#define PIXEL_PIN D1
#define PIXEL_BRIGHTNESS 255 // Between 0 and 255

// Define servo pin
#define SERVO_PIN D0

// Thanks to Adafruit for the NeoPatterns async NeoPixel animations!
// Pattern types supported:
enum pattern
{
	NONE,
	SCANNER,
	FADE
};
// Pattern directions supported:
enum direction
{
	FORWARD,
	REVERSE
};

// NeoPattern Class - derived from the Adafruit_NeoPixel class
class NeoPatterns : public Adafruit_NeoPixel
{
public:
	// Member Variables:
	pattern ActivePattern; // which pattern is running
	direction Direction;   // direction to run the pattern

	unsigned long Interval;	  // milliseconds between updates
	unsigned long lastUpdate; // last update of position

	uint32_t Color1, Color2; // What colors are in use
	uint16_t TotalSteps;	 // total number of steps in the pattern
	uint16_t Index;			 // current step within the pattern

	void (*OnComplete)(); // Callback on completion of pattern

	// Constructor - calls base-class constructor to initialize strip
	NeoPatterns(uint16_t pixels, uint8_t pin, uint8_t type, void (*callback)())
		: Adafruit_NeoPixel(pixels, pin, type)
	{
		OnComplete = callback;
	}

	// Update the pattern
	void Update()
	{
		if ((millis() - lastUpdate) > Interval) // time to update
		{
			lastUpdate = millis();
			switch (ActivePattern)
			{
			case SCANNER:
				ScannerUpdate();
				break;
			case FADE:
				FadeUpdate();
				break;
			default:
				break;
			}
		}
	}

	// Increment the Index and reset at the end
	void Increment()
	{
		if (Direction == FORWARD)
		{
			Index++;
			if (Index >= TotalSteps)
			{
				Index = 0;
				if (OnComplete != NULL)
				{
					OnComplete(); // call the completion callback
				}
			}
		}
		else // Direction == REVERSE
		{
			--Index;
			if (Index <= 0)
			{
				Index = TotalSteps - 1;
				if (OnComplete != NULL)
				{
					OnComplete(); // call the completion callback
				}
			}
		}
	}

	// Reverse pattern direction
	void Reverse()
	{
		if (Direction == FORWARD)
		{
			Direction = REVERSE;
			Index = TotalSteps - 1;
		}
		else
		{
			Direction = FORWARD;
			Index = 0;
		}
	}

	// Initialize for a SCANNNER
	void Scanner(uint32_t color1, uint8_t interval)
	{
		ActivePattern = SCANNER;
		Interval = interval;
		TotalSteps = (numPixels() - 1) * 2;
		Color1 = color1;
		Index = 0;
	}

	// Update the Scanner Pattern
	void ScannerUpdate()
	{
		for (int i = 0; i < numPixels(); i++)
		{
			if (i == Index) // Scan Pixel to the right
			{
				setPixelColor(i, Color1);
			}
			else if (i == TotalSteps - Index) // Scan Pixel to the left
			{
				setPixelColor(i, Color1);
			}
			else // Fading tail
			{
				setPixelColor(i, DimColor(getPixelColor(i)));
			}
		}
		show();
		Increment();
	}

	// Initialize for a Fade
	void Fade(uint32_t color1, uint32_t color2, uint16_t steps, uint8_t interval, direction dir = FORWARD)
	{
		ActivePattern = FADE;
		Interval = interval;
		TotalSteps = steps;
		Color1 = color1;
		Color2 = color2;
		Index = 0;
		Direction = dir;
	}

	// Update the Fade Pattern
	void FadeUpdate()
	{
		// Calculate linear interpolation between Color1 and Color2
		// Optimize order of operations to minimize truncation error
		uint8_t red = ((Red(Color1) * (TotalSteps - Index)) + (Red(Color2) * Index)) / TotalSteps;
		uint8_t green = ((Green(Color1) * (TotalSteps - Index)) + (Green(Color2) * Index)) / TotalSteps;
		uint8_t blue = ((Blue(Color1) * (TotalSteps - Index)) + (Blue(Color2) * Index)) / TotalSteps;

		ColorSet(Color(red, green, blue));
		show();
		Increment();
	}

	// Calculate 50% dimmed version of a color (used by ScannerUpdate)
	uint32_t DimColor(uint32_t color)
	{
		// Shift R, G and B components one bit to the right
		uint32_t dimColor = Color(Red(color) >> 1, Green(color) >> 1, Blue(color) >> 1);
		return dimColor;
	}

	// Set all pixels to a color (synchronously)
	void ColorSet(uint32_t color)
	{
		for (int i = 0; i < numPixels(); i++)
		{
			setPixelColor(i, color);
		}
		show();
	}

	// Returns the Red component of a 32-bit color
	uint8_t Red(uint32_t color)
	{
		return (color >> 16) & 0xFF;
	}

	// Returns the Green component of a 32-bit color
	uint8_t Green(uint32_t color)
	{
		return (color >> 8) & 0xFF;
	}

	// Returns the Blue component of a 32-bit color
	uint8_t Blue(uint32_t color)
	{
		return color & 0xFF;
	}
};

// Function prototypes
void silentServoWrite(int position);
void flagUp(const char* event, const char* data);
void flagDown(const char* event, const char* data);
void NPStickAnimationComplete();

// Declare NeoPattern Object
NeoPatterns NeoPixelStick(PIXEL_COUNT, PIXEL_PIN, WS2812B, &NPStickAnimationComplete);

// Declare Servo Object
Servo flagServo;

// Defining NeoPixel NeoPixel colors! Customize to your own liking!
const uint32_t BLACK = NeoPixelStick.Color(0, 0, 0);
const uint32_t LOADING = NeoPixelStick.Color(255, 255, 255);
const uint32_t NOTIFICATION = NeoPixelStick.Color(55, 25, 255);

bool isFlagUp = false;

// Variable for triggering NeoPixel turn off animation!
bool willTurnLightOff = true;

// Setup function
void setup()
{
	// Initialize NeoPixel
	NeoPixelStick.begin();
	NeoPixelStick.setBrightness(PIXEL_BRIGHTNESS);

	// Initialize Servo and reset flag position
	flagServo.attach(SERVO_PIN);
	silentServoWrite(15);

	// Set up cloud variable and subscribe to trigger event for the flag
	Particle.variable("isFlagUp", isFlagUp);
	Particle.subscribe("set_mailbox_flag", setFlag);

	// Initiate cloud connection!
	Particle.connect();
}

// Loop function
void loop()
{
	if (Particle.connected())
	{
		if (isFlagUp)
		{
			// Continiously fade in and out the NeoPixel if the flag is up and device is connected
			// and the animation hasn't been already initiated
			if (NeoPixelStick.ActivePattern != FADE)
			{
				willTurnLightOff = true;
				NeoPixelStick.Fade(BLACK, NOTIFICATION, 150, 30, FORWARD);
			}
		}
		else
		{
			// If the flag is down, the NeoPixel hasn't been turned off yet,
			// and the device is connected, turn it off with a fade
			if (willTurnLightOff)
			{
				// Start turn off fade from white if loading animation was running,
				// otherwise from the current color of the first NeoPixel
				uint32_t fadeStartColor = LOADING;

				if (NeoPixelStick.ActivePattern == FADE)
				{
					fadeStartColor = NeoPixelStick.getPixelColor(0);
				}

				uint32_t black = NeoPixelStick.Color(0, 0, 0);
				NeoPixelStick.Fade(fadeStartColor, black, 100, 10, FORWARD);
				willTurnLightOff = false;
			}
		}
	}
	else
	{
		// If the device is not connected, initiate NeoPixel loading animation
		if (NeoPixelStick.ActivePattern != SCANNER)
		{
			willTurnLightOff = true;
			NeoPixelStick.Scanner(LOADING, 50);
		}
	}

	// Continue async animation execution
	NeoPixelStick.Update();
}

// Function run when pattern completes
void NPStickAnimationComplete()
{
	// Revers-fade the NeoPixel rather than an instant transition if the flag is up
	if (NeoPixelStick.ActivePattern == FADE && isFlagUp && Particle.connected())
	{
		NeoPixelStick.Reverse();
	}
	// Stop animation if the NeoPixel turn off animation was just completed
	else if (NeoPixelStick.ActivePattern != NONE && !isFlagUp && Particle.connected())
	{
		NeoPixelStick.ActivePattern = NONE;
		NeoPixelStick.clear();
		NeoPixelStick.show();
	}
}

// The small servo I use is not powerful enough to hold the heavy flag and it makes a buzzing noise
// This is a workaround to stop the buzzing
void silentServoWrite(int position)
{
	flagServo.write(position);
	delay(1000);
	flagServo.detach();
	flagServo.attach(SERVO_PIN);
}

// Cloud event handler to set flag from the cloud
void setFlag(const char* event, const char* data)
{
	// Pass in "up" or "down" to event data in order to set the flag accordingly
	if (strcmp(data, "up") == 0)
	{
		isFlagUp = true;
		silentServoWrite(85);
	}
	else if (strcmp(data, "down") == 0)
	{
		isFlagUp = false;
		silentServoWrite(15);
	}
}
