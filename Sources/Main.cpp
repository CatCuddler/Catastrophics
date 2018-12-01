#include "pch.h"

#include <Kore/IO/FileReader.h>
#include <Kore/Graphics4/Graphics.h>
#include <Kore/Input/Keyboard.h>
#include <Kore/System.h>
#include <Kore/Log.h>

#include "Tileset.h"

using namespace Kore;

namespace {
	const int scale = 4;
	const int w = 128 * 2;
	const int h = 168;
	
	float camX = 0;
	float camY = 0;
	
	Graphics2::Graphics2* g2;
	
	Graphics4::Texture* cat;
	vec2 playerPosition;
	
	Graphics4::Texture* guy;
	float guyWidth;
	float guyHeight;
	vec2 guyPosition;
	int guyFrameCount;
	int guyRunIndex;
	
	int lastDirection = 1;	// 0 - left, 1 - right
	bool left, right, up, down;
	
	enum GameState {
		TitleState, InGameState, GameOverState
	};
	GameState state;
	
	void movePlayer() {
		float moveDistance = 4;
		
		if (left && playerPosition.x() >= moveDistance) {
			playerPosition = vec2(playerPosition.x() -= moveDistance, playerPosition.y());
		} else if (right && playerPosition.x() <= columns * tileWidth - w - moveDistance) {
			playerPosition = vec2(playerPosition.x() += moveDistance, playerPosition.y());
		} else if (up && playerPosition.y() >= moveDistance) {
			playerPosition = vec2(playerPosition.x(), playerPosition.y() -= moveDistance);
		} else if (down && playerPosition.y() <= rows * tileHeight - h - moveDistance) {
			playerPosition = vec2(playerPosition.x(), playerPosition.y() += moveDistance);
		}
		
		//Kore::log(Kore::LogLevel::Info, "%f %f", playerPosition.x(), playerPosition.y());
	}
	
	void moveGuy() {
		++guyFrameCount;
		if (guyFrameCount > 10) {
			guyFrameCount = 0;
			
			guyRunIndex = guyRunIndex % 8;
			++guyRunIndex;
		}
		
		float px = playerPosition.x();
		float py = tileHeight - guyHeight;
		// Render guy --> guy should follow the cat
		if (left)
			g2->drawScaledSubImage(guy, (guyRunIndex + 1) * guyWidth, 0, -guyWidth, guyHeight, px - camX, py - camY, guyWidth, guyHeight);
		else if (right)
			g2->drawScaledSubImage(guy, guyRunIndex * guyWidth, 0, guyWidth, guyHeight, px - camX, py - camY, guyWidth, guyHeight);
		else if (lastDirection == 0)
			g2->drawScaledSubImage(guy, guyWidth, 0, -guyWidth, guyHeight, px - camX, py - camY, guyWidth, guyHeight);
		else if (lastDirection == 1)
			g2->drawScaledSubImage(guy, 0, 0, guyWidth, guyHeight, px - camX, py - camY, guyWidth, guyHeight);
	}

	void update() {
		Graphics4::begin();
		Graphics4::restoreRenderTarget();
		Graphics4::clear(Graphics4::ClearColorFlag);
		
		movePlayer();

		g2->begin(false, w, h);
		//g2->drawImage(cat, 0, 0);
		
		if (state == TitleState) {
			log(LogLevel::Info, "Add title screen");
		} else if (state == InGameState) {
			camX = playerPosition.x();
			camY = playerPosition.y();
			drawTiles(g2, camX, camY);
			moveGuy();
		} else if (state == GameOverState) {
			log(LogLevel::Info, "Add game over screen");
			//g2->drawImage(gameOverImage, 0, 0);
		}
		
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
	Kore::System::init("LudumDare43", w * scale, h * scale);
	Kore::System::setCallback(update);
	
	initTiles("Tiles/tiles.csv", "Tiles/tiles.png");
	
	playerPosition = vec2(0, 0);
	//cat = new Graphics4::Texture("cat.png");
	
	guy = new Graphics4::Texture("Tiles/player.png");
	guyWidth = guy->width / 10.0f;
	guyHeight = guy->height;
	guyPosition = vec2(0, 0);
	guyFrameCount = 0;
	
	g2 = new Graphics2::Graphics2(w, h, false);
	
	left = false;
	right = false;
	up = false;
	down = false;
	
	state = InGameState;
	
	Keyboard::the()->KeyDown = keyDown;
	Keyboard::the()->KeyUp = keyUp;
	
	Kore::System::start();

	return 0;
}
