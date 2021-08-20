#ifndef B_STATWORLD_H_INCLUDED
#define B_STATWORLD_H_INCLUDED

#include <PGE/Graphics/Graphics.h>

#include "../Graphics/Resources.h"
#include "../Graphics/Text/TextRenderer.h"
#include "../Timing/TimeMaster.h"
#include "../Graphics/Camera.h"

class StatWorld {
	public:
		StatWorld(TimeMaster& tm);
		~StatWorld();

		bool shouldEnd() const;
		void run();

	private:
		TimeMaster& tm;
		
		// TODO: Remove somehow?
		Camera cam = Camera(0.f, 0.f, 0.f);

		PGE::Graphics* graphics;
		Resources* resources;

		Font* font;
		TextRenderer* text;
};

#endif // B_STATWORLD_H_INCLUDED
