#include "pch.h"

#include <Kore/IO/FileReader.h>
#include <Kore/Graphics4/Graphics.h>
#include <Kore/Input/Keyboard.h>
#include <Kore/System.h>
#include <Kore/Log.h>
#include <Kore/Graphics1/Color.h>

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
	
	Kravur* font14;
	Kravur* font24;
	Kravur* font34;
	Kravur* font44;
	
	const char* helpText;
	const char* const stairsUp = "Key Up: walk the stairs up";
	const char* const stairsDown = "Key Down: walk the stairs down";
	
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
		//log(LogLevel::Info, "%f %f", playerPosition.x(), playerPosition.y());
		
		cat->update(playerPosition);
		
		helpText = nullptr;
		
		int tileID = getTileID(playerPosition.x(), playerPosition.y());
		//log(LogLevel::Info, "%i", tileID);
		if (tileID == Stairs3) {
			log(LogLevel::Info, "walk downstairs -> left");
			helpText = stairsDown;
		}
		if (tileID == Stairs4) {
			log(LogLevel::Info, "walk downstairs -> right");
			helpText = stairsDown;
		}
		if (tileID == Stairs1) {
			log(LogLevel::Info, "walk upstairs");
			helpText = stairsUp;
		}
		if (tileID == Stairs6) {
			log(LogLevel::Info, "walk upstairs -> left -> right");
			helpText = stairsUp;
		}
	}
	
	void moveGuy() {
		// TODO: guy should follow the cat
		guyPosition = playerPosition;
		
		guy->update(guyPosition);
	}
	
	void drawGUI() {
		g2->setFont(font14);
		g2->setFontColor(Graphics1::Color::Black);
		g2->setFontSize(14);
		
		if (helpText != nullptr) {
			g2->drawString(helpText, 0, 0);
		}
		
		g2->setColor(Graphics1::Color::White);
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
			
			cat->render(g2);
			//guy->render(g2);
			
			drawGUI();
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
	cat->init("Tiles/cat_walking_anim.png", 4, Animation::AnimationTyp::Walking);
	
	guyPosition = vec2(0, 0);
	guy = new Animation();
	guy->init("Tiles/player.png", 9, Animation::AnimationTyp::Walking);
	
	g2 = new Graphics2::Graphics2(w, h, false);
	
	left = false;
	right = false;
	up = false;
	down = false;
	
	font14 = Kravur::load("Fonts/arial", FontStyle(), 14);
	font24 = Kravur::load("Fonts/arial", FontStyle(), 24);
	font34 = Kravur::load("Fonts/arial", FontStyle(), 34);
	font44 = Kravur::load("Fonts/arial", FontStyle(), 44);
	
	state = InGameState;
	
	Keyboard::the()->KeyDown = keyDown;
	Keyboard::the()->KeyUp = keyUp;
	
	Kore::System::start();

	return 0;
}
