#include "pch.h"

#include <Kore/IO/FileReader.h>
#include <Kore/Graphics4/Graphics.h>
#include <Kore/Input/Keyboard.h>
#include <Kore/System.h>
#include <Kore/Log.h>

#include "Tileset.h"
#include "Animation.h"

using namespace Kore;

namespace {
	const int scale = 4;
	const int w = 128 * 2;
	const int h = 168;
	
	float camX = 0;
	float camY = 0;
	
	Graphics2::Graphics2* g2;
	
	Animation* cat;
	vec2 playerPosition;
	
	Animation* guy;
	vec2 guyPosition;
	
	int lastDirection = 1;	// 0 - left, 1 - right
	bool left, right, up, down;
	
	enum GameState {
		TitleState, InGameState, GameOverState
	};
	GameState state;
	
	void moveCat() {
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
		
		cat->update();
		//Kore::log(Kore::LogLevel::Info, "%f %f", playerPosition.x(), playerPosition.y());
	}
	
	void moveGuy() {
		// TODO: guy should follow the cat
		guyPosition = playerPosition;
		
		guy->update();
	}

	void update() {
		Graphics4::begin();
		Graphics4::restoreRenderTarget();
		Graphics4::clear(Graphics4::ClearColorFlag);
		
		moveCat();
		moveGuy();

		g2->begin(false, w, h);
		
		if (state == TitleState) {
			log(LogLevel::Info, "Add title screen");
		} else if (state == InGameState) {
			camX = playerPosition.x();
			camY = playerPosition.y();
			drawTiles(g2, camX, camY);
			
			cat->render(g2, playerPosition.x(), playerPosition.y());
			guy->render(g2, guyPosition.x(), guyPosition.y());
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
	cat = new Animation();
	cat->init("Tiles/cat_skel_anim.png", 4, Animation::AnimationTyp::Walking);
	
	guyPosition = vec2(0, 0);
	guy = new Animation();
	guy->init("Tiles/player.png", 10, Animation::AnimationTyp::Walking);
	
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
