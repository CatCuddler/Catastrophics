#include "pch.h"

#include <Kore/IO/FileReader.h>
#include <Kore/Graphics4/Graphics.h>
#include <Kore/Input/Keyboard.h>
#include <Kore/System.h>
#include <Kore/Log.h>
#include <Kore/Graphics1/Color.h>
#include <Kore/Input/Mouse.h>
#include <Kore/Audio2/Audio.h>
#include <Kore/Audio1/Audio.h>
#include <Kore/Audio1/Sound.h>
#include <Kore/Audio1/SoundStream.h>

#include "Tileset.h"
#include "Animation.h"
#include "FallingObject.h"

using namespace Kore;



namespace {
	const int scale = 4;
	const int w = 128 * 2;
	const int h = 168;
	
	float px = 0;
	float py = 0;
	float camX = 0;
	float camY = 0;
	
	float pushUp = 4;
	int jumpPrep = 0;
	int maxJumpPrep = 10;
	int jumpFrames = 0;

	int maxJumpFrames = 20;
	float startHeight = -1;

	int droppedObjects = 0;
	const int maxDroppedObjects1 = 18;
	const int maxDroppedObjects2 = 0;
	const int maxDroppedObjects3 = 8;
	
	int level = 1;
	
	SoundStream* music;
	
	const int maxFallingObjects = 50;
	int fallingObjects0 = 0;
	int fallingObjects1 = 0;
	int fallingObjects2 = 0;
	FallingObject** fos0;
	FallingObject** fos1;
	FallingObject** fos2;
	Graphics4::Texture* gameOverImage;
	Graphics4::Texture* introImage;
	
	Graphics2::Graphics2* g2;
	
	Animation* cat_walk;
	Animation* cat_jump;
	Animation* cat_attack;
	vec2 playerCenter;
	float playerWidth, playerHeight;
	
	float targetYPosition;
	
	Animation* guy;
	vec2 guyPosition;
	
	int lastDirection = 1;	// 0 - left, 1 - right
	bool left, right, up, down, prep, jump, falling, attack;
	
	Kravur* font14;
	Kravur* font24;
	Kravur* font34;
	Kravur* font44;
	
	const char* helpText;
	const char* const stairsUp = "Key Up: walk the stairs up";
	const char* const stairsDown = "Key Down: walk the stairs down";
	const char* const jumpText = "Key Up: jump on the table";
	const char* const loadLevelText = "Key Up: load next level";
	const char* const cannotLoadNextLevel = "You did not sacrificed enough objects.";

	enum GameState {
		TitleState, InGameState, GameOverState
	};
	GameState state;
	
	void loadNextLevel() {
		char levelName[20];
		sprintf(levelName, "Tiles/level%i.csv", level);
		
		if (level == 1) {
			log(LogLevel::Info, "Load level %i", level);
			initTiles(levelName, "Tiles/tiles.png");
			++level;
		}
		else if (level == 2) {
			if (droppedObjects >= maxDroppedObjects1) {
				log(LogLevel::Info, "Load level %i", level);
				initTiles(levelName, "Tiles/kitchen.png");
				++level;
			} else {
				helpText = cannotLoadNextLevel;
			}
		}
		else if (level == 3) {
			if (droppedObjects >= maxDroppedObjects2) {
				log(LogLevel::Info, "Load level %i", level);
				initTiles(levelName, "Tiles/bath.png");
				++level;
			}
		}
		else if (level == 4) {
			if (droppedObjects >= maxDroppedObjects3) {
				state = GameOverState;
				log(LogLevel::Info, "Game over");
				++level;
			}
		}
		
		droppedObjects = 0;
	}
	
	void moveCatInTheMiddleOfTheTile() {
		vec2 tileCenter = getTileCenterBottom(playerCenter.x(), playerCenter.y());
		px = tileCenter.x() - playerWidth / 2;
		py = tileCenter.y() - playerHeight;
	}

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
		// Check if next level can be load
		else if (tileID == Door) {
			helpText = loadLevelText;
		}
		


		// Check if the cat can jump on the table
		if (cat_walk->status == Animation::Status::WalkingRight)
			tileID = getTileID(playerCenter.x() + tileWidth, playerCenter.y());
		if (cat_walk->status == Animation::Status::WalkingLeft)
			tileID = getTileID(playerCenter.x() - tileWidth, playerCenter.y());
		if (tileID == Globus1 || tileID == Candles1) {
			helpText = jumpText;
		}
		
		float moveDistance = 4;
		
