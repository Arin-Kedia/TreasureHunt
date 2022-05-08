/*This source code copyrighted by Lazy Foo' Productions (2004-2022)
and may not be redistributed without written permission.*/

// Using SDL, SDL_image, standard IO, strings, and file streams
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include "maze.h"
#include <stdio.h>
#include <string.h> //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> //close
#include <arpa/inet.h> //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#define SA struct sockaddr

#define TRUE 1
#define FALSE 0
#define PORT 8082

using namespace std;

// Screen dimension constants
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 500;

const int LEVEL_WIDTH_MAIN = 4760;
const int LEVEL_HEIGHT_MAIN = 1920;
const int LEVEL_WIDTH_MAZE = 2480;
const int LEVEL_HEIGHT_MAZE = 2480;
const int LEVEL_WIDTH_REST = 2240;
const int LEVEL_HEIGHT_REST = 1280;
const int LEVEL_WIDTH_HOST = 2440;
const int LEVEL_HEIGHT_HOST = 2040;

const int TOTAL_TILES_MAIN = 5712;
const int TOTAL_TILES_MAZE = 3844;
const int TOTAL_TILES_REST = 1792;
const int TOTAL_TILES_HOST = 3111;

// The dimensions of the level
int LEVEL_WIDTH = 4760;
int LEVEL_HEIGHT = 1920;

// Tile constants
const int TILE_WIDTH = 40;
const int TILE_HEIGHT = 40;
int TOTAL_TILES = 5712;
const int TOTAL_TILE_SPRITES = 13;

// The different tile sprites
const int TILE_RED = 0;
const int TILE_GREEN = 1;
const int TILE_BLUE = 2;
const int TILE_CENTER = 3;
const int TILE_TOP = 4;
const int TILE_TOPRIGHT = 5;
const int TILE_RIGHT = 6;
const int TILE_BOTTOMRIGHT = 7;
const int TILE_BOTTOM = 8;
const int TILE_BOTTOMLEFT = 9;
const int TILE_LEFT = 10;
const int TILE_TOPLEFT = 11;
const int TILE_RESGATE = 12;

char message[80] = {'0'};
char receive[80] = {'1'};
char* receive1;

// Texture wrapper class
class LTexture
{
public:
	// Initializes variables
	LTexture();

	// Deallocates memory
	~LTexture();

	// Loads image at specified path
	bool loadFromFile(string path);

#if defined(SDL_TTF_MAJOR_VERSION)
	// Creates image from font string
	bool loadFromRenderedText(string textureText, SDL_Color textColor);
#endif

	// Deallocates texture
	void free();

	// Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	// Set blending
	void setBlendMode(SDL_BlendMode blending);

	// Set alpha modulation
	void setAlpha(Uint8 alpha);

	// Renders texture at given point
	void render(int x, int y, SDL_Rect *clip = NULL, double angle = 0.0, SDL_Point *center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

	// Gets image dimensions
	int getWidth();
	int getHeight();

	// The actual hardware texture
	SDL_Texture *mTexture;

	// Image dimensions
	int mWidth;
	int mHeight;
};

// The tile
class Tile
{
public:
	// Initializes position and type
	Tile(int x, int y, int tileType);

	// Shows the tile
	void render(SDL_Rect &camera);

	// Get the tile type
	int getType();

	// Get the collision box
	SDL_Rect getBox();

	// The attributes of the tile
	SDL_Rect mBox;

	// The tile type
	int mType;
};

// The dot that will move around on the screen
class Dot
{
public:
	// The dimensions of the dot
	static const int DOT_WIDTH = 20;
	static const int DOT_HEIGHT = 20;

	// Maximum axis velocity of the dot
	static const int DOT_VEL = 5;

	// Initializes the variables
	Dot();

	// Takes key presses and adjusts the dot's velocity
	void handleEvent(SDL_Event &e);

	string information();

	// Moves the dot and check collision against tiles
	void move(Tile *tiles[]);

	// Centers the camera over the dot
	void setCamera(SDL_Rect &camera);

	// Shows the dot on the screen
	void render(SDL_Rect &camera);

	int tp;

	// Collision box of the dot
	SDL_Rect mBox;

	// The velocity of the dot
	int mVelX, mVelY;
};

// Starts up SDL and creates window
bool init();

// Loads media
bool loadMedia(Tile *tiles[]);

// Frees media and shuts down SDL
void close(Tile *tiles[]);

// Box collision detector
bool checkCollision(SDL_Rect a, SDL_Rect b);

// Checks collision box against set of tiles
bool touchesWall(SDL_Rect box, Tile *tiles[]);

// Sets tiles from tile map
bool setTiles(Tile *tiles[], string s);

int getTileIndex(int x, int y);

int getTileIndexMaze(int x, int y);

int getTileIndexRest(int x, int y);

int getTileIndexHost(int x, int y);

void rendermaze();

void renderrest();

void renderhost();

void changeMoney();

int getbuild(int x, int y);

// The window we'll be rendering to
SDL_Window *gWindow = NULL;

// The window renderer
SDL_Renderer *gRenderer = NULL;

// Globally used font
TTF_Font *gFont = NULL;

// Scene textures
LTexture gDot1Texture;
LTexture gDot2Texture;
LTexture gTileTexture;
LTexture gTextTexture;

SDL_Rect *gTileClips = new SDL_Rect[TOTAL_TILE_SPRITES];

bool isYuluOn = false;
bool isSleeping = false;

bool inmaze = false;
bool inrest = false;
bool inhost = false;
bool inmap = true;

int money = 20;

int locx;
int locy;

int hunger = 0;
int hungerf;
int fatigue = 0;
int fatiguef;

int feedtime = 0;
int sleeptime = 0;

Uint32 startFeed = SDL_GetTicks();
Uint32 startSleep = SDL_GetTicks();

Uint32 startYuluTime = 0;

// The level tiles
Tile *tileSetMain[TOTAL_TILES_MAIN];
Tile *tileSetMaze[TOTAL_TILES_MAZE];
Tile *tileSetRest[TOTAL_TILES_REST];
Tile *tileSetHost[TOTAL_TILES_HOST];
Tile **tileSet;

std::string buildings[21][2] = {
	{"Jwalamukhi Hostel!", "1"},
	{"Aravali Hostel!", "1"},
	{"Karakoram Hostel!", "1"},
	{"Nilgiri Hostel!", "1"},
	{"Kumaon Hostel!", "1"},
	{"Student Activity Centre!", "1"},
	{"Shivalik Hostel!", "1"},
	{"Satpura Hostel!", "1"},
	{"Udaigiri Hostel!", "1"},
	{"IIT Hospital!", "1"},
	{"Food Outlet (Rajdhani)!", "1"},
	{"Kailash Hostel!", "1"},
	{"Central Library!", "1"},
	{"Food Outlet (Amul)!", "1"},
	{"Food Outlet (Nescafe)!", "1"},
	{"Academic Area!", "1"},
	{"Academic Area!", "1"},
	{"IIT Main Building!", "1"},
	{"Lecture Hall Complex!", "1"},
	{"Block 99B!", "1"},
	{"Block 99C!", "1"},
};

LTexture::LTexture()
{
	// Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	// Deallocate
	free();
}

bool LTexture::loadFromFile(string path)
{
	// Get rid of preexisting texture
	free();

	// The final texture
	SDL_Texture *newTexture = NULL;

	// Load image at specified path
	SDL_Surface *loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		// Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0xFF, 0xFF, 0xFF));

		// Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			// Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		// Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	// Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

bool LTexture::loadFromRenderedText(string textureText, SDL_Color textColor)
{
	// Get rid of preexisting texture
	free();

	// Render text surface
	SDL_Surface *textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
	if (textSurface != NULL)
	{
		// Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (mTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			// Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		// Get rid of old surface
		SDL_FreeSurface(textSurface);
	}
	else
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}

	// Return success
	return mTexture != NULL;
}

void LTexture::free()
{
	// Free texture if it exists
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	// Modulate texture rgb
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
	// Set blending function
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
	// Modulate texture alpha
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y, SDL_Rect *clip, double angle, SDL_Point *center, SDL_RendererFlip flip)
{
	// Set rendering space and render to screen
	SDL_Rect renderQuad = {x, y, mWidth, mHeight};

	// Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	// Render to screen
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

Tile::Tile(int x, int y, int tileType)
{
	// Get the offsets
	mBox.x = x;
	mBox.y = y;

	// Set the collision box
	mBox.w = TILE_WIDTH;
	mBox.h = TILE_HEIGHT;

	// Get the tile type
	mType = tileType;
}

void Tile::render(SDL_Rect &camera)
{
	// If the tile is on screen
	if (checkCollision(camera, mBox))
	{
		// Show the tile
		gTileTexture.render(mBox.x - camera.x, mBox.y - camera.y, &gTileClips[mType]);
	}
}

int Tile::getType()
{
	return mType;
}

SDL_Rect Tile::getBox()
{
	return mBox;
}

Dot::Dot()
{
	// Initialize the collision box
	mBox.x = 370;
	mBox.y = 0;
	mBox.w = DOT_WIDTH;
	mBox.h = DOT_HEIGHT;
	tp = 0;

	// Initialize the velocity
	mVelX = 0;
	mVelY = 0;
}

void Dot::handleEvent(SDL_Event &e)
{
	// If a key was pressed
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		int i, j;
		int tilelocx = (int)(mBox.x / 40) * 40;
		int tilelocy = (int)(mBox.y / 40) * 40;
		if (inmap)
		{
			i = getTileIndex(mBox.x, mBox.y);
			j = (tileSet[i]->getType()) - 2;
		}
		if (inmaze)
		{
			i = getTileIndexMaze(mBox.x, mBox.y);
			j = (tileSet[i]->getType()) - 2;
		}
		if (inrest)
		{
			i = getTileIndexRest(mBox.x, mBox.y);
			j = (tileSet[i]->getType()) - 7;
		}
		if (inhost)
		{
			i = getTileIndexHost(mBox.x, mBox.y);
			j = (tileSet[i]->getType()) - 5;
		}

		if (isSleeping)
		{
			isSleeping = false;
			sleeptime = SDL_GetTicks() - startSleep;
			fatigue -= 2 * sleeptime / 3000;
		}
		// Adjust the velocity
		if (isYuluOn == false)
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_UP:
				mVelY -= DOT_VEL;
				break;
			case SDLK_DOWN:
				mVelY += DOT_VEL;
				break;
			case SDLK_LEFT:
				mVelX -= DOT_VEL;
				break;
			case SDLK_RIGHT:
				mVelX += DOT_VEL;
				break;
			case SDLK_s:
				if ((inmap) && (j == 2))
				{
					if (money >= 0)
					{
						isYuluOn = true;
						money -= 5;
						startYuluTime = SDL_GetTicks();
					}
				}
				else if ((inhost) && (j == 2))
				{
					isSleeping = true;
					startSleep = SDL_GetTicks();
					cout << startSleep << "\n";
				}
				break;
			case SDLK_e:
				if (inrest)
				{
					inrest = false;
					inmap = true;
					mBox.x = locx;
					mBox.y = locy;
					LEVEL_HEIGHT = LEVEL_HEIGHT_MAIN;
					LEVEL_WIDTH = LEVEL_WIDTH_MAIN;
					TOTAL_TILES = TOTAL_TILES_MAIN;
					tileSet = tileSetMain;
					loadMedia(tileSet);
				}
				else if (inhost && !isSleeping)
				{
					inhost = false;
					inmap = true;
					mBox.x = locx;
					mBox.y = locy;
					LEVEL_HEIGHT = LEVEL_HEIGHT_MAIN;
					LEVEL_WIDTH = LEVEL_WIDTH_MAIN;
					TOTAL_TILES = TOTAL_TILES_MAIN;
					tileSet = tileSetMain;
					loadMedia(tileSet);
				}
				break;
			case SDLK_1:
				if (inrest)
				{
					if (j == 0)
					{
						if (money > 0)
						{
							money -= 50;
							hunger -= 50;
							feedtime = 30000;
							startFeed = SDL_GetTicks();
						}
					}
				}
				break;
			case SDLK_2:
				if (inrest)
				{
					if (j == 0)
					{
						if (money > 0)
						{
							money -= 30;
							hunger -= 40;
							feedtime = 30000;
							startFeed = SDL_GetTicks();
						}
					}
				}
				break;
			case SDLK_RETURN:
				if ((tilelocx == 40) && (tilelocy == 40) && inmaze)
				{
					inmaze = false;
					inmap = true;
					mBox.x = locx;
					mBox.y = locy;
					LEVEL_HEIGHT = LEVEL_HEIGHT_MAIN;
					LEVEL_WIDTH = LEVEL_WIDTH_MAIN;
					TOTAL_TILES = TOTAL_TILES_MAIN;
					tileSet = tileSetMain;
					loadMedia(tileSet);
				}
				else if (j == 0)
				{
					if (inmap)
					{
						locx = mBox.x;
						locy = mBox.y;
						mBox.x = 40;
						mBox.y = 40;
						int k = getbuild(mBox.x, mBox.y);
						gen(60, (buildings[k][1] == "1"));
						rendermaze();
					}
					else if (inmaze)
					{
						tp += 5;
						tileSet[i] = new Tile(tilelocx, tilelocy, 0);
						int k = getbuild(mBox.x, mBox.y);
						buildings[k][1] = "0";
					}
				}
				else if (j == 7)
				{
					if (inmap)
					{
						locx = mBox.x;
						locy = mBox.y;
						mBox.x = 1100;
						mBox.y = 1200;
						renderrest();
					}
				}
				else if (j == 10)
				{
					if (inmap)
					{
						locx = mBox.x;
						locy = mBox.y;
						mBox.x = 40;
						mBox.y = 40;
						renderhost();
					}
				}
				break;
			}
		}
		else
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_UP:
				mVelY -= 2 * DOT_VEL;
				break;
			case SDLK_DOWN:
				mVelY += 2 * DOT_VEL;
				break;
			case SDLK_LEFT:
				mVelX -= 2 * DOT_VEL;
				break;
			case SDLK_RIGHT:
				mVelX += 2 * DOT_VEL;
				break;
			case SDLK_e:
				if (j == 2)
				{
					changeMoney();
					isYuluOn = false;
				}
				break;
			}
		}
	}
	// If a key was released
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{
		// Adjust the velocity
		if (isYuluOn == false)
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_UP:
				mVelY += DOT_VEL;
				break;
			case SDLK_DOWN:
				mVelY -= DOT_VEL;
				break;
			case SDLK_LEFT:
				mVelX += DOT_VEL;
				break;
			case SDLK_RIGHT:
				mVelX -= DOT_VEL;
				break;
			}
		}
		else
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_UP:
				mVelY += 2 * DOT_VEL;
				break;
			case SDLK_DOWN:
				mVelY -= 2 * DOT_VEL;
				break;
			case SDLK_LEFT:
				mVelX += 2 * DOT_VEL;
				break;
			case SDLK_RIGHT:
				mVelX -= 2 * DOT_VEL;
				break;
			}
		}
	}
}

string Dot::information()
{
	if (inmap)
	{
		int i = getTileIndex(mBox.x, mBox.y);
		if (i == 960 || i == 841 || i == 1079)
		{
			return "Jwalamukhi Hostel!";
		}
		else if (i == 959)
		{
			return "Jwalamukhi Hostel! Press ENTER Key To Enter";
		}
		else if (i == 1793 || i == 1912 || i == 2031)
		{
			return "Aravali Hostel!";
		}
		else if (i == 1911)
		{
			return "Aravali Hostel! Press ENTER Key To Enter";
		}
		else if (i == 2745 || i == 2864 || i == 2983)
		{
			return "Karakoram Hostel!";
		}

		else if (i == 2863)
		{
			return "Karakoram Hostel! Press ENTER Key To Enter";
		}
		else if (i == 3697 || i == 3816 || i == 3935)
		{
			return "Nilgiri Hostel!";
		}
		else if (i == 3815)
		{
			return "Nilgiri Hostel! Press ENTER Key To Enter";
		}
		else if (i == 248 || i == 367 || i == 486)
		{
			return "Kumaon Hostel!";
		}
		else if (i == 368)
		{
			return "Kumaon Hostel! Press ENTER Key To Enter";
		}
		else if (i == 1676 || i == 1795 || i == 1914 || i == 2033)
		{
			return "Volleball Ground!";
		}
		else if (i == 2747 || i == 2866 || i == 2985)
		{
			return "Nalanda Ground!";
		}
		else if (i == 2159 || i == 2160 || i == 2161 || i == 2162 || i == 2163 || i == 2278 || i == 2282 || i == 2397 || i == 2401 || i == 2516 || i == 2517 || i == 2518 || i == 2519 || i == 2520)
		{
			return "SAC Circle!";
		}
		else if (i == 3352 || i == 3471 || i == 3590)
		{
			return "Student Activity Centre!";
		}
		else if (i == 3472)
		{
			return "Student Activity Centre! Press ENTER Key To Enter";
		}
		else if (i == 2045 || i == 2164)
		{
			return "Shivalik Hostel Park!";
		}
		else if (i == 1102 || i == 1221 || i == 1340)
		{
			return "Shivalik Hostel!";
		}
		else if (i == 1220)
		{
			return "Shivalik Hostel! Press ENTER Key To Enter";
		}
		else if (i == 746 || i == 865 || i == 984)
		{
			return "Satpura Hostel!";
		}
		else if (i == 866)
		{
			return "Satpura Hostel! Press ENTER Key To Enter";
		}
		else if (i == 989 || i == 990)
		{
			return "Udaigiri Hostel!";
		}
		else if (i == 871)
		{
			return "Udaigiri Hostel! Press ENTER Key To Enter";
		}
		else if (i == 2416 || i == 2417 || i == 2418)
		{
			return "IIT Hospital!";
		}
		else if (i == 2298)
		{
			return "IIT Hospital! Press ENTER Key To Enter";
		}
		else if (i == 2654 || i == 2655 || i == 2656)
		{
			return "Cricket Ground!";
		}
		else if (i == 2666 || i == 2667 || i == 2668)
		{
			return "Athletics Field!";
		}
		else if (i == 1816 || i == 1935 || i == 2054)
		{
			return "Food Outlet (Rajdhani)!";
		}
		else if (i == 1934)
		{
			return "Food Outlet (Rajdhani)! Press ENTER Key To Enter";
		}
		else if (i == 1038 || i == 1039 || i == 1040)
		{
			return "Kailash Hostel!";
		}
		else if (i == 920)
		{
			return "Kailash Hostel! Press ENTER Key To Enter";
		}
		else if (i == 2324 || i == 2325 || i == 2206)
		{
			return "Central Library!";
		}
		else if (i == 2205)
		{
			return "Central Library! Press ENTER Key To Enter";
		}
		else if (i == 2317 || i == 2318)
		{
			return "Food Outlet (Amul)!";
		}
		else if (i == 2199)
		{
			return "Food Outlet (Amul)! Press ENTER Key To Enter";
		}
		else if (i == 1015 || i == 896 || i == 897)
		{
			return "Food Outlet (Nescafe)!";
		}
		else if (i == 1016)
		{
			return "Food Outlet (Nescafe)! Press ENTER Key To Enter";
		}
		else if (i == 1249 || i == 1250 || i == 1131)
		{
			return "Academic Area!";
		}
		else if (i == 1130)
		{
			return "Academic Area! Press ENTER Key To Enter";
		}
		else if (i == 1249 || i == 1250 || i == 1131 || i == 1122 || i == 1123 || i == 1004)
		{
			return "Academic Area!";
		}
		else if (i == 1130 || i == 1003)
		{
			return "Academic Area! Press ENTER Key To Enter";
		}
		else if (i == 1854 || i == 1855 || i == 1856 || i == 1737)
		{
			return "IIT Main Building!";
		}
		else if (i == 1736)
		{
			return "IIT Main Building! Press ENTER Key To Enter";
		}
		else if (i == 2452 || i == 2333 || i == 2334)
		{
			return "Lecture Hall Complex!";
		}
		else if (i == 2453)
		{
			return "Lecture Hall Complex! Press ENTER Key To Enter";
		}
		else if (i == 2922 || i == 2923)
		{
			return "Block 99B!";
		}
		else if (i == 3041)
		{
			return "Block 99B! Press ENTER Key To Enter";
		}
		else if (i == 1623 || i == 1724)
		{
			return "Block 99C!";
		}
		else if (i == 1624)
		{
			return "Block 99C! Press ENTER Key To Enter";
		}
		else if (i == 8 || i == 9 || i == 10)
		{
			return "Gate No. 6!";
		}
		else if (i == 4410)
		{
			return "Residential Building! Press ENTER to Enter";
		}
		else if (i == 4411 || i == 4412)
		{
			return "Resedential buildings!";
		}
		else if (i == 5605 || i == 5606 || i == 5607 || i == 5608)
		{
			return "Gate No. 4!";
		}
		else if (i == 4661 || i == 4662 || i == 4663 || i == 4780 || i == 4781 || i == 4782 || i == 4899 || i == 4890 || i == 4891)
		{
			return "West Campus";
		}
		else if (i == 3980 || i == 3981 || i == 3982 || i == 3983 || i == 3984 || i == 3985)
		{
			return "Block 102!";
		}
		else if (i == 3926 || i == 4045)
		{
			return "Gate No. 2!";
		}
		else if (i == 1189 || i == 1308)
		{
			return "Gate No. 1!";
		}
		else if (i == 1176 || i == 1177)
		{
			return "Amaltas";
		}
		else if (i == 3074 || i == 3075 || i == 3076 || i == 3077 || i == 3193 || i == 3194 || i == 3195 || i == 3196)
		{
			return "East Campus";
		}
		else if (i == 484 || i == 2388 || i == 1228 || i == 2640 || i == 2671 || i == 2570 || i == 1063)
		{
			if (isYuluOn)
			{
				return "Yulu Stand! Press E to end yulu ride";
			}
			else
			{
				return "Yulu Stand! Press S to start yulu ride";
			}
		}
		else
		{
			return "";
		}
	}
	else if (inmaze)
	{
		int i = getbuild(locx, locy);
		stringstream temp;
		temp << "Welcome to " << buildings[i][0] << "\n";
		return temp.str().c_str();
	}
	else if (inhost)
	{
		int i = getTileIndexHost(mBox.x, mBox.y);
		if (i == 558 || i == 619 || i == 680)
		{
			return "Room Number 1!\n";
		}
		else if (i == 618)
		{
			if (!isSleeping)
			{
				return "Room Number 1! Press S key to Sleep\n";
			}
			else
			{
				return "Room Number 1! Press any key to WakeUp\n";
			}
		}
		else if (i == 1168 || i == 1229 || i == 1290)
		{
			return "Room Number 2!\n";
		}
		else if (i == 1228)
		{
			if (!isSleeping)
			{
				return "Room Number 2! Press S key to Sleep\n";
			}
			else
			{
				return "Room Number 2! Press any key to WakeUp\n";
			}
		}
		else if (i == 1778 || i == 1839 || i == 1900)
		{
			return "Room Number 3!\n";
		}
		else if (i == 1838)
		{
			if (!isSleeping)
			{
				return "Room Number 3! Press S key to Sleep\n";
			}
			else
			{
				return "Room Number 3! Press any key to WakeUp\n";
			}
		}
		else if (i == 2388 || i == 2449 || i == 2510)
		{
			return "Room Number 4!\n";
		}
		else if (i == 2448)
		{
			if (!isSleeping)
			{
				return "Room Number 4! Press S key to Sleep\n";
			}
			else
			{
				return "Room Number 4! Press any key to WakeUp\n";
			}
		}
		else if (i == 560 || i == 621 || i == 682)
		{
			return "Room Number 5!\n";
		}
		else if (i == 622)
		{
			if (!isSleeping)
			{
				return "Room Number 5! Press S key to Sleep\n";
			}
			else
			{
				return "Room Number 5! Press any key to WakeUp\n";
			}
		}
		else if (i == 1170 || i == 1231 || i == 1292)
		{
			return "Room Number 6!\n";
		}
		else if (i == 1232)
		{
			if (!isSleeping)
			{
				return "Room Number 6! Press S key to Sleep\n";
			}
			else
			{
				return "Room Number 6! Press any key to WakeUp\n";
			}
		}
		else if (i == 1780 || i == 1841 || i == 1902)
		{
			return "Room Number 7!\n";
		}
		else if (i == 1842)
		{
			if (!isSleeping)
			{
				return "Room Number 7! Press S key to Sleep\n";
			}
			else
			{
				return "Room Number 7! Press any key to WakeUp\n";
			}
		}
		else if (i == 2390 || i == 2451 || i == 2512)
		{
			return "Room Number 8!\n";
		}
		else if (i == 2452)
		{
			if (!isSleeping)
			{
				return "Room Number 8! Press S key to Sleep\n";
			}
			else
			{
				return "Room Number 8! Press any key to WakeUp\n";
			};
		}
		else if (i == 578 || i == 639 || i == 700)
		{
			return "Room Number 9!\n";
		}
		else if (i == 638)
		{
			if (!isSleeping)
			{
				return "Room Number 9! Press S key to Sleep\n";
			}
			else
			{
				return "Room Number 9! Press any key to WakeUp\n";
			}
		}
		else if (i == 1188 || i == 1249 || i == 1310)
		{
			return "Room Number 10!\n";
		}
		else if (i == 1248)
		{
			if (!isSleeping)
			{
				return "Room Number 10! Press S key to Sleep\n";
			}
			else
			{
				return "Room Number 10! Press any key to WakeUp\n";
			}
		}
		else if (i == 1798 || i == 1859 || i == 1920)
		{
			return "Room Number 11!\n";
		}
		else if (i == 1858)
		{
			if (!isSleeping)
			{
				return "Room Number 11! Press S key to Sleep\n";
			}
			else
			{
				return "Room Number 11! Press any key to WakeUp\n";
			}
		}
		else if (i == 2408 || i == 2469 || i == 2530)
		{
			return "Room Number 12!\n";
		}
		else if (i == 2468)
		{
			if (!isSleeping)
			{
				return "Room Number 12! Press S key to Sleep\n";
			}
			else
			{
				return "Room Number 12! Press any key to WakeUp\n";
			}
		}
		else if (i == 580 || i == 641 || i == 702)
		{
			return "Room Number 13!\n";
		}
		else if (i == 642)
		{
			if (!isSleeping)
			{
				return "Room Number 13! Press S key to Sleep\n";
			}
			else
			{
				return "Room Number 13! Press any key to WakeUp\n";
			}
		}
		else if (i == 1190 || i == 1251 || i == 1312)
		{
			return "Room Number 14!\n";
		}
		else if (i == 1252)
		{
			if (!isSleeping)
			{
				return "Room Number 14! Press S key to Sleep\n";
			}
			else
			{
				return "Room Number 14! Press any key to WakeUp\n";
			}
		}
		else if (i == 1800 || i == 1861 || i == 1922)
		{
			return "Room Number 15!\n";
		}
		else if (i == 1862)
		{
			if (!isSleeping)
			{
				return "Room Number 15! Press S key to Sleep\n";
			}
			else
			{
				return "Room Number 15! Press any key to WakeUp\n";
			}
		}
		else if (i == 2410 || i == 2471 || i == 2532)
		{
			return "Room Number 16!\n";
		}
		else if (i == 2472)
		{
			if (!isSleeping)
			{
				return "Room Number 16! Press S key to Sleep\n";
			}
			else
			{
				return "Room Number 16! Press any key to WakeUp\n";
			}
		}
		else if (i == 598 || i == 659 || i == 720)
		{
			return "Room Number 17!\n";
		}
		else if (i == 658)
		{
			if (!isSleeping)
			{
				return "Room Number 17! Press S key to Sleep\n";
			}
			else
			{
				return "Room Number 17! Press any key to WakeUp\n";
			}
		}
		else if (i == 1208 || i == 1269 || i == 1320)
		{
			return "Room Number 18!\n";
		}
		else if (i == 1268)
		{
			if (!isSleeping)
			{
				return "Room Number 18! Press S key to Sleep\n";
			}
			else
			{
				return "Room Number 18! Press any key to WakeUp\n";
			}
		}
		else if (i == 1818 || i == 1879 || i == 1940)
		{
			return "Room Number 19!\n";
		}
		else if (i == 1878)
		{
			if (!isSleeping)
			{
				return "Room Number 19! Press S key to Sleep\n";
			}
			else
			{
				return "Room Number 19! Press any key to WakeUp\n";
			}
		}
		else if (i == 2428 || i == 2489 || i == 2550)
		{
			return "Room Number 20!\n";
		}
		else if (i == 2488)
		{
			if (!isSleeping)
			{
				return "Room Number 20! Press S key to Sleep\n";
			}
			else
			{
				return "Room Number 20! Press any key to WakeUp\n";
			}
		}
		else if (i == 600 || i == 661 || i == 722)
		{
			return "Room Number 21!\n";
		}
		else if (i == 662)
		{
			if (!isSleeping)
			{
				return "Room Number 21! Press S key to Sleep\n";
			}
			else
			{
				return "Room Number 21! Press any key to WakeUp\n";
			}
		}
		else if (i == 1210 || i == 1271 || i == 1322)
		{
			return "Room Number 22!\n";
		}
		else if (i == 1272)
		{
			if (!isSleeping)
			{
				return "Room Number 22! Press S key to Sleep\n";
			}
			else
			{
				return "Room Number 22! Press any key to WakeUp\n";
			}
		}
		else if (i == 18120 || i == 1881 || i == 1942)
		{
			return "Room Number 23!\n";
		}
		else if (i == 1882)
		{
			if (!isSleeping)
			{
				return "Room Number 23! Press S key to Sleep\n";
			}
			else
			{
				return "Room Number 23! Press any key to WakeUp\n";
			}
		}
		else if (i == 2430 || i == 2491 || i == 2552)
		{
			return "Room Number 24!\n";
		}
		else if (i == 2492)
		{
			if (!isSleeping)
			{
				return "Room Number 24! Press S key to Sleep\n";
			}
			else
			{
				return "Room Number 24! Press any key to WakeUp\n";
			}
		}
		else
		{
			return "";
		}
	}
	else if (inrest)
	{
		int i = getTileIndexRest(mBox.x, mBox.y);
		if (i == 407 || i == 433)
		{
			return "Press 1 to eat noodles , press 2 to eat burger!\n";
		}
		else
		{
			return "";
		}
	}
}

void Dot::move(Tile *tiles[])
{
	// Move the dot left or right
	mBox.x += mVelX;

	// If the dot went too far to the left or right or touched a wall
	if ((mBox.x < 0) || (mBox.x + DOT_WIDTH > LEVEL_WIDTH) || touchesWall(mBox, tiles))
	{
		// move back
		mBox.x -= mVelX;
	}

	// Move the dot up or down
	mBox.y += mVelY;

	// If the dot went too far up or down or touched a wall
	if ((mBox.y < 0) || (mBox.y + DOT_HEIGHT > LEVEL_HEIGHT) || touchesWall(mBox, tiles))
	{
		// move back
		mBox.y -= mVelY;
	}
}

void Dot::setCamera(SDL_Rect &camera)
{
	// Center the camera over the dot
	camera.x = (mBox.x + DOT_WIDTH / 2) - SCREEN_WIDTH / 2;
	camera.y = (mBox.y + DOT_HEIGHT / 2) - SCREEN_HEIGHT / 2;

	// Keep the camera in bounds
	if (camera.x < 0)
	{
		camera.x = 0;
	}
	if (camera.y < 0)
	{
		camera.y = 0;
	}
	if (camera.x > LEVEL_WIDTH - camera.w)
	{
		camera.x = LEVEL_WIDTH - camera.w;
	}
	if (camera.y > LEVEL_HEIGHT - camera.h)
	{
		camera.y = LEVEL_HEIGHT - camera.h;
	}
}

void Dot::render(SDL_Rect &camera)
{
	// Show the dot
	gDot1Texture.render(mBox.x - camera.x, mBox.y - camera.y);
}

bool init()
{
	// Initialization flag
	bool success = true;

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		// Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		// Create window
		gWindow = SDL_CreateWindow("Treasure Hunt (Player 1)", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH + 280, SCREEN_HEIGHT + 120, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			// Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				// Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				// Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}

				// Initialize SDL_ttf
				if (TTF_Init() == -1)
				{
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadTextMedia(string s)
{
	// Loading success flag
	bool success = true;

	// Load Text Texture
	gFont = TTF_OpenFont("./lazy.ttf", 28);
	if (gFont == NULL)
	{
		printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}
	else
	{
		// Render text
		SDL_Color textColor = {0, 0, 0};
		if (!gTextTexture.loadFromRenderedText(s, textColor))
		{
			printf("Failed to render text texture!\n");
			success = false;
		}
	}
	TTF_CloseFont(gFont);

	return success;
}

bool loadMedia(Tile *tiles[])
{
	// Loading success flag
	bool success = true;

	// Load dot texture
	if (!gDot1Texture.loadFromFile("dot.bmp"))
	{
		printf("Failed to load dot texture!\n");
		success = false;
	}

	// Load dot texture
	if (!gDot2Texture.loadFromFile("dot.bmp"))
	{
		printf("Failed to load dot texture!\n");
		success = false;
	}

	// Load tile texture
	if (!gTileTexture.loadFromFile("tiles.jpeg"))
	{
		printf("Failed to load tile set texture!\n");
		success = false;
	}

	// Load tile map
	if (inmap)
	{
		if (!setTiles(tiles, "lazy.map"))
		{
			printf("Failed to load tile set!\n");
			success = false;
		}
	}
	else if (inmaze)
	{
		if (!setTiles(tiles, "maze.map"))
		{
			printf("Failed to load tile set!\n");
			success = false;
		}
	}
	else if (inrest)
	{
		if (!setTiles(tiles, "lazy_rest.map"))
		{
			printf("Failed to load tile set!\n");
			success = false;
		}
	}
	else if (inhost)
	{
		if (!setTiles(tiles, "lazy_resident.map"))
		{
			printf("Failed to load tile set!\n");
			success = false;
		}
	}

	return success;
}

void close(Tile *tiles[])
{
	// Deallocate tiles
	for (int i = 0; i < TOTAL_TILES; ++i)
	{
		if (tiles[i] != NULL)
		{
			delete tiles[i];
			tiles[i] = NULL;
		}
	}

	// Free loaded images
	gDot1Texture.free();
	gDot2Texture.free();
	gTileTexture.free();

	// Destroy window
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	// Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

bool checkCollision(SDL_Rect a, SDL_Rect b)
{
	// The sides of the rectangles
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;

	// Calculate the sides of rect A
	leftA = a.x;
	rightA = a.x + a.w;
	topA = a.y;
	bottomA = a.y + a.h;

	// Calculate the sides of rect B
	leftB = b.x;
	rightB = b.x + b.w;
	topB = b.y;
	bottomB = b.y + b.h;

	// If any of the sides from A are outside of B
	if (bottomA <= topB)
	{
		return false;
	}

	if (topA >= bottomB)
	{
		return false;
	}

	if (rightA <= leftB)
	{
		return false;
	}

	if (leftA >= rightB)
	{
		return false;
	}

	// If none of the sides from A are outside B
	return true;
}

void CheckHunger()
{
	if (hungerf < 0)
	{
		hungerf = 0;
	}
	else if (hungerf > 100)
	{
		hungerf = 100;
	}
}

void CheckFatigue()
{
	if (fatiguef < 0)
	{
		fatiguef = 0;
	}
	else if (fatiguef > 100)
	{
		fatiguef = 100;
	}
}

bool setTiles(Tile *tiles[], string s)
{
	// Success flag
	bool tilesLoaded = true;

	// The tile offsets
	int x = 0, y = 0;

	// Open the map
	ifstream map(s);

	// If the map couldn't be loaded
	if (map.fail())
	{
		printf("Unable to load map file!\n");
		tilesLoaded = false;
	}
	else
	{
		// Initialize the tiles
		for (int i = 0; i < TOTAL_TILES; ++i)
		{
			// Determines what kind of tile will be made
			int tileType = -1;

			// Read tile from map file
			map >> tileType;

			// If the was a problem in reading the map
			if (map.fail())
			{
				// Stop loading map
				printf("Error loading map: Unexpected end of file!\n");
				tilesLoaded = false;
				break;
			}

			// If the number is a valid tile number
			if ((tileType >= 0) && (tileType < TOTAL_TILE_SPRITES))
			{
				tiles[i] = new Tile(x, y, tileType);
			}
			// If we don't recognize the tile type
			else
			{
				// Stop loading map
				printf("Error loading map: Invalid tile type at %d!\n", i);
				tilesLoaded = false;
				break;
			}

			// Move to next tile spot
			x += TILE_WIDTH;

			// If we've gone too far
			if (x >= LEVEL_WIDTH)
			{
				// Move back
				x = 0;

				// Move to the next row
				y += TILE_HEIGHT;
			}
		}

		// Clip the sprite sheet
		if (tilesLoaded)
		{
			gTileClips[TILE_RED].x = 5;
			gTileClips[TILE_RED].y = 5;
			gTileClips[TILE_RED].w = TILE_WIDTH;
			gTileClips[TILE_RED].h = TILE_HEIGHT;

			gTileClips[TILE_GREEN].x = 5;
			gTileClips[TILE_GREEN].y = 55;
			gTileClips[TILE_GREEN].w = TILE_WIDTH;
			gTileClips[TILE_GREEN].h = TILE_HEIGHT;

			gTileClips[TILE_BLUE].x = 5;
			gTileClips[TILE_BLUE].y = 105;
			gTileClips[TILE_BLUE].w = TILE_WIDTH;
			gTileClips[TILE_BLUE].h = TILE_HEIGHT;

			gTileClips[TILE_TOPLEFT].x = 55;
			gTileClips[TILE_TOPLEFT].y = 5;
			gTileClips[TILE_TOPLEFT].w = TILE_WIDTH;
			gTileClips[TILE_TOPLEFT].h = TILE_HEIGHT;

			gTileClips[TILE_LEFT].x = 55;
			gTileClips[TILE_LEFT].y = 55;
			gTileClips[TILE_LEFT].w = TILE_WIDTH;
			gTileClips[TILE_LEFT].h = TILE_HEIGHT;

			gTileClips[TILE_BOTTOMLEFT].x = 55;
			gTileClips[TILE_BOTTOMLEFT].y = 105;
			gTileClips[TILE_BOTTOMLEFT].w = TILE_WIDTH;
			gTileClips[TILE_BOTTOMLEFT].h = TILE_HEIGHT;

			gTileClips[TILE_TOP].x = 105;
			gTileClips[TILE_TOP].y = 5;
			gTileClips[TILE_TOP].w = TILE_WIDTH;
			gTileClips[TILE_TOP].h = TILE_HEIGHT;

			gTileClips[TILE_CENTER].x = 105;
			gTileClips[TILE_CENTER].y = 55;
			gTileClips[TILE_CENTER].w = TILE_WIDTH;
			gTileClips[TILE_CENTER].h = TILE_HEIGHT;

			gTileClips[TILE_BOTTOM].x = 105;
			gTileClips[TILE_BOTTOM].y = 105;
			gTileClips[TILE_BOTTOM].w = TILE_WIDTH;
			gTileClips[TILE_BOTTOM].h = TILE_HEIGHT;

			gTileClips[TILE_TOPRIGHT].x = 155;
			gTileClips[TILE_TOPRIGHT].y = 5;
			gTileClips[TILE_TOPRIGHT].w = TILE_WIDTH;
			gTileClips[TILE_TOPRIGHT].h = TILE_HEIGHT;

			gTileClips[TILE_RIGHT].x = 155;
			gTileClips[TILE_RIGHT].y = 55;
			gTileClips[TILE_RIGHT].w = TILE_WIDTH;
			gTileClips[TILE_RIGHT].h = TILE_HEIGHT;

			gTileClips[TILE_BOTTOMRIGHT].x = 155;
			gTileClips[TILE_BOTTOMRIGHT].y = 105;
			gTileClips[TILE_BOTTOMRIGHT].w = TILE_WIDTH;
			gTileClips[TILE_BOTTOMRIGHT].h = TILE_HEIGHT;

			gTileClips[TILE_RESGATE].x = 5;
			gTileClips[TILE_RESGATE].y = 105;
			gTileClips[TILE_RESGATE].w = TILE_WIDTH;
			gTileClips[TILE_RESGATE].h = TILE_HEIGHT;
		}
	}

	// Close the file
	map.close();

	// If the map was loaded fine
	return tilesLoaded;
}

void rendermaze()
{
	inmap = false;
	inmaze = true;
	LEVEL_HEIGHT = LEVEL_HEIGHT_MAZE;
	LEVEL_WIDTH = LEVEL_WIDTH_MAZE;
	TOTAL_TILES = TOTAL_TILES_MAZE;
	tileSet = tileSetMaze;
	loadMedia(tileSet);
}

void renderrest()
{
	inmap = false;
	inrest = true;
	LEVEL_HEIGHT = LEVEL_HEIGHT_REST;
	LEVEL_WIDTH = LEVEL_WIDTH_REST;
	TOTAL_TILES = TOTAL_TILES_REST;
	tileSet = tileSetRest;
	loadMedia(tileSetRest);
}

void renderhost()
{
	inmap = false;
	inhost = true;
	LEVEL_HEIGHT = LEVEL_HEIGHT_HOST;
	LEVEL_WIDTH = LEVEL_WIDTH_HOST;
	TOTAL_TILES = TOTAL_TILES_HOST;
	tileSet = tileSetHost;
	loadMedia(tileSet);
}

bool touchesWall(SDL_Rect box, Tile *tiles[])
{
	// Go through the tiles
	for (int i = 0; i < TOTAL_TILES; ++i)
	{
		bool collide = false;
		// If the tile is a wall type tile
		if (inmap)
		{
			collide = (tiles[i]->getType() == TILE_BLUE) || (tiles[i]->getType() == TILE_RIGHT) || (tiles[i]->getType() == TILE_TOPRIGHT) || (tiles[i]->getType() == TILE_BOTTOMLEFT || (tiles[i]->getType() == TILE_TOP) || (tiles[i]->getType() == TILE_RESGATE));
			collide = !collide;
			collide = collide || (isYuluOn && (tiles[i]->getType() == TILE_TOPRIGHT));
		}
		else if (inmaze)
		{
			collide = (tiles[i]->getType() == TILE_GREEN);
		}
		else if (inrest)
		{
			collide = (tiles[i]->getType() == TILE_BOTTOM) || (tiles[i]->getType() == TILE_BOTTOMRIGHT);
			collide = !collide;
		}
		else if (inhost)
		{
			collide = (tiles[i]->getType() == TILE_BLUE) || (tiles[i]->getType() == TILE_BOTTOMRIGHT);
			collide = !collide;
		}

		if (collide)
		{
			// If the collision box touches the wall tile
			if (checkCollision(box, tiles[i]->getBox()))
			{
				return true;
			}
		}
	}

	// If no wall tiles were touched
	return false;
}

int getTileIndex(int x, int y)
{
	int j = (int)(x / 40) + (int)(y / 40) * 119;
	return j;
}

int getTileIndexMaze(int x, int y)
{
	int j = (int)(x / 40) + (int)(y / 40) * 62;
	return j;
}

int getTileIndexRest(int x, int y)
{
	int j = (int)(x / 40) + (int)(y / 40) * 56;
	return j;
}

int getTileIndexHost(int x, int y)
{
	int j = (int)(x / 40) + (int)(y / 40) * 61;
	return j;
}

void renderHungerBar(float f)
{
	SDL_Rect fillRectHBarB = {SCREEN_WIDTH + 80, 40, 20, 200};
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
	SDL_RenderFillRect(gRenderer, &fillRectHBarB);

	int height = (int)(200.0 * (1.0 - f));
	SDL_Rect fillRectHBar = {SCREEN_WIDTH + 80, 40, 20, height};
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x80, 0x80, 0x69);
	SDL_RenderFillRect(gRenderer, &fillRectHBar);
}

void renderFatigueBar(float f)
{
	SDL_Rect fillRectFBarB = {SCREEN_WIDTH + 180, 40, 20, 200};
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0x00, 0xFF);
	SDL_RenderFillRect(gRenderer, &fillRectFBarB);

	int height = (int)(200.0 * (1.0 - f));
	SDL_Rect fillRectFBar = {SCREEN_WIDTH + 180, 40, 20, height};
	SDL_SetRenderDrawColor(gRenderer, 0x80, 0xff, 0x80, 0x69);
	SDL_RenderFillRect(gRenderer, &fillRectFBar);
}

string con2dig(int n)
{
	stringstream t;
	if (n < 0)
	{
		t << "00";
	}
	else if (n < 10)
	{
		t << 0 << n;
	}
	else
	{
		t << n;
	}
	return t.str().c_str();
}

void changeMoney()
{
	if (isYuluOn)
	{
		Uint32 yulutime = SDL_GetTicks() - startYuluTime;
		int s = yulutime / 1000;
		int moneychange = (int)(s * 1.5);
		money -= moneychange;
	}
}

int getbuild(int x, int y)
{
	int i = getTileIndex(x, y);
	if (i == 959)
	{
		return 0;
	}
	else if (i == 1911)
	{
		return 1;
	}
	else if (i == 2863)
	{
		return 2;
	}
	else if (i == 3815)
	{
		return 3;
	}
	else if (i == 368)
	{
		return 4;
	}
	else if (i == 3472)
	{
		return 5;
	}
	else if (i == 1220)
	{
		return 6;
	}
	else if (i == 866)
	{
		return 7;
	}
	else if (i == 871)
	{
		return 8;
	}
	else if (i == 2298)
	{
		return 9;
	}
	else if (i == 1934)
	{
		return 10;
	}
	else if (i == 920)
	{
		return 11;
	}
	else if (i == 2205)
	{
		return 12;
	}
	else if (i == 2199)
	{
		return 13;
	}
	else if (i == 1016)
	{
		return 14;
	}
	else if (i == 1130)
	{
		return 15;
	}
	else if (i == 1003)
	{
		return 16;
	}
	else if (i == 1736)
	{
		return 17;
	}
	else if (i == 2453)
	{
		return 18;
	}
	else if (i == 3041)
	{
		return 19;
	}
	else
	{
		return 20;
	}
}

char* func(int sockfd, char s[])
{
	char buff[80];
	int n;
	for (;;) {
		bzero(s, sizeof(s));

		n = 0;

		write(sockfd, s, sizeof(s));
		bzero(s, sizeof(s));
		char* alpha;
		read(sockfd, alpha, sizeof(alpha));
		return alpha;

	}
}

int main(int argc, char *args[])
{
int sockfd, connfd;
		struct sockaddr_in servaddr, cli;

        // socket create and verification
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
            printf("socket creation failed...\n");
            exit(0);
        }
        else
        {
            printf("Socket successfully created..\n");
        }
         printf("2");
        bzero(&servaddr, sizeof(servaddr));

        // assign IP, PORT
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        servaddr.sin_port = htons(PORT);
       
        if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
            printf("connection with the server failed...\n");
            exit(0);
        }
        else
        {
            printf("connected to the server..\n");
         }
	// Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{	
			printf("connected to the server..\n");
			tileSet = tileSetMain;

			// Load media
			if (!loadMedia(tileSet))
			{
				printf("Failed to load media!\n");
			}
			else
			{
			
			receive1 = func(sockfd, message);
				// Main loop flag
				bool quit = false;

				// Event handler
				SDL_Event e;

				// The dot that will be moving around on the screen
				Dot dot1;
				Dot dot2;

				// Current time start time
				Uint32 startTime = 0;

				startTime = SDL_GetTicks();

				// In memory text stream
				stringstream timeText;

				SDL_Rect camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

				// While application is running
				while (!quit)
				{
					// Handle events on queue
					while (SDL_PollEvent(&e) != 0)
					{

						// User requests quit
						if (e.type == SDL_QUIT)
						{
							quit = true;
						}

						// Handle input for the dot
						if ((SDL_GetTicks() - startFeed) >= feedtime)
						{
							feedtime = 0;
							dot1.handleEvent(e);
						}

						dot1.information();
					}

					// Move the dot
					dot1.move(tileSet);
					dot1.setCamera(camera);

					// Clear screen
					SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
					SDL_RenderClear(gRenderer);

					// Render level
					for (int i = 0; i < TOTAL_TILES; ++i)
					{
						tileSet[i]->render(camera);
					}

					int ms = SDL_GetTicks() - startTime;
					int s = ms / 1000;
					hungerf = (int)(s / 9) + hunger;
					fatiguef = (int)(s / 12) + fatigue;
					int m = s / 60;
					s = s % 60;
					int h = m / 60;
					m = m % 60;

					timeText.str("");
					timeText << con2dig(h) << ":" << con2dig(m) << ":" << con2dig(s);

					// Move the dot
					dot1.move(tileSet);
					dot1.setCamera(camera);

					// Clear screen
					SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
					SDL_RenderClear(gRenderer);

					// Render level
					for (int i = 0; i < TOTAL_TILES; ++i)
					{
						tileSet[i]->render(camera);
					}

					// Render dot
					dot1.render(camera);
					if (inmap)
					{
						gDot2Texture.render(dot2.mBox.x - camera.x, dot2.mBox.y - camera.y);
					}

					SDL_Rect fillRectSBar = {SCREEN_WIDTH, 0, 280, SCREEN_HEIGHT};
					SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0x00, 0xFF);
					SDL_RenderFillRect(gRenderer, &fillRectSBar);

					SDL_Rect fillRectMBar = {0, SCREEN_HEIGHT, SCREEN_WIDTH + 280, 120};
					SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0xFF, 0xFF);
					SDL_RenderFillRect(gRenderer, &fillRectMBar);

					loadTextMedia(dot1.information());
					gTextTexture.render(140, SCREEN_HEIGHT + 60);

					loadTextMedia("Hunger");
					gTextTexture.render(SCREEN_WIDTH + 50, 240);

					loadTextMedia("Fatigue");
					gTextTexture.render(SCREEN_WIDTH + 150, 240);

					CheckHunger();
					CheckFatigue();

					renderHungerBar((float)hungerf / 100.0);
					renderFatigueBar((float)fatiguef / 100.0);

					loadTextMedia("Time:");
					gTextTexture.render(SCREEN_WIDTH + 50, 290);

					loadTextMedia(timeText.str().c_str());
					gTextTexture.render(SCREEN_WIDTH + 130, 290);

					loadTextMedia("P1 Score:");
					gTextTexture.render(SCREEN_WIDTH + 50, 340);

					loadTextMedia(con2dig(dot1.tp));
					gTextTexture.render(SCREEN_WIDTH + 200, 340);

					loadTextMedia("P2 Score:");
					gTextTexture.render(SCREEN_WIDTH + 50, 390);

					loadTextMedia(con2dig(dot2.tp));
					gTextTexture.render(SCREEN_WIDTH + 200, 390);

					loadTextMedia("Money:");
					gTextTexture.render(SCREEN_WIDTH + 90, 440);

					loadTextMedia(con2dig(money));
					gTextTexture.render(SCREEN_WIDTH + 180, 440);

					// Update screen
					SDL_RenderPresent(gRenderer);

					if(quit)
					{
						close(sockfd);
					}
				}
			}

		// Free resources and close SDL
			close(tileSet);
		}
	

	return 0;
}
