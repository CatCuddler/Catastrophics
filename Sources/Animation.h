#pragma once

#include <Kore/Graphics2/Graphics.h>
#include <Kore/Graphics4/Graphics.h>

class Animation {
public:
	enum AnimationTyp {
		Sitting, Walking
	} animationTyp;
	
	enum Status {
		WalkingLeft, WalkingRight, StandingLeft, StandingRight, WalkingDownRight, WalkingUpRight, WalkingDownLeft, WalkingUpLeft
	} status;
	
	Animation();
	void init(const char* textureName, int animTiles, AnimationTyp typ);
	void update(Kore::vec2 position);
	void render(Kore::Graphics2::Graphics2* g2, float camX, float camY);
	void changeFloor();
	float getHeight() const;
	float getWidth() const;
	
	float x, y;
	AnimationTyp typ;
	
private:
	int animIndex;
	int animTiles;
	
	Kore::vec2 position;
	
	Kore::Graphics4::Texture* texture;
	int width, height;
	
	int frameCount;
	int directionLock;
	
	
	
};