		if (cat_walk->status != Animation::Status::WalkingDownLeft && cat_walk->status != Animation::Status::WalkingDownRight &&
			cat_walk->status != Animation::Status::WalkingUpLeft && cat_walk->status != Animation::Status::WalkingUpRight) {
			if (left && px >= -10) {
				px -= moveDistance;
				cat_walk->status = Animation::Status::WalkingLeft;
			} else if (right && px <= columns * tileWidth - playerWidth + 10) {
				px += moveDistance;
				cat_walk->status = Animation::Status::WalkingRight;
			} else if (up /*&& py >= tileHeight - playerHeight + moveDistance*/) {
				if (tileID == Stairs1) {
					moveCatInTheMiddleOfTheTile();
					targetYPosition = py - tileHeight;
					cat_walk->status = Animation::Status::WalkingUpRight;
				}
				if (tileID >= Stairs6) {
					moveCatInTheMiddleOfTheTile();
					px += 50;
					targetYPosition = py - tileHeight;
					cat_walk->status = Animation::Status::WalkingUpLeft;
				}
				if (tileID == Door) {
					loadNextLevel();
				}
			} else if (down /*&& py <= rows * tileHeight - playerHeight*/) {
				if (tileID == Stairs3) {
					moveCatInTheMiddleOfTheTile();
					targetYPosition = py + tileHeight;
					cat_walk->status = Animation::Status::WalkingDownLeft;
				}
				if (tileID == Stairs4) {
					moveCatInTheMiddleOfTheTile();
					targetYPosition = py + tileHeight;
					cat_walk->status = Animation::Status::WalkingDownRight;
				}
			}
		}
		//log(LogLevel::Info, "%f %f", playerPosition.x(), playerPosition.y());
		
		if (cat_walk->status == Animation::Status::WalkingDownLeft) {
			px -= moveDistance;
			py += moveDistance;
			if (py == targetYPosition) {
				cat_walk->status = Animation::Status::StandingLeft;
				lastDirection = 0;
			}
		}
		if (cat_walk->status == Animation::Status::WalkingUpRight) {
			px += moveDistance;
			py -= moveDistance;
			if (py == targetYPosition) {
				cat_walk->status = Animation::Status::StandingRight;
				lastDirection = 1;
			}
		}
		if (cat_walk->status == Animation::Status::WalkingDownRight) {
			px += moveDistance;
			py += moveDistance;
			if (py == targetYPosition) {
				cat_walk->status = Animation::Status::StandingRight;
				lastDirection = 1;
			}
		}
		if (cat_walk->status == Animation::Status::WalkingUpLeft) {
			px -= moveDistance;
			py -= moveDistance;
			if (py == targetYPosition) {
				cat_walk->status = Animation::Status::StandingLeft;
				lastDirection = 0;
			}
		}

		if (prep)
		{
			++jumpPrep;
			if (jumpPrep > maxJumpPrep)
			{
				jumpPrep = 0;
				prep = false;
				falling = false;
				jump = true;
			}
		}

		if (jump)
		{
			if (startHeight == -1)
			{
				startHeight = py;
			}
			py -= pushUp;
			++jumpFrames;

			if (jumpFrames > maxJumpFrames) {
				jumpFrames = 0;
				jump = false;
				prep = false;
				falling = true;
			}
		} else if (falling) {
			py += pushUp;
			
			if (py >= startHeight) {
				py = startHeight;
				falling = false;
				startHeight = -1;
			}
		}
		
		playerCenter = vec3(px + playerWidth / 2, py + playerHeight / 2);
		cat_walk->update(playerCenter);
		cat_jump->update(playerCenter);
		cat_attack->update(playerCenter);
		
	}
	
	void moveGuy() {
		// TODO: guy should follow the cat
		guyPosition = playerCenter;
		
		guy->update(guyPosition);
	}
	
	void drawGUI() {
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
		float targetCamY;
		if (cat_walk->status != Animation::WalkingUpLeft && cat_walk->status != Animation::WalkingDownLeft &&
			cat_walk->status != Animation::WalkingUpRight && cat_walk->status != Animation::WalkingDownRight)
			targetCamY = getFloor(py) * tileHeight;
		else
			targetCamY = py + playerHeight - tileHeight;//Kore::min(Kore::max(0.0f, playerCenter.y() - h / 2), 1.f * rows * tileHeight - h);
		
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
		//moveGuy();

		g2->begin(false, w, h);
		
		if (state == TitleState) {
			g2->drawImage(introImage, 0, 0);
			g2->drawString("Press enter to start the game", 0, 0);
		} else if (state == InGameState) {
			//camX = playerPosition.x();
			//camY = playerPosition.y();
			drawTiles(g2, camX, camY);

			if (level == 1 + 1) {
				for (int i = 0; i < fallingObjects0; ++i) fos0[i]->render(g2, camX, camY, w * scale, h * scale);
			}
			if (level == 2 + 1) {
				for (int i = 0; i < fallingObjects1; ++i) fos1[i]->render(g2, camX, camY, w * scale, h * scale);
			}
			if (level == 3 + 1) {
				for (int i = 0; i < fallingObjects2; ++i) fos2[i]->render(g2, camX, camY, w * scale, h * scale);
			}

			
			bool lastDir = lastDirection == 0;
			if (prep) {
				cat_jump->renderFrame(g2, 1, lastDir, camX, camY);
			} else if (jump) {
				if (!left && !right) cat_jump->renderFrame(g2, 0, lastDir,camX, camY);
				else cat_jump->renderFrame(g2, 2, lastDir,camX, camY);
			} else if (falling) {
				cat_jump->renderFrame(g2, 4, lastDir, camX, camY);
			} else if (attack) {
				cat_attack->renderFrame(g2, 1, lastDir, camX, camY);
			} else cat_walk->render(g2, camX, camY);
			//guy->render(g2);
			
			animateSpider(playerCenter.x(), playerCenter.y());
			drop(playerCenter.x(), playerCenter.y(), jump || falling);
			
			if (level == 1 + 1) {
				for (int i = 0; i < fallingObjects0; ++i) {
					if (attack) fos0[i]->update(playerCenter.x(), playerCenter.y() + 20, true);
					else fos0[i]->update(playerCenter.x(), playerCenter.y(), jump || falling);
					if (fos0[i]->isDroped()) ++droppedObjects;
				}
			} else if (level == 2 + 1) {
				for (int i = 0; i < fallingObjects1; ++i) {
					if (attack) fos1[i]->update(playerCenter.x(), playerCenter.y() + 20, true);
					else fos1[i]->update(playerCenter.x(), playerCenter.y(), jump || falling);
					if (fos1[i]->isDroped()) ++droppedObjects;
				}
			} else if (level == 3 + 1) {
				for (int i = 0; i < fallingObjects2; ++i) {
					if (attack) fos2[i]->update(playerCenter.x(), playerCenter.y()+20, true);
					else fos2[i]->update(playerCenter.x(), playerCenter.y(), jump || falling);
					if (fos2[i]->isDroped()) ++droppedObjects;
				}
			}
			if (level == 1) drawGUI();
		}
		else if (state == GameOverState) {
			g2->drawImage(gameOverImage, 0, 0);
		}
		
		g2->end();

		Graphics4::end();
		Graphics4::swapBuffers();
	}
	
	bool onStairs()
	{
		return cat_walk->status == Animation::Status::WalkingDownLeft ||
			cat_walk->status == Animation::Status::WalkingDownRight ||
			cat_walk->status == Animation::Status::WalkingUpLeft ||
			cat_walk->status == Animation::Status::WalkingUpRight;
	}

	void mousePress(int windowId, int button, int x, int y) {
		int posX = camX + x;
		int posY = camY + y;
		log(LogLevel::Info, "x: %i , y: %i", posX , posY);
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
			case KeySpace:
				if (!onStairs() && prep == false && jump == false && falling == false)
				{
					prep = true;
				}
				break;
			case KeyReturn:
				attack = true;
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
			case KeySpace:
				prep = false;
				if(jump) falling = true;
				jump = false;
				jumpFrames = 0;
				jumpPrep = 0;
				break;
			case KeyReturn:
				if (state == TitleState) state = InGameState;
				attack = false;
				break;
			default:
				break;
		}
	}
}

int kore(int argc, char** argv) {
	Kore::System::init("LudumDare43", w * scale, h * scale);
	Kore::System::setCallback(update);
	
	loadNextLevel();

	fos0 = new FallingObject*[maxFallingObjects];
	fos0[1] = new FallingObject(225, 100, 168, "bottle_green.png"); //check
	fos0[18] = new FallingObject(280, 100, 168, "books2.png"); //check
	fos0[21] = new FallingObject(333, 100, 168, "candles.png"); //chandalier here
	fos0[2] = new FallingObject(420, 100, 168, "glass_cup.png"); //chek
	fos0[5] = new FallingObject(448, 100, 168, "mug.png"); //check
	fos0[3] = new FallingObject(450, 149, 168, "bottle.png"); //check
	fos0[20] = new FallingObject(538, 82, 168, "books3.png"); //check
	fos0[0] = new FallingObject(552, 82, 168, "vase.png"); //check
	fos0[4] = new FallingObject(587, 149, 168, "bottle_green.png"); //check
	fos0[19] = new FallingObject(610, 83, 168, "books.png");//check
	fos0[7] = new FallingObject(783, 141, 168, "book_red.png"); //check
	fos0[23] = new FallingObject(776, 140, 168, "book_red.png"); //check
	fos0[24] = new FallingObject(776, 117, 168, "book_brown.png"); //check
	fos0[25] = new FallingObject(783, 118, 168, "book_brown.png"); //check
	fos0[6] = new FallingObject(808, 100, 168, "mug2.png"); // check
	fos0[8] = new FallingObject(874, 100, 168, "walkytalky.png"); //chek
	fos0[22] = new FallingObject(972, 120, 168, "duck_middle.png"); //chek

	fos0[9] = new FallingObject(12, 305, 168*2, "vase.png");
	fos0[10] = new FallingObject(278, 278, 168*2, "vase.png");
	fos0[11] = new FallingObject(310, 279, 168*2, "glass_cup.png");
	fos0[12] = new FallingObject(436, 211, 168*2, "book_red.png");
	fos0[13] = new FallingObject(512, 310, 168*2, "books.png");
	fos0[14] = new FallingObject(594, 307, 168*2, "books2.png");
	fos0[15] = new FallingObject(605, 191, 168*2, "book_brown.png");
	fos0[16] = new FallingObject(687, 258, 168*2, "lamp.png");
	fos0[17] = new FallingObject(778, 304, 168*2, "vase.png");
	fallingObjects0 = 26;
	
	fos1 = new FallingObject*[maxFallingObjects];
	fallingObjects1 = 0;

	fos2 = new FallingObject*[maxFallingObjects];
	fos2[0] = new FallingObject(431, 89, tileHeight, "duck_small.png"); //
	fos2[1] = new FallingObject(343, 95, tileHeight, "duck_middle.png"); //
	fos2[2] = new FallingObject(263, 54, tileHeight, "duck_small.png"); //
	fos2[3] = new FallingObject(125, 52, tileHeight, "duck_small.png"); //
	fos2[4] = new FallingObject(194, 54, tileHeight, "duck_middle.png"); //
	fos2[5] = new FallingObject(264, 126, tileHeight, "duck_small.png"); //
	
	fos2[6] = new FallingObject(166, 236, tileHeight*2, "duck_small.png"); //
	fos2[7] = new FallingObject(271, 218, tileHeight*2, "duck_middle.png"); //
	fos2[8] = new FallingObject(334, 202, tileHeight*2, "duck_small.png"); //
	fos2[9] = new FallingObject(77, 236, tileHeight*2, "duck_small.png"); //
	fos2[10] = new FallingObject(514, 218, tileHeight*2, "duck_middle.png"); //
	fos2[11] = new FallingObject(571, 218, tileHeight*2, "duck_small.png"); //
	fallingObjects2 = 12;

	cat_walk = new Animation();
	cat_walk->init("Tiles/cat_walking_anim.png", 5, Animation::AnimationTyp::Walking);
	playerWidth = cat_walk->getWidth();
	playerHeight = cat_walk->getHeight();
	cat_jump = new Animation();
	cat_jump->init("Tiles/cat_jumping_anim.png", 5, Animation::AnimationTyp::Jumping);
	cat_attack = new Animation();
	cat_attack->init("Tiles/cat_attack_anim.png", 2, Animation::AnimationTyp::Attacking);

	px = 0;
	py = tileHeight - playerHeight;
	playerCenter = vec3(px + playerWidth / 2, py + playerHeight / 2);
	
	guyPosition = vec2(0, 0);
	guy = new Animation();
	guy->init("Tiles/player.png", 9, Animation::AnimationTyp::Walking);
	
	g2 = new Graphics2::Graphics2(w, h, false);
	g2->setImageScaleQuality(Graphics2::Low);
	left = false;
	right = false;
	up = false;
	down = false;
	prep = false;
	jump = false;
	falling = false;
	attack = false;

	font14 = Kravur::load("Fonts/arial", FontStyle(), 14);
	font24 = Kravur::load("Fonts/arial", FontStyle(), 24);
	font34 = Kravur::load("Fonts/arial", FontStyle(), 34);
	font44 = Kravur::load("Fonts/arial", FontStyle(), 44);
	
	g2->setFont(font14);
	g2->setFontColor(Graphics1::Color::Black);
	g2->setFontSize(14);
	
	gameOverImage = new Graphics4::Texture("gameover.png");
	introImage = new Graphics4::Texture("intro.png");
	
	state = TitleState;
	
	Keyboard::the()->KeyDown = keyDown;
	Keyboard::the()->KeyUp = keyUp;
	Mouse::the()->Press = mousePress;
	
	Audio1::init();
	Audio2::init();
	music = new SoundStream("LeChat.ogg", true);
	Audio1::play(music);

	Kore::System::start();

	return 0;
}
