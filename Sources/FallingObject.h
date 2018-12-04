#pragma once
#include "pch.h"
#include <Kore/Graphics4/Graphics.h>
#include <Kore/Graphics2/Graphics.h>
#include <Kore/Math/Vector.h>
struct FallingObject{
	Kore::vec2 position;
	Kore::vec2 center;
	float radius;
	float height;
	float width;
	bool activated;
	int frameCount;
	int maxFrameCount;
	float fallingSpeed = 6;
	float rotation;
	float floorHeight;

	Kore::Graphics4::Texture* tex;

	FallingObject(float x, float y, float floorHeight, const char* texName);
	FallingObject(float x, float y, float with, float height, float floorHeight, Kore::Graphics4::Texture* tex);
	void update(float catX, float catY, bool activate);
	void render(Kore::Graphics2::Graphics2* render, float cameraX, float cameraY, float width, float height);
	bool isDroped();
};