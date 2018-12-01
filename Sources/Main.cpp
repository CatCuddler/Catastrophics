#include "pch.h"

#include <Kore/IO/FileReader.h>
#include <Kore/Graphics4/Graphics.h>
#include <Kore/Graphics4/PipelineState.h>
#include <Kore/Graphics4/Shader.h>
#include <Kore/System.h>

#include "Tileset.h"

using namespace Kore;

namespace {
	const int w = 1280 / 2;
	const int h = 720 / 2;
	
	float camX = 0;
	float camY = 0;
	
	Graphics2::Graphics2* g2;
	
	Graphics4::Texture* cat;

	void update() {
		Graphics4::begin();
		Graphics4::clear(Graphics4::ClearColorFlag);

		g2->begin(true, w, h);
		//g2->drawImage(cat, 0, 0);
		
		drawTiles(g2, camX, camY);
		
		g2->end();

		Graphics4::end();
		Graphics4::swapBuffers();
	}
}

int kore(int argc, char** argv) {
	Kore::System::init("LudumDare43", w * 2, h * 2);
	Kore::System::setCallback(update);
	
	initTiles("Tiles/school.csv", "Tiles/school.png");
	
	//cat = new Graphics4::Texture("cat.png");
	
	g2 = new Graphics2::Graphics2(w, h, false);
	
	Kore::System::start();

	return 0;
}
