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
	
	float px = 0;
	float py = 0;
	float camX = 0;
	float camY = 0;
	
	Graphics2::Graphics2* g2;
	
	Animation* cat;
	vec2 playerCenter;
	bool moveDownLeft;
	float playerWidth, playerHeight;
	
	float targetYPosition;
	
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
		int tileID = getTileID(playerCenter.x(), playerCenter.y());
		//log(LogLevel::Info, "%i", tileID);
		
		helpText = nullptr;
		if (tileID == Stairs3) {
			//log(LogLevel::Info, "walk downstairs -> left");
			helpText = stairsDown;
		}
		else if (tileID == Stairs4) {
			//log(LogLevel::Info, "walk downstairs -> right");
			helpText = stairsDown;
		}
		else if (tileID == Stairs1) {
			//log(LogLevel::Info, "walk upstairs -> right");
			helpText = stairsUp;
		}
		else if (tileID == Stairs6) {
			//log(LogLevel::Info, "walk upstairs -> left");
			helpText = stairsUp;
		}
		
		float moveDistance = 4;
		
		if (!moveDownLeft) {
			if (left && px >= -10) {
				px -= moveDistance;
			} else if (right && px <= columns * tileWidth - playerWidth + 10) {
				px += moveDistance;
			} else if (up && py >= tileHeight - playerHeight + moveDistance) {
				if (tileID >= Stairs1 && tileID <= Stairs3) {
					log(LogLevel::Info, "walk upstairs -> right");
					py -= moveDistance;
				}
			} else if (down && py <= rows * tileHeight - playerHeight) {
				if (tileID == Stairs3) {
					targetYPosition = py + tileHeight;
					moveDownLeft = true;
				}
			}
		}
		//log(LogLevel::Info, "%f %f", playerPosition.x(), playerPosition.y());
		
		if (moveDownLeft) {
			if (py == targetYPosition) {
				moveDownLeft = false;
				px -= moveDistance;
			} else {
				px -= moveDistance;
				py += moveDistance;
			}
		}
		
		playerCenter = vec3(px + playerWidth / 2, py + playerHeight / 2);
		cat->update(playerCenter);
	}
	
	void moveGuy() {
		// TODO: guy should follow the cat
		guyPosition = playerCenter;
		
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
		
		float targetCamX = Kore::min(Kore::max(0.0f, playerCenter.x() - w / 2), 1.f * columns * tileWidth - w);
		float targetCamY = Kore::min(Kore::max(0.0f, playerCenter.y() - h / 2), 1.f * rows * tileHeight - h);
		
		vec2 cam(camX, camY);
		vec2 target(targetCamX, targetCamY);
		vec2 dir = target - cam;
		if (dir.getLength() < 16.0f) {
			camX = targetCamX;
			camY = targetCamY;
		} else {
			dir.setLength(15.0f);
			cam = cam + dir;
			camX = cam.x();
			camY = cam.y();
		}
		
		moveCat();
		moveGuy();

		g2->begin(false, w, h);
		
		if (state == TitleState) {
			log(LogLevel::Info, "Add title screen");
		} else if (state == InGameState) {
			//camX = playerPosition.x();
			//camY = playerPosition.y();
			drawTiles(g2, camX, camY);
			
			cat->render(g2, camX, camY);
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
	
	cat = new Animation();
	cat->init("Tiles/cat_walking_anim.png", 4, Animation::AnimationTyp::Walking);
	playerWidth = cat->getWidth();
	playerHeight = cat->getHeight();
	px = 0;
	py = tileHeight - playerHeight;
	playerCenter = vec3(px + playerWidth / 2, py + playerHeight / 2);
	moveDownLeft = false;
	
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
