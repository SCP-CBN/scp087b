#include "StatWorld.h"

using namespace PGE;

#include "../Utilities/Directories.h"

StatWorld::StatWorld(TimeMaster& tm) : tm(tm) {
	graphics = Graphics::create("Stats", 420, 420, WindowMode::Windowed, { }, 0, 30); // Magic Windows title bar height.
	resources = new Resources(*graphics, cam);

	font = new Font(*resources, Directories::GFX + "Vegur");
	text = new TextRenderer(*resources, *font);
	
	constexpr float FONT_SCALE = 10.f;
	text->setScale(FONT_SCALE);
	text->setPosition(Vector2f(50.f, 50.f - font->getHeight() * FONT_SCALE));
}

StatWorld::~StatWorld() {
	delete text;
	delete font;
	delete resources;
	delete graphics;
}

bool StatWorld::shouldEnd() const {
	return !graphics->isWindowOpen();
}

void StatWorld::run() {
	graphics->update();
	graphics->clear(Colors::BLACK);
	text->setText(tm.print());
	text->render();
	graphics->swap();
}
