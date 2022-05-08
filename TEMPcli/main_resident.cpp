/*This source code copyrighted by Lazy Foo' Productions (2004-2022)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, strings, and file streams
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>
#include <fstream>

//Screen dimension constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 640;

//The dimensions of the level
const int LEVEL_WIDTH = 2440;
const int LEVEL_HEIGHT = 2040;

//Tile constants
const int TILE_WIDTH = 40;
const int TILE_HEIGHT = 40;
const int TOTAL_TILES = 3111;
const int TOTAL_TILE_SPRITES = 12;

//The different tile sprites
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

//Texture wrapper class
class LTexture
{
	public:
		//Initializes variables
		LTexture();

		//Deallocates memory
		~LTexture();

		//Loads image at specified path
		bool loadFromFile( std::string path );
		
		#if defined(SDL_TTF_MAJOR_VERSION)
		//Creates image from font string
		bool loadFromRenderedText( std::string textureText, SDL_Color textColor );
		#endif

		//Deallocates texture
		void free();

		//Set color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );

		//Set blending
		void setBlendMode( SDL_BlendMode blending );

		//Set alpha modulation
		void setAlpha( Uint8 alpha );
		
		//Renders texture at given point
		void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

		//Gets image dimensions
		int getWidth();
		int getHeight();

	private:
		//The actual hardware texture
		SDL_Texture* mTexture;

		//Image dimensions
		int mWidth;
		int mHeight;
};

//The tile
class Tile
{
    public:
		//Initializes position and type
		Tile( int x, int y, int tileType );

		//Shows the tile
		void render( SDL_Rect& camera );

		//Get the tile type
		int getType();

		//Get the collision box
		SDL_Rect getBox();

		//The attributes of the tile
		SDL_Rect mBox;

		//The tile type
		int mType;
};

//The dot that will move around on the screen
class Dot
{
    public:
		//The dimensions of the dot
		static const int DOT_WIDTH = 20;
		static const int DOT_HEIGHT = 20;

		//Maximum axis velocity of the dot
		static const int DOT_VEL = 5;

		//Initializes the variables
		Dot();

		//Takes key presses and adjusts the dot's velocity
		void handleEvent( SDL_Event& e);

        void information();

		//Moves the dot and check collision against tiles
		void move( Tile *tiles[] );

		//Centers the camera over the dot
		void setCamera( SDL_Rect& camera );

		//Shows the dot on the screen
		void render( SDL_Rect& camera );

		//Collision box of the dot
		SDL_Rect mBox;

		//The velocity of the dot
		int mVelX, mVelY;
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia( Tile* tiles[] );

//Frees media and shuts down SDL
void close( Tile* tiles[] );

//Box collision detector
bool checkCollision( SDL_Rect a, SDL_Rect b );

//Checks collision box against set of tiles
bool touchesWall( SDL_Rect box, Tile* tiles[] );

//Sets tiles from tile map
bool setTiles( Tile *tiles[] );

int getTileIndex(int x, int y);

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Scene textures
LTexture gDotTexture;
LTexture gTileTexture;

SDL_Rect gTileClips[ TOTAL_TILE_SPRITES ];

bool isYuluOn = false;

//The level tiles
Tile* tileSet[ TOTAL_TILES ];

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile( std::string path )
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		return  "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0xFF, 0xFF, 0xFF ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			return  "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

#if defined(SDL_TTF_MAJOR_VERSION)
bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
	if( textSurface != NULL )
	{
		//Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if( mTexture == NULL )
		{
			return  "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
	else
	{
		return  "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}

	
	//Return success
	return mTexture != NULL;
}
#endif

void LTexture::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}
		
void LTexture::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

Tile::Tile( int x, int y, int tileType )
{
    //Get the offsets
    mBox.x = x;
    mBox.y = y;

    //Set the collision box
    mBox.w = TILE_WIDTH;
    mBox.h = TILE_HEIGHT;

    //Get the tile type
    mType = tileType;
}

void Tile::render( SDL_Rect& camera )
{
    //If the tile is on screen
    if( checkCollision( camera, mBox ) )
    {
        //Show the tile
        gTileTexture.render( mBox.x - camera.x, mBox.y - camera.y, &gTileClips[ mType ] );
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
    //Initialize the collision box
    mBox.x = 40;
    mBox.y = 40;
	mBox.w = DOT_WIDTH;
	mBox.h = DOT_HEIGHT;

    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;
}

void Dot::handleEvent( SDL_Event& e )
{
    //If a key was pressed
	if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
        int i = getTileIndex(mBox.x,mBox.y);
        i = (tileSet[i]->getType())-2;
        //Adjust the velocity
        if (isYuluOn == false)
        {
            switch( e.key.keysym.sym )
            {
                case SDLK_UP: mVelY -= DOT_VEL; break;
                case SDLK_DOWN: mVelY += DOT_VEL; break;
                case SDLK_LEFT: mVelX -= DOT_VEL; break;
                case SDLK_RIGHT: mVelX += DOT_VEL; break;
				case SDLK_RETURN: if (i==5) {return "Slept Successfully!\n"; }break;
            }
        }
        else
        {
            switch( e.key.keysym.sym )
            {
                case SDLK_UP: mVelY -= 2*DOT_VEL; break;
                case SDLK_DOWN: mVelY += 2*DOT_VEL; break;
                case SDLK_LEFT: mVelX -= 2*DOT_VEL; break;
                case SDLK_RIGHT: mVelX += 2*DOT_VEL; break;
                case SDLK_e: if (i==2){ isYuluOn =false; }break;
            }
        }
        
    }
    //If a key was released
    else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
    {
        //Adjust the velocity
        if (isYuluOn ==false)
        {
            switch( e.key.keysym.sym )
            {
                case SDLK_UP: mVelY += DOT_VEL; break;
                case SDLK_DOWN: mVelY -= DOT_VEL; break;
                case SDLK_LEFT: mVelX += DOT_VEL; break;
                case SDLK_RIGHT: mVelX -= DOT_VEL; break;
            }
        }
        else
        {
            switch( e.key.keysym.sym )
            {
                case SDLK_UP: mVelY += 2*DOT_VEL; break;
                case SDLK_DOWN: mVelY -= 2*DOT_VEL; break;
                case SDLK_LEFT: mVelX += 2*DOT_VEL; break;
                case SDLK_RIGHT: mVelX -= 2*DOT_VEL; break;
            }
        }
    }
}

void Dot::information()
{
        int i = getTileIndex(mBox.x , mBox.y);
        if (i==558 || i==619 || i== 680)
        {
            return "Room Number 1!\n"; 
		}
        else if (i==618)
        {
            return "Room Number 1! Press ENTER key to Sleep\n"; 
        }
        else if (i==1168 || i==1229 || i== 1290)
        {
            return "Room Number 2!\n"; 
		}
        else if (i==1228)
        {
            return "Room Number 2! Press ENTER key to Sleep\n"; 
        }
        else if (i==1778 || i==1839 || i== 1900)
        {
            return "Room Number 3!\n"; 
		}
        else if (i==1838)
        {
            return "Room Number 3! Press ENTER key to Sleep\n"; 
        }
        else if (i==2388 || i==2449 || i== 2510)
        {
            return "Room Number 4!\n"; 
		}
        else if (i==2448)
        {
            return "Room Number 4! Press ENTER key to Sleep\n"; 
        }
        else if (i==560 || i==621 || i== 682)
        {
            return "Room Number 5!\n"; 
		}
        else if (i==622)
        {
            return "Room Number 5! Press ENTER key to Sleep\n"; 
        }
        else if (i==1170 || i==1231 || i== 1292)
        {
            return "Room Number 6!\n"; 
		}
        else if (i==1232)
        {
            return "Room Number 6! Press ENTER key to Sleep\n"; 
        }
        else if (i==1780 || i==1841 || i== 1902)
        {
            return "Room Number 7!\n"; 
		}
        else if (i==1842)
        {
            return "Room Number 7! Press ENTER key to Sleep\n"; 
        }
        else if (i==2390 || i==2451 || i== 2512)
        {
            return "Room Number 8!\n"; 
		}
        else if (i==2452)
        {
            return "Room Number 8! Press ENTER key to Sleep\n"; 
        }
        else if (i==578 || i==639 || i== 700)
        {
            return "Room Number 9!\n"; 
		}
        else if (i==638)
        {
            return "Room Number 9! Press ENTER key to Sleep\n"; 
        }
        else if (i==1188 || i==1249 || i== 1310)
        {
            return "Room Number 10!\n"; 
		}
        else if (i==1248)
        {
            return "Room Number 10! Press ENTER key to Sleep\n"; 
        }
        else if (i==1798 || i==1859 || i== 1920)
        {
            return "Room Number 11!\n"; 
		}
        else if (i==1858)
        {
            return "Room Number 11! Press ENTER key to Sleep\n"; 
        }
        else if (i==2408 || i==2469 || i== 2530)
        {
            return "Room Number 12!\n"; 
		}
        else if (i==2468)
        {
            return "Room Number 12! Press ENTER key to Sleep\n"; 
        }
        else if (i==580 || i==641 || i== 702)
        {
            return "Room Number 13!\n"; 
		}
        else if (i==642)
        {
            return "Room Number 13! Press ENTER key to Sleep\n"; 
        }
        else if (i==1190 || i==1251 || i== 1312)
        {
            return "Room Number 14!\n"; 
		}
        else if (i==1252)
        {
            return "Room Number 14! Press ENTER key to Sleep\n"; 
        }
        else if (i==1800 || i==1861 || i== 1922)
        {
            return "Room Number 15!\n"; 
		}
        else if (i==1862)
        {
            return "Room Number 15! Press ENTER key to Sleep\n"; 
        }
        else if (i==2410 || i==2471 || i== 2532)
        {
            return "Room Number 16!\n"; 
		}
        else if (i==2472)
        {
            return "Room Number 16! Press ENTER key to Sleep\n"; 
        }
        else if (i==598 || i==659 || i== 720)
        {
            return "Room Number 17!\n"; 
		}
        else if (i==658)
        {
            return "Room Number 17! Press ENTER key to Sleep\n"; 
        }
        else if (i==1208 || i==1269 || i== 1320)
        {
            return "Room Number 18!\n"; 
		}
        else if (i==1268)
        {
            return "Room Number 18! Press ENTER key to Sleep\n"; 
        }
        else if (i==1818 || i==1879 || i== 1940)
        {
            return "Room Number 19!\n"; 
		}
        else if (i==1878)
        {
            return "Room Number 19! Press ENTER key to Sleep\n"; 
        }
        else if (i==2428 || i==2489 || i== 2550)
        {
            return "Room Number 20!\n"; 
		}
        else if (i==2488)
        {
            return "Room Number 20! Press ENTER key to Sleep\n"; 
        }
        else if (i==600 || i==661 || i== 722)
        {
            return "Room Number 21!\n"; 
		}
        else if (i==662)
        {
            return "Room Number 21! Press ENTER key to Sleep\n"; 
        }
        else if (i==1210 || i==1271 || i== 1322)
        {
            return "Room Number 22!\n"; 
		}
        else if (i==1272)
        {
            return "Room Number 22! Press ENTER key to Sleep\n"; 
        }
        else if (i==18120 || i==1881 || i== 1942)
        {
            return "Room Number 23!\n"; 
		}
        else if (i==1882)
        {
            return "Room Number 23! Press ENTER key to Sleep\n"; 
        }
        else if (i==2430 || i==2491 || i== 2552)
        {
            return "Room Number 24!\n"; 
		}
        else if (i==2492)
        {
            return "Room Number 24! Press ENTER key to Sleep\n"; 
        }
}

void Dot::move( Tile *tiles[] )
{
    //Move the dot left or right
    mBox.x += mVelX;

    //If the dot went too far to the left or right or touched a wall
    if( ( mBox.x < 0 ) || ( mBox.x + DOT_WIDTH > LEVEL_WIDTH ) || touchesWall( mBox, tiles ) )
    {
        //move back
        mBox.x -= mVelX;
    }

    //Move the dot up or down
    mBox.y += mVelY;

    //If the dot went too far up or down or touched a wall
    if( ( mBox.y < 0 ) || ( mBox.y + DOT_HEIGHT > LEVEL_HEIGHT ) || touchesWall( mBox, tiles ) )
    {
        //move back
        mBox.y -= mVelY;
    }

}

void Dot::setCamera( SDL_Rect& camera )
{
	//Center the camera over the dot
	camera.x = ( mBox.x + DOT_WIDTH / 2 ) - SCREEN_WIDTH / 2;
	camera.y = ( mBox.y + DOT_HEIGHT / 2 ) - SCREEN_HEIGHT / 2;

	//Keep the camera in bounds
	if( camera.x < 0 )
	{ 
		camera.x = 0;
	}
	if( camera.y < 0 )
	{
		camera.y = 0;
	}
	if( camera.x > LEVEL_WIDTH - camera.w )
	{
		camera.x = LEVEL_WIDTH - camera.w;
	}
	if( camera.y > LEVEL_HEIGHT - camera.h )
	{
		camera.y = LEVEL_HEIGHT - camera.h;
	}
}

void Dot::render( SDL_Rect& camera )
{
    //Show the dot
	gDotTexture.render( mBox.x - camera.x, mBox.y - camera.y );
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		return  "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			return  "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			return  "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL )
			{
				return  "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					return  "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia( Tile* tiles[] )
{
	//Loading success flag
	bool success = true;

	//Load dot texture
	if( !gDotTexture.loadFromFile( "dot.bmp" ) )
	{
		return  "Failed to load dot texture!\n" );
		success = false;
	}

	//Load tile texture
	if( !gTileTexture.loadFromFile( "tiles.png" ) )
	{
		return  "Failed to load tile set texture!\n" );
		success = false;
	}

	//Load tile map
	if( !setTiles( tiles ) )
	{
		return  "Failed to load tile set!\n" );
		success = false;
	}

	return success;
}

void close( Tile* tiles[] )
{
	//Deallocate tiles
	for( int i = 0; i < TOTAL_TILES; ++i )
	{
		 if( tiles[ i ] != NULL )
		 {
			delete tiles[ i ];
			tiles[ i ] = NULL;
		 }
	}

	//Free loaded images
	gDotTexture.free();
	gTileTexture.free();

	//Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

bool checkCollision( SDL_Rect a, SDL_Rect b )
{
    //The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Calculate the sides of rect A
    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    bottomA = a.y + a.h;

    //Calculate the sides of rect B
    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;

    //If any of the sides from A are outside of B
    if( bottomA <= topB )
    {
        return false;
    }

    if( topA >= bottomB )
    {
        return false;
    }

    if( rightA <= leftB )
    {
        return false;
    }

    if( leftA >= rightB )
    {
        return false;
    }

    //If none of the sides from A are outside B
    return true;
}

bool setTiles( Tile* tiles[] )
{
	//Success flag
	bool tilesLoaded = true;

    //The tile offsets
    int x = 0, y = 0;

    //Open the map
    std::ifstream map( "lazy_resident.map" );

    //If the map couldn't be loaded
    if( map.fail() )
    {
		return  "Unable to load map file!\n" );
		tilesLoaded = false;
    }
	else
	{
		//Initialize the tiles
		for( int i = 0; i < TOTAL_TILES; ++i )
		{
			//Determines what kind of tile will be made
			int tileType = -1;

			//Read tile from map file
			map >> tileType;

			//If the was a problem in reading the map
			if( map.fail() )
			{
				//Stop loading map
				return  "Error loading map: Unexpected end of file!\n" );
				tilesLoaded = false;
				break;
			}

			//If the number is a valid tile number
			if( ( tileType >= 0 ) && ( tileType < TOTAL_TILE_SPRITES ) )
			{
				tiles[ i ] = new Tile( x, y, tileType );
			}
			//If we don't recognize the tile type
			else
			{
				//Stop loading map
				return  "Error loading map: Invalid tile type at %d!\n", i );
				tilesLoaded = false;
				break;
			}

			//Move to next tile spot
			x += TILE_WIDTH;

			//If we've gone too far
			if( x >= LEVEL_WIDTH )
			{
				//Move back
				x = 0;

				//Move to the next row
				y += TILE_HEIGHT;
			}
		}
		
		//Clip the sprite sheet
		if( tilesLoaded )
		{
			gTileClips[ TILE_RED ].x = 5;
			gTileClips[ TILE_RED ].y = 5;
			gTileClips[ TILE_RED ].w = TILE_WIDTH;
			gTileClips[ TILE_RED ].h = TILE_HEIGHT;

			gTileClips[ TILE_GREEN ].x = 5;
			gTileClips[ TILE_GREEN ].y = 55;
			gTileClips[ TILE_GREEN ].w = TILE_WIDTH;
			gTileClips[ TILE_GREEN ].h = TILE_HEIGHT;

			gTileClips[ TILE_BLUE ].x = 5;
			gTileClips[ TILE_BLUE ].y = 105;
			gTileClips[ TILE_BLUE ].w = TILE_WIDTH;
			gTileClips[ TILE_BLUE ].h = TILE_HEIGHT;

			gTileClips[ TILE_TOPLEFT ].x = 55;
			gTileClips[ TILE_TOPLEFT ].y = 5;
			gTileClips[ TILE_TOPLEFT ].w = TILE_WIDTH;
			gTileClips[ TILE_TOPLEFT ].h = TILE_HEIGHT;

			gTileClips[ TILE_LEFT ].x = 55;
			gTileClips[ TILE_LEFT ].y = 55;
			gTileClips[ TILE_LEFT ].w = TILE_WIDTH;
			gTileClips[ TILE_LEFT ].h = TILE_HEIGHT;

			gTileClips[ TILE_BOTTOMLEFT ].x = 55;
			gTileClips[ TILE_BOTTOMLEFT ].y = 105;
			gTileClips[ TILE_BOTTOMLEFT ].w = TILE_WIDTH;
			gTileClips[ TILE_BOTTOMLEFT ].h = TILE_HEIGHT;

			gTileClips[ TILE_TOP ].x = 105;
			gTileClips[ TILE_TOP ].y = 5;
			gTileClips[ TILE_TOP ].w = TILE_WIDTH;
			gTileClips[ TILE_TOP ].h = TILE_HEIGHT;

			gTileClips[ TILE_CENTER ].x = 105;
			gTileClips[ TILE_CENTER ].y = 55;
			gTileClips[ TILE_CENTER ].w = TILE_WIDTH;
			gTileClips[ TILE_CENTER ].h = TILE_HEIGHT;

			gTileClips[ TILE_BOTTOM ].x = 105;
			gTileClips[ TILE_BOTTOM ].y = 105;
			gTileClips[ TILE_BOTTOM ].w = TILE_WIDTH;
			gTileClips[ TILE_BOTTOM ].h = TILE_HEIGHT;

			gTileClips[ TILE_TOPRIGHT ].x = 155;
			gTileClips[ TILE_TOPRIGHT ].y = 5;
			gTileClips[ TILE_TOPRIGHT ].w = TILE_WIDTH;
			gTileClips[ TILE_TOPRIGHT ].h = TILE_HEIGHT;

			gTileClips[ TILE_RIGHT ].x = 155;
			gTileClips[ TILE_RIGHT ].y = 55;
			gTileClips[ TILE_RIGHT ].w = TILE_WIDTH;
			gTileClips[ TILE_RIGHT ].h = TILE_HEIGHT;

			gTileClips[ TILE_BOTTOMRIGHT ].x = 155;
			gTileClips[ TILE_BOTTOMRIGHT ].y = 105;
			gTileClips[ TILE_BOTTOMRIGHT ].w = TILE_WIDTH;
			gTileClips[ TILE_BOTTOMRIGHT ].h = TILE_HEIGHT;
		}
	}

    //Close the file
    map.close();

    //If the map was loaded fine
    return tilesLoaded; 
}

bool touchesWall( SDL_Rect box, Tile* tiles[] )
{
    //Go through the tiles
    for( int i = 0; i < TOTAL_TILES; ++i )
    {
        //If the tile is a wall type tile
        bool collide = ( tiles[ i ]->getType() == TILE_BLUE ) || ( tiles[ i ]->getType() == TILE_BOTTOMRIGHT ) ;
        collide = ! collide;
        collide = collide || (isYuluOn && (tiles[i]->getType() == TILE_TOPRIGHT));
        
        if( collide)   
        {
            //If the collision box touches the wall tile
            if( checkCollision( box, tiles[ i ]->getBox() ) )
            {
                return true;
            }
        }
    }

    //If no wall tiles were touched
    return false;
}

int getTileIndex(int x, int y)
{
    int j = (int)(x/40)+(int)(y/40)*61;
    return j;
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init() )
	{
		return  "Failed to initialize!\n" );
	}
	else
	{

		//Load media
		if( !loadMedia( tileSet ) )
		{
			return  "Failed to load media!\n" );
		}
		else
		{	
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//The dot that will be moving around on the screen
			Dot dot;

			//Level camera
			SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

			//While application is running
			while( !quit )
			{
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
                    
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}

					//Handle input for the dot
					dot.handleEvent( e );
                    dot.information(  );
				}

				//Move the dot
				dot.move( tileSet );
				dot.setCamera( camera );

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );

				//Render level
				for( int i = 0; i < TOTAL_TILES; ++i )
				{
					tileSet[ i ]->render( camera );
				}

				//Render dot
				dot.render( camera );

				//Update screen
				SDL_RenderPresent( gRenderer );
			}
		}
		
		//Free resources and close SDL
		close( tileSet );
	}

	return 0;
}