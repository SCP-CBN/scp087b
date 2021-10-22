#include "ConfigParser.h"

#include <iostream>
#include <fstream>
#include <cstdint>

using namespace std;

const int bufferSize = sizeof(uint64_t) + sizeof(uint64_t);
char filename[] = "./config.dat";

ConfigParser::ConfigParser() {
	settingMap = unordered_map<uint64_t, uint64_t>();
	ifstream checkStream;
	checkStream.open(filename);
	if (checkStream) { readFromFile(); }
	else {
		constructDefaults();
		constructFileFromMap();
	}
	checkStream.close();
}

void ConfigParser::setKey(ActionKey action, PGE::KeyboardInput::Keycode key) {
	settingMap.insert({ (uint64_t)action, (uint64_t)key });
}

PGE::KeyboardInput::Keycode ConfigParser::getKey(ActionKey key) {
	auto searchValue = settingMap.find((uint64_t)key);
	return (searchValue != settingMap.end() ? (PGE::KeyboardInput::Keycode)searchValue->second : PGE::KeyboardInput::Keycode::UNKNOWN);
}

void ConfigParser::printParser() {
	for (const auto& [action, key] : settingMap) {
		cout << "Action - " << action << '.' << '\n';
		cout << "Key - " << (int)key << '.' << '\n';
	}
}

void ConfigParser::constructFileFromMap() {
	ofstream fileStream;
	fileStream.open(filename, ios::binary | ios::out);

	for (const auto& it : settingMap) {
		uint64_t action = (uint64_t)it.first;
		uint64_t key = (uint64_t)it.second;
		fileStream.write(reinterpret_cast<const char*>(&action), sizeof(uint64_t));
		fileStream.write(reinterpret_cast<const char*>(&key), sizeof(uint64_t));
	}

	fileStream.close();
}

void ConfigParser::readFromFile() {
	ifstream fileStream;
	fileStream.open(filename, ios::binary | ios::in);

	fileStream.seekg(0, fileStream.end);
	uint64_t charsLeft = fileStream.tellg();
	fileStream.seekg(0, fileStream.beg);

	while (charsLeft > 0) {
		uint64_t action;
		uint64_t key;
		fileStream.read(reinterpret_cast<char*>(&action), sizeof(uint64_t));
		fileStream.read(reinterpret_cast<char*>(&key), sizeof(uint64_t));
		settingMap.insert({ action, key });

		charsLeft -= bufferSize;
	}

	fileStream.close();
}

void ConfigParser::constructDefaults() {
	setKey(ActionKey::FORWARD, PGE::KeyboardInput::Keycode::W);
	setKey(ActionKey::BACK, PGE::KeyboardInput::Keycode::S);
	setKey(ActionKey::LEFT, PGE::KeyboardInput::Keycode::A);
	setKey(ActionKey::RIGHT, PGE::KeyboardInput::Keycode::D);
	setKey(ActionKey::NOCLIP, PGE::KeyboardInput::Keycode::N);
}