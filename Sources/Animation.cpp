#include "pch.h"

#include "Animation.h"
#include "Tileset.h"

#include <Kore/Math/Random.h>
#include <Kore/Log.h>

using namespace Kore;

Animation::Animation() {
	
}

void Animation::init(const char* textureName, int animTiles, AnimationTyp typ) {
	texture = new Graphics4::Texture(textureName);
	width = texture->width / animTiles;
	height = texture->height;
	this->animTiles = animTiles;
	this->typ = typ;
}

void Animation::update() {
	++frameCount;
	if (frameCount > 10) {
		frameCount = 0;
		
		animIndex = animIndex % animTiles;
		++animIndex;
	}
}

void Animation::changeFloor() {
	
}

void Animation::render(Kore::Graphics2::Graphics2* g2, float posX, float poxY) {
	float px = posX;
	float py = tileHeight - height;
	
	switch (status) {
		case WalkingRight:
		case StandingRight: {
			g2->drawScaledSubImage(texture, (animIndex + 1) * width, 0, -width, height, px - posX, py - poxY, width, height);
			break;
		}
		case WalkingLeft:
		case StandingLeft: {
			g2->drawScaledSubImage(texture, animIndex * width, 0, width, height, px - posX, py - poxY, width, height);
			break;
		}
	}
}
