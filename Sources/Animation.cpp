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
		
		++animIndex;
		animIndex = animIndex % (animTiles);
		
	}
	
	if (position.x() < newPosition.x()) status = WalkingRight;
	else if (position.x() > newPosition.x()) status = WalkingLeft;
	else if (position.x() == newPosition.x()) {
		if (status == WalkingRight) status = StandingRight;
		else if (status == WalkingLeft) status = StandingLeft;
	}
	
	/*if (position.y() < newPosition.y()) status = WalkingDownLeft;
	else if (position.y() > newPosition.y()) status = WalkingUpRight;
	else if (position.y() == newPosition.y()) {
		if (status == WalkingDownLeft) status = StandingLeft;
		else if (status == WalkingLeft) status = StandingRight;
	}*/
	
	position = newPosition;
}

void Animation::changeFloor() {
	
}

float Animation::getHeight() const {
	return height;
}

float Animation::getWidth() const {
	return width;
}

void Animation::render(Kore::Graphics2::Graphics2* g2, float camX, float camY) {
	//float px = position.x();
	//float py = tileHeight - height + position.y();
	
	//camX -= width / 2;
	//camY = 0;
	
	vec2 pos = vec2(position.x() - (width / 2) - camX, position.y() - (height / 2) - camY);
	
	switch (status) {
		case StandingRight:
			g2->drawScaledSubImage(texture, 0, 0, width, height, pos.x(), pos.y(), width, height);
			//log(LogLevel::Info, "Standing right");
			break;
		case StandingLeft:
			g2->drawScaledSubImage(texture, width, 0, -width, height, pos.x(), pos.y(), width, height);
			//log(LogLevel::Info, "Standing left");
			break;
		case WalkingRight: {
			g2->drawScaledSubImage(texture, animIndex * width, 0, width, height, pos.x(), pos.y(), width, height);
			//log(LogLevel::Info, "Walking right");
			break;
		}
		case WalkingLeft: {
			g2->drawScaledSubImage(texture, (animIndex + 1)  * width, 0, -width, height, pos.x(), pos.y(), width, height);
			//log(LogLevel::Info, "Walking left");
			break;
		case WalkingDownRight:
			g2->pushRotation(Kore::pi / 4, tileWidth / 2, tileHeight / 2);
			
			
			//g2->drawScaledSubImage(texture, animIndex * width, 0, width, height, pos.x(), pos.y(), width, height);
			//g2->drawImage(texture, 0, 0);
			log(LogLevel::Info, "Walking down right");
			
			g2->transformation = Kore::mat3::Identity();
			
			break;
		case WalkingDownLeft:
			log(LogLevel::Info, "Walking down left");
			g2->pushRotation(-Kore::pi / 4, tileWidth / 2, tileHeight / 2);
			//g2->drawScaledSubImage(texture, animIndex * width, 0, -width, height, pos.x(), pos.y(), width, height);
			g2->transformation = Kore::mat3::Identity();
			break;
		case WalkingUpRight:
			g2->pushRotation(-Kore::pi / 4, tileWidth / 2, tileHeight / 2);
			//g2->drawScaledSubImage(texture, animIndex * width, 0, width, height, pos.x(), pos.y(), width, height);
			log(LogLevel::Info, "Walking up");
			g2->transformation = Kore::mat3::Identity();
			break;
		case WalkingUpLeft:
			g2->pushRotation(-Kore::pi / 4, tileWidth / 2, tileHeight / 2);
			//g2->drawScaledSubImage(texture, animIndex * width, 0, width, height, pos.x(), pos.y(), width, height);
			log(LogLevel::Info, "Walking up");
			g2->transformation = Kore::mat3::Identity();
			break;
		}
	}
}
