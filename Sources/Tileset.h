#pragma once

#include <Kore/Graphics2/Graphics.h>

using namespace Kore;

namespace {
	const char* tileFile;
	Graphics4::Texture* image;
	
	const int lightCount = 8;
	const int spiderCountMax = 1;
	const int dropCountMax = 10;
	const int tileWidth = 128;
	const int tileHeight = 168;
	const int spiderCooldownMax = 30;

	int spiderFrameCount = 0;
	vec2i spiderPos[spiderCountMax];
	int spiderState[spiderCountMax];
	int spiderCooldownCurr[spiderCountMax];
	int spiderCountCurr;
	
	int objectFrameCount = 0;
	vec2i objectPositions[dropCountMax];
	int objectState[dropCountMax];
	int objectCountCurr;
	
	const int rows = 2;
	const int columns = 8;
	
	int* source;

	vec2* doors;
	int doorCount;
}

void loadCsv(const char* csvFile);

void initTiles(const char* csvFile, const char* tileFile);
void drawTiles(Graphics2::Graphics2* g2, float camX, float camY);

int getFloor(float py);
int getTileID(float px, float py);
int getTileIndex(float px, float py);
vec2 getTileCenterBottom(float px, float py);
vec2 findDoor(float lastX, float lastY);

void resetSpiders();
bool animateSpider(float px, float py);
void drop(float px, float py);

enum TileID {Door = 0, TV = 1, Books1 = 2, Closet = 3, Globus1 = 4, Globus2 = 5, Globus3 = 6, Globus4 = 7, Candles1 = 8, Candles2 = 9, Candles3 = 10, Candles4 = 11, Spider1 = 12, Spider2 = 13, Spider3 = 14, Spider4 = 15, Books2 = 16, Laptop = 17, Window1 = 18, Window2 = 19, PC1 = 20, PC2 = 21, Sofa = 22, Plant1 = 23, Laptop2 = 24, Plant2 = 25, Stairs1 = 26, Stairs2 = 27, Stairs3 = 28, Stairs4 = 29, Stairs5 = 30, Stairs6 = 31, /*Cactus = 32, Stairs1 = 33, Stairs2 = 34, Stairs3 = 35, Stairs4 = 36, Stairs5 = 37, Stairs6 = 38, Wall7 = 39, PC2 = 40, Sofa = 41*/};
