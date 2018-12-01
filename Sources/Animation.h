#pragma once

#include <Kore/Graphics2/Graphics.h>
#include <Kore/Graphics4/Graphics.h>

class Animation {
public:
	enum AnimationTyp {
		Sitting, Walking
	} animationTyp;
	
	Animation();
	void init(const char* textureName, int animTiles, AnimationTyp typ);
	void update();
	void render(Kore::Graphics2::Graphics2* g2, float posX, float posY);
	void changeFloor();
	
	float x, y;
	AnimationTyp typ;
	
private:
	int anim;
	int animIndex;
	int animTiles;
	
	Kore::Graphics4::Texture* texture;
	int width, height;
	
	int frameCount;
	int directionLock;
	
	enum Status {
		WalkingLeft, WalkingRight, StandingLeft, StandingRight
	} status;
	
};
