#ifndef B_CONFIGPARSER_H_INCLUDED
#define B_CONFIGPARSER_H_INCLUDED

#include <unordered_map>
#include <PGE/Input/InputManager.h>

class ConfigParser {
public:	
	enum class ActionKey {
		FORWARD = 0,
		BACK = 1,
		LEFT = 2,
		RIGHT = 3,
		NOCLIP = 4,
	};

	ConfigParser();
	PGE::KeyboardInput::Keycode getKey(ActionKey key);
	void setKey(ActionKey action, PGE::KeyboardInput::Keycode key);
	void constructDefaults();
	void constructFileFromMap();
	void readFromFile();
	void printParser();

private:
	std::unordered_map<uint64_t, uint64_t> settingMap;
};

#endif // B_CONFIGPARSER_H_INCLUDED
