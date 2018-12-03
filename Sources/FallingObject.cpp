#include "FallingObject.h"

FallingObject::FallingObject(float x, float y, float floorHeight, const char* texName)
{
	Kore::Graphics4::Texture* tex = new Kore::Graphics4::Texture(texName);
	this->width = tex->width;
	this->height = tex->height;
	this->position = Kore::vec2(x, y * 1.12f - height);
	this->tex = tex;
	this->activated = false;
	this->frameCount = 0;
	this->center = Kore::vec2(x + (width / 2), y + (height / 2));
	this->radius = Kore::sqrt(width*width + height * height) / 2;
	this->floorHeight = floorHeight;
	this->maxFrameCount = (floorHeight - position.y() + height) / fallingSpeed;
}

FallingObject::FallingObject(float x, float y, float with, float height, float floorHeight, Kore::Graphics4::Texture* tex)
{
	this->position = Kore::vec2(x, y);
	this->width = width;
	this->height = height;
	this->tex = tex;
	this->activated = false;
	this->frameCount = 0;
	this->center = Kore::vec2(x + (width / 2), y + (height / 2));
	this->radius = Kore::sqrt(width*width + height*height) / 2;
	this->floorHeight = floorHeight;
	this->maxFrameCount = (floorHeight - position.y()) / fallingSpeed;
}

void FallingObject::update(float catX, float catY, bool activate)
{
	if (!activated && activate)
	{
		Kore::vec2 cat(catX, catY);
		float dist = center.distance(cat);
		if (dist <= radius)
		{
			activated = true;
		}
	}
	if (activated && frameCount <= maxFrameCount)
	{
		++frameCount;
		position.y() += fallingSpeed;
		center = Kore::vec2(position.x() + (width / 2), position.y() + (height / 2));
	}
	if (position.y() + width > floorHeight)
	{
		position.y() = floorHeight-(height/2);
		center = Kore::vec2(position.x() + (width / 2), position.y() + (height / 2));
	}
}

void FallingObject::render(Kore::Graphics2::Graphics2* g2, float cameraX, float cameraY, float width, float height)
{
	if (cameraX < position.x() + this->width && position.x() < cameraX + width
		&& cameraY < position.y() && cameraY + height > position.y() + this->height)
	{
		float maxRotation = Kore::pi / 2;
		float currentRotation = ((float)frameCount / (float)maxFrameCount) * maxRotation;
		g2->pushRotation(currentRotation, center.x()-cameraX, center.y() - cameraY);
		g2->drawScaledSubImage(tex, 0, 0, this->width, this->height, position.x() - cameraX - this->width/2, position.y() - cameraY, this->width, this->height);
		g2->transformation = Kore::mat3::Identity();
	}
}

bool FallingObject::isDroped()
{
	return frameCount >= maxFrameCount;
}