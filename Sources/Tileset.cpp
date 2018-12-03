#include "pch.h"

#include "Tileset.h"

#include <Kore/IO/FileReader.h>
#include <Kore/Log.h>
#include <Kore/Graphics1/Image.h>
#include <Kore/TextureImpl.h>
#include <Kore/Math/Random.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

void initTiles(const char* csvFilePath, const char* tileFilePath) {
	tileFile = tileFilePath;
	loadCsv(csvFilePath);
	image = new Graphics4::Texture(tileFilePath);
}

void loadCsv(const char* csvFile) {
	FileReader file(csvFile);
	
	void* data = file.readAll();
	int length = file.size();
	
	char* cpyData = new char[length + 1];
	for (int i = 0; i < length; ++i) {
		cpyData[i] = ((char*)data)[i];
	}
	cpyData[length] = 0;
	
	source = new int[rows * columns];
	int i = 0;
	
	char delimiter[] = ",;\n";
	char* ptr = std::strtok(cpyData, delimiter);
	while (ptr != nullptr) {
		assert(i < rows * columns);
		int num = atoi(ptr);
		//log(Info, "%i -> %i", i, num);
		source[i] = num;
		ptr = std::strtok(nullptr, delimiter);
		i++;
	}

	doorCount = 0;
	spiderCountCurr = 0;
	objectCountCurr = 0;
	doors = new vec2[(rows - 1) * 2];
	for (int y = 0; y < rows; ++y) {
		for (int x = 0; x < columns; ++x) {
			int index = source[y * columns + x];
			if (index == Door) {
				doors[doorCount] = vec2(x * tileWidth, y * tileHeight);
				++doorCount;
			}
			else if (index >= Spider1 && index <= Spider8) {
				assert(spiderCountCurr < spiderCountMax);
				spiderPos[spiderCountCurr] = vec2i(x, y);
				spiderState[spiderCountCurr] = Spider1;
				spiderCooldownCurr[spiderCountCurr] = 0;
				++spiderCountCurr;
			}
			else if (index == TableGlobus1 || index == TableAndCandles1) {
				assert(objectCountCurr < dropCountMax);
				objectPositions[objectCountCurr] = vec2i(x, y);
				objectState[objectCountCurr] = index;
				++objectCountCurr;
			}
		}
	}
}

void resetSpiders() {
	spiderFrameCount = 0;
	for (int i = 0; i < spiderCountCurr; ++i) {
		spiderState[i] = Spider1;
		spiderCooldownCurr[i] = 0;
	}
}

void drawTiles(Graphics2::Graphics2* g2, float camX, float camY) {
	const int sourceColumns = image->texWidth / tileWidth;
	const int sourceRows = image->texHeight / tileHeight;
	
	for (int y = 0; y < rows; ++y) {
		for (int x = 0; x < columns; ++x) {
			int index = source[y * columns + x];

			int row    = (int)(index / sourceColumns);
			int column = index % sourceColumns;
			
			g2->drawScaledSubImage(image, column * tileWidth, row * tileHeight, tileWidth, tileHeight, x * tileWidth - camX, y * tileHeight - camY, tileWidth, tileHeight);
		}
	}
}

bool animateSpider(float px, float py) {
	bool caughtPlayer = false;
	++spiderFrameCount;

	bool doMove = false;
	if (spiderFrameCount >= 5) {
		doMove = true;
		spiderFrameCount = 0;
	}

	for (int i = 0; i < spiderCountCurr; ++i) {
		int collx = (spiderPos[i].x() + .5f) * tileWidth;
		int collyMin = spiderPos[i].y() * tileHeight + 9 + (spiderState[i] - Spider1) * 11;
		spiderCooldownCurr[i] -= 1;
		if (doMove) {
			bool inRange = abs(collx - px) <= tileWidth;
			bool active = inRange ;
			if (active && spiderState[i] < Spider8 && spiderCooldownCurr[i] <= 0)
				++spiderState[i];
			else if (!active && spiderState[i] > Spider1)
				--spiderState[i];
			source[spiderPos[i].y() * columns  + spiderPos[i].x()] = spiderState[i];
		}
		caughtPlayer |= (spiderState[i] >= Spider3 && Kore::abs(collx - px) < tileWidth * 0.25f && getFloor(collyMin) == getFloor(py));
	}
	return caughtPlayer;
}

void drop(float px, float py) {
	
	++objectFrameCount;
	
	bool doMove = false;
	if (objectFrameCount >= 5) {
		doMove = true;
		objectFrameCount = 0;
	}
	
	for (int i = 0; i < objectCountCurr; ++i) {
		if (doMove) {
			int collx = (objectPositions[i].x() + .5f) * tileWidth;
			bool inRange = abs(collx - px) <= tileWidth / 4;
			if (inRange &&
				((objectState[i] >= TableGlobus1 && objectState[i] < TableGlobus4) ||
				(objectState[i] >= TableAndCandles1 && objectState[i] < TableAndCandles4)))
				++objectState[i];
			source[objectPositions[i].y() * columns  + objectPositions[i].x()] = objectState[i];
		}
	}
}

int getFloor(float py) {
	return ((int)py) / tileHeight;
}

int getTileID(float px, float py) {
	int index = getTileIndex(px, py);
	return source[index];
}

int getTileIndex(float px, float py) {
	int x = px / tileWidth;
	int y = py / tileHeight;
	return y * columns + x;
}

vec2 getTileCenterBottom(float px, float py) {
	int x = px / tileWidth;
	int y = py / tileHeight;
	
	float tileCenterX = x * tileWidth + tileWidth / 2;
	float tileBottomY = y * tileHeight + tileHeight;
	
	return vec2(tileCenterX, tileBottomY);
}

vec2 findDoor(float lastX, float lastY) {
	int compIndex = getTileIndex(lastX, lastY);
	for (int i = 0; i < doorCount; ++i) {
		int index = getTileIndex(doors[i].x(), doors[i].y());
		if (index == compIndex) {
			if (i % 2 == 0)
				return doors[i + 1];
			else
				return doors[i - 1];
		}
	}
	return doors[doorCount - 1];
}
