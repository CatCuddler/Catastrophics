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
	this->status = Animation::Status::StandingRight;
	this->animIndex = 0;
	this->frameCount = 0;
	position = vec2(0, 0);
}

void Animation::update(Kore::vec2 newPosition) {
	++frameCount;
	if (frameCount > 10) {
		frameCount = 0;
		
		animIndex = animIndex % (animTiles-1);
		++animIndex;
	}
	
	if (position.x() < newPosition.x()) status = WalkingRight;
	else if (position.x() > newPosition.x()) status = WalkingLeft;
	else if (position.x() == newPosition.x()) {
		if (status == WalkingRight) status = StandingRight;
		else if (status == WalkingLeft) status = StandingLeft;
	}
	
	position = newPosition;
}

void Animation::changeFloor() {
	
}

void Animation::render(Kore::Graphics2::Graphics2* g2) {
	float px = position.x();
	float py = tileHeight - height + position.y();
	
	switch (status) {
		case StandingRight:
			g2->drawScaledSubImage(texture, 0, 0, width, height, px - position.x(), py - position.y(), width, height);
			//log(LogLevel::Info, "Standing right");
			break;
		case WalkingRight: {
			g2->drawScaledSubImage(texture, animIndex * width, 0, width, height, px - position.x(), py - position.y(), width, height);
			//log(LogLevel::Info, "Walking right");
			break;
		}
		case StandingLeft:
			g2->drawScaledSubImage(texture, width, 0, -width, height, px - position.x(), py - position.y(), width, height);
			//log(LogLevel::Info, "Standing left");
			break;
		case WalkingLeft: {
			g2->drawScaledSubImage(texture, (animIndex + 1) * width, 0, -width, height, px - position.x(), py - position.y(), width, height);
			//log(LogLevel::Info, "Walking left");
			break;
		}
	}
}
