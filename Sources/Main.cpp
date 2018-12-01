#include "pch.h"

#include <Kore/IO/FileReader.h>
#include <Kore/Graphics4/Graphics.h>
#include <Kore/Input/Keyboard.h>
#include <Kore/System.h>
#include <Kore/Log.h>

#include "Tileset.h"

using namespace Kore;

namespace {
	const int w = 1280 / 2;
	const int h = 720 / 2;
	
	float camX = 0;
	float camY = 0;
	
	Graphics2::Graphics2* g2;
	
	Graphics4::Texture* cat;
	vec2 playerPosition;
	
	int lastDirection = 1;	// 0 - left, 1 - right
	bool left, right, up, down;
	
	void movePlayer() {
		float moveSpeed = 4;
		
		if (left) {
			playerPosition = vec2(playerPosition.x() -= moveSpeed, playerPosition.y());
		} else if (right) {
			playerPosition = vec2(playerPosition.x() += moveSpeed, playerPosition.y());
		} else if (up) {
			playerPosition = vec2(playerPosition.x(), playerPosition.y() += moveSpeed);
		} else if (down) {
			playerPosition = vec2(playerPosition.x(), playerPosition.y() -= moveSpeed);
		}
	}

	void update() {
		Graphics4::begin();
		Graphics4::clear(Graphics4::ClearColorFlag);
		
		movePlayer();

		g2->begin(true, w, h);
		//g2->drawImage(cat, 0, 0);
		
		camX = playerPosition.x();
		camY = playerPosition.y();
		Kore::log(Kore::LogLevel::Info, "%f %f", camX, camX);
		drawTiles(g2, camX, camY);
		
		g2->end();

		Graphics4::end();
		Graphics4::swapBuffers();
	}
	
	void keyDown(KeyCode code) {
		switch (code) {
			case KeyLeft:
			case KeyA:
				left = true;
				lastDirection = 0;
				break;
			case KeyRight:
			case KeyD:
				right = true;
				lastDirection = 1;
				break;
			case KeyDown:
			case KeyS:
				down = true;
				break;
			case KeyUp:
			case KeyW:
				up = true;
				break;
			default:
				break;
		}
	}
	
	void keyUp(KeyCode code) {
		switch (code) {
			case KeyLeft:
			case KeyA:
				left = false;
				break;
			case KeyRight:
			case KeyD:
				right = false;
				break;
			case KeyDown:
			case KeyS:
				down = false;
				break;
			case KeyUp:
			case KeyW:
				up = false;
				break;
			default:
				break;
		}
	}
}

int kore(int argc, char** argv) {
	Kore::System::init("LudumDare43", w * 2, h * 2);
	Kore::System::setCallback(update);
	
	initTiles("Tiles/school.csv", "Tiles/school.png");
	
	playerPosition = vec2(0, 0);
	//cat = new Graphics4::Texture("cat.png");
	
	g2 = new Graphics2::Graphics2(w, h, false);
	
	left = false;
	right = false;
	up = false;
	down = false;
	
	Keyboard::the()->KeyDown = keyDown;
	Keyboard::the()->KeyUp = keyUp;
	
	Kore::System::start();

	return 0;
}
