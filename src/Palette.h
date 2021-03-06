/*

		Palette Class....


*/

#pragma once
// #include <RgbColor.h>
// #include <HslColor.h>
// #include <HsbColor.h>
#include <internal/RgbColor.h>
#include <internal/HslColor.h>
#include <internal/HsbColor.h>
#include <NeoPixelBus.h>
#include <functional>


#include <ArduinoJson.h>

//#define DebugPalette Serial

#if defined(DebugPalette)
#define PaletteDebugf(...) DebugPalette.printf(__VA_ARGS__)
//#define PaletteDebugf(_1, ...) DebugPalette.printf_P( PSTR(_1), ##__VA_ARGS__) //  this saves around 5K RAM...

#else
#define PaletteDebugf(...) {}
#endif

union storedColor {
	RgbColor rgb;
	HslColor hsl;
	HsbColor hsb;
};

//enum palette_type { OFF = 0, COMPLEMENTARY, MONOCHROMATIC, ANALOGOUS, SPLITCOMPLEMENTS, TRIADIC, TETRADIC, MULTI, WHEEL};

#define NUMBER_OF_RANDOM_MODES 4
#define NUMBER_OF_PALETTE_TYPES 9

extern const char * random_mode_strings[];
extern const char * palettes_strings[];

class EffectHandler;

class Palette
{

private:
	typedef std::function< void(void) > callback;

public:
	enum palette_type { OFF = 0, COMPLEMENTARY, MONOCHROMATIC, ANALOGOUS, SPLITCOMPLEMENTS, TRIADIC, TETRADIC, MULTI, WHEEL};
	enum random_mode { NOT_RANDOM = 0 , TOTAL_RANDOM, TIME_BASED_RANDOM, RANDOM_AFTER_LOOP};

	Palette(const char * name = "Palette");
	Palette(palette_type mode, uint16_t total, const char * name = "Palette" );
	~Palette();

	RgbColor next();
	RgbColor previous();
	RgbColor current();

	void refresh() {};

	void loop() {

//( _random == TIME_BASED_RANDOM || _random == RANDOM_AFTER_LOOP)

		if ( _mode != OFF && millis() - _randtimertick > (_delay * 1000) ) {

			if (_eventCallback) {
				_eventCallback();
			}

			_randtimertick = millis();
		}

	}


	void attachCallback( callback  ptr)  {
		_eventCallback = ptr;
	}

	void input(RgbColor inputcolour)
	{
		//if (_random) _random = NOT_RANDOM; // not sure i want this...

		//PaletteDebugf("[Palette] _input (%u,%u,%u)\n", _input.R, _input.G, _input.B);
		//PaletteDebugf("[Palette]new (%u,%u,%u)\n", inputcolour.R, inputcolour.G, inputcolour.B);

		if (!_random && (inputcolour.R != _input.R || inputcolour.G != _input.G || inputcolour.B != _input.B)) {
			_position = 0;
			PaletteDebugf("Input color changed (%u,%u,%u)\n", inputcolour.R, inputcolour.G, inputcolour.B);
			_input = inputcolour;

		}

	};

	// need to think about this.. if mode is changed then _available can be rubbish....
	// maybe _total should not be exposed.. but wanted can be...

	void mode(palette_type mode);
	void mode(const char *);
	Palette::palette_type mode() {return _mode;};
	const char * getModeString();

	static inline const char * enumToString(palette_type mode) { return palettes_strings[mode] ;  }
	static palette_type stringToEnum(const char *);

	Palette::random_mode randommode() { return _random; };

	static Palette::random_mode randommodeStringtoEnum(const char * mode);

	const char * randommodeAsString() { return random_mode_strings[_random]; }

	void randommode(random_mode random) { _random = random;}
	void randommode(const char *);
	uint16_t getavailable() { return _available; }

	void total(uint16_t total)
	{
		_total = total;
		_available = available(_mode, _total);
	}

	uint16_t total() {return _total; }

	float range() { return _range; }
	void range(float range) { _range = range; }
	void range (uint8_t range) { _range = float( range / 255 ); }

	RgbColor get(uint8_t position) {};

	bool addJson(JsonObject& root);
	bool parseJson(JsonObject& root);

	static RgbColor comlementary(RgbColor in, uint16_t position);
	static RgbColor monochromatic(RgbColor in, uint16_t position);
	static RgbColor analogous(RgbColor in, uint16_t position, uint16_t total, float range);
	static RgbColor splitcomplements(RgbColor in, uint16_t position, float range);
	static RgbColor triadic(RgbColor in, uint16_t position) // return multiple (total set to 3)
	{
		return multi(in, position, 3);
	}
	static RgbColor tetradic(RgbColor in, uint16_t position) // return multiple set to 4.. might tweak this a bit..
	{
		return multi(in, position, 4);
	}
	static RgbColor multi(RgbColor in, uint16_t position, uint16_t total) ;
	static RgbColor wheel(uint8_t position);

	static uint8_t available(palette_type mode, uint16_t total);

	void delay(uint32_t delay) { _delay = delay; }
	uint32_t delay() { return _delay; }
private:

	RgbColor _last;
	uint16_t _position{0};
	uint16_t _total{1}; 		// sets number of colours in palette.  not always used.
	uint16_t _available{1};	// some palettes have fixed number of colours available
	palette_type _mode{OFF};
	//bool _random = false;
	random_mode _random{NOT_RANDOM};
	RgbColor _input;
	float _range = 0.2f; // spread of palettes...
	uint32_t _delay{10};
	uint32_t _randtimertick{0};
	const char * _name;
	callback  _eventCallback{nullptr};

};
