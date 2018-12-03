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
	position = vec2(width / 2, tileHeight + height/2);
}

void Animation::update(Kore::vec2 newPosition) {
	++frameCount;
	if (frameCount > 10) {
		frameCount = 0;
		
		++animIndex;
		animIndex = animIndex % (animTiles);
		
	}
	
	if (position.x() == newPosition.x()) {
		if (status == WalkingRight) status = StandingRight;
		else if (status == WalkingLeft) status = StandingLeft;
	}
	
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

void Animation::renderFrame(Kore::Graphics2::Graphics2* g2, int frame, float camX, float camY)
{
	vec2 pos = vec2(position.x() - (width / 2) - camX, position.y() - (height / 2) - camY);
	g2->drawScaledSubImage(texture, frame * width, 0, width, height, pos.x(), pos.y(), width, height);
}

void Animation::render(Kore::Graphics2::Graphics2* g2, float camX, float camY) {
	vec2 pos = vec2(position.x() - (width / 2) - camX, position.y() - (height / 2) - camY);
	
	float angle = Kore::pi / 10;
	
	switch (status) {
		case StandingRight:
			//log(LogLevel::Info, "Standing right");
			g2->drawScaledSubImage(texture, 0, 0, width, height, pos.x(), pos.y(), width, height);
			break;
		case StandingLeft:
			//log(LogLevel::Info, "Standing left");
			g2->drawScaledSubImage(texture, width, 0, -width, height, pos.x(), pos.y(), width, height);
			break;
		case WalkingRight: {
			//log(LogLevel::Info, "Walking right");
			g2->drawScaledSubImage(texture, animIndex * width, 0, width, height, pos.x(), pos.y(), width, height);
			break;
		}
		case WalkingLeft: {
			//log(LogLevel::Info, "Walking left");
			g2->drawScaledSubImage(texture, (animIndex + 1)  * width, 0, -width, height, pos.x(), pos.y(), width, height);
			break;
		case WalkingDownRight:
			//log(LogLevel::Info, "Walking down right");
			g2->pushRotation(angle, tileWidth / 2, tileHeight / 2);
			g2->drawScaledSubImage(texture, animIndex * width, 0, width, height, pos.x(), pos.y(), width, height);
			g2->transformation = Kore::mat3::Identity();
			break;
		case WalkingDownLeft:
			//log(LogLevel::Info, "Walking down left");
			g2->pushRotation(-angle, tileWidth / 2, tileHeight / 2);
			g2->drawScaledSubImage(texture, animIndex * width, 0, -width, height, pos.x(), pos.y(), width, height);
			g2->transformation = Kore::mat3::Identity();
			break;
		case WalkingUpRight:
			//log(LogLevel::Info, "Walking up right");
			g2->pushRotation(-angle, tileWidth / 2, tileHeight / 2);
			g2->drawScaledSubImage(texture, animIndex * width, 0, width, height, pos.x(), pos.y(), width, height);
			g2->transformation = Kore::mat3::Identity();
			break;
		case WalkingUpLeft:
			//log(LogLevel::Info, "Walking up left");
			g2->pushRotation(angle, tileWidth / 2, tileHeight / 2);
			g2->drawScaledSubImage(texture, animIndex * width, 0, -width, height, pos.x(), pos.y(), width, height);
			g2->transformation = Kore::mat3::Identity();
			break;
		}
	}
}
