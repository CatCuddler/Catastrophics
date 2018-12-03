#pragma once

#include <Kore/Graphics2/Graphics.h>

using namespace Kore;

namespace {
	const char* tileFile;
	Graphics4::Texture* image;
	
	const int lightCount = 8;
	const int spiderCountMax = 1;
	const int tileWidth = 128;
	const int tileHeight = 168;
	const int spiderCooldownMax = 30;

	int spiderFrameCount = 0;
	vec2i spiderPos[spiderCountMax];
	int spiderState[spiderCountMax];
	int spiderCooldownCurr[spiderCountMax];
	int spiderCountCurr;
	
	int globusFrameCount = 0;
	vec2i globusPos;
	int globusState;
	
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
void animateGlobus(float px, float py);
	
enum TileID {Door = 0, Window1 = 1, Books1 = 2, Closet = 3, TableGlobus1 = 4, TableGlobus2 = 5, TableGlobus3 = 6, TableGlobus4 = 7, TableAndCandles = 8, Wall1 = 9, Wall2 = 10, Wall3 = 11, Spider1 = 12, Spider2 = 13, Spider3 = 14, Spider4 = 15, Spider5 = 16, Spider6 = 17, Spider7 = 18, Spider8 = 19, Books2 = 20, Laptop1 = 21, Window = 22, Wall4 = 23, PC = 24, Fridge = 25, Emtpy = 26, TV = 27, Wall5 = 28, Wall6 = 29, Plant = 30, Laptop2 = 31, Cactus = 32, Stairs1 = 33, Stairs2 = 34, Stairs3 = 35, Stairs4 = 36, Stairs5 = 37, Stairs6 = 38, Wall7 = 39, PC2 = 40, Sofa = 41};
