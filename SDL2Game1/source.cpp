/*This source code copyrighted by Lazy Foo' Productions 2004-2024
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <string>


//Screen dimension constants
const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

class gTexture
{
public:
	//Initializes variables
	gTexture() {
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}

	//Deallocates memory
	~gTexture() {
		//Deallocate
		free();
	}

	//Loads image at specified path
	bool loadFromFile(std::string path) {
		//Get rid of preexisting texture
		free();
		SDL_Texture* newTexture = NULL;
		newTexture = IMG_LoadTexture(gRenderer, path.c_str());
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			//Get image dimensions
			SDL_QueryTexture(newTexture, NULL, NULL, &mWidth, &mHeight);
		}
		mTexture = newTexture;
		return mTexture != NULL;

	}

	//Deallocates texture
	void free() {
		// Free texture if it exists
			if (mTexture != NULL)
			{
				SDL_DestroyTexture(mTexture);
				mTexture = NULL;
				mWidth = 0;
				mHeight = 0;
			}
	}

	//Renders texture at given point
	void render(int x, int y) {
		SDL_Rect box = { x, y, mWidth, mHeight };
		SDL_RenderCopy(gRenderer, mTexture, NULL, &box);
	}

	//Gets image dimensions
	int getWidth() {
		return mWidth;
	}
	int getHeight() {
		return mHeight;
	}

private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	//Image dimensions
	int mWidth;
	int mHeight;
};
gTexture test;
gTexture background;
bool init()
{
	//Initialization flag
	bool success = true;
	//Create window
	gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (gWindow == NULL)
	{
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Create renderer for window
		gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (gRenderer == NULL)
		{
			printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Initialize renderer color
			SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

			//Initialize PNG loading
			int imgFlags = IMG_INIT_PNG;
			if (!(IMG_Init(imgFlags) & imgFlags))
			{
				printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
				success = false;
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	if (!test.loadFromFile("src/slime.png"))
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	if (!background.loadFromFile("src/backmap1.png"))
	{
		printf("Failed to load texture image!\n");
		success = false;
	}

	return success;
}

void close()
{
	//Free loaded texture
	test.free();
	background.free();

	//Destroy window    
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}


int main(int argc, char* args[])
{

	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Load media
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//While application is running
			while (!quit)
			{
				//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
				}
				//Clear screen (background -> color in gRenderer) (giong nhu xoa background)
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);

				//Render texture to screen
				background.render(0, 0);
				test.render(240, 190);

				//Update screen
				SDL_RenderPresent(gRenderer);
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}

