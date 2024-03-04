/*This source code copyrighted by Lazy Foo' Productions 2004-2024
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL.h>
#include <vector>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <string>
int const totalButton = 1;
const int buttonStateSize = 4; //increase this everytime you add buttonSheet
//enum must-have things in a array from gButton[TOTAL_BUTTON]
enum buttonSheet { // remember to increase buttonStateSize whenever add new status in this
	MOUSEOUT = 0,
	MOUSEIN = 1,
	MOUSEDOWN = 2,
	MOUSEUP = 3,
};
const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;
Mix_Music* gMusic = NULL;
//The window renderer
SDL_Renderer* gRenderer = NULL;


class gTexture
{
public:
	//Initializes variables
	gTexture() {
		mTexture = NULL;
		xaxis = 0;
		yaxis = 0;
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
				xaxis = 0;
				yaxis = 0;
			}
	}

	//Renders texture at given point
	void render() {
		SDL_Rect box = { xaxis, yaxis, mWidth, mHeight };
		SDL_RenderCopy(gRenderer, mTexture, NULL, &box);
	}

	//Gets image dimensions
	int getWidth() {
		return mWidth;
	}
	int getHeight() {
		return mHeight;
	}
	int getXaxis() {
		return xaxis;
	}
	int getYaxis() {
		return yaxis;
	}
	void setXaxis(int x) {
		xaxis = x;
	}
	void setYaxis(int y) {
		yaxis = y;
	}
	

private:
	//The actual hardware texture
	SDL_Texture* mTexture;
	//Image dimensions
	int xaxis;
	int yaxis;
	int mWidth;
	int mHeight;
};
class gButton
{
public:
	gTexture store[buttonStateSize];
	//Initializes internal variables
	gButton(gTexture but) 
	{
		for (int i = 0; i < buttonStateSize; i++) {
			store[i] = but;
	}
		mPos.x = but.getXaxis();
		mPos.y = but.getYaxis();
		mPos.w = but.getWidth();
		mPos.h = but.getHeight();
		current = MOUSEOUT;
	}
	void mousePress(SDL_MouseButtonEvent& b) {
		if (b.button == SDL_BUTTON_LEFT) {
			store[current].render();
		}
	}
	void insTexture(int m, gTexture& z) {
		store[m] = z;
		
	}
	//Handles mouse event
	void handleEvent(SDL_Event* e) {
		//If mouse event happened
		if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
		{
			int x, y;
			SDL_GetMouseState(&x, &y);
			//Check if mouse is in button
			bool inside = true;

			//Mouse is left of the button
			if (x < mPos.x)
			{
				inside = false;
			}
			//Mouse is right of the button
			else if (x > mPos.x + mPos.w)
			{
				inside = false;
			}
			//Mouse above the button
			else if (y < mPos.y)
			{
				inside = false;
			}
			//Mouse below the button
			else if (y > mPos.y + mPos.h)
			{
				inside = false;

			}
			if (!inside)
			{
				current = MOUSEOUT;
			}
			//Mouse is inside button
			else
			{
				switch (e->type)
				{
				case SDL_MOUSEMOTION:
					current = MOUSEIN;
					break;

				case SDL_MOUSEBUTTONDOWN:
					current = MOUSEDOWN;
					break;

				case SDL_MOUSEBUTTONUP:
					current = MOUSEUP;
					break;
				}
			}
		}
	}
private:
	SDL_Rect mPos;
	buttonSheet current;
};

std::vector<gButton> gButtons;
gTexture background;
gTexture pvp;
gTexture bPvp2;


bool playMusic(std::string path) {
	if (gMusic != NULL) {
		Mix_HaltMusic();
		Mix_FreeMusic(gMusic);
		gMusic = NULL;
	}
	gMusic = Mix_LoadMUS(path.c_str());
	if (gMusic == NULL)
	{
		printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
	}
	else
	{
		Mix_PlayMusic(gMusic, -1);
	}
	return gMusic != NULL;
}
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
		//Initialize SDL
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
		{
			printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
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
			//Initialize SDL_mixer
			if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
			{
				printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
				success = false;
			}
		}
	}

	return success;
}

bool loadMedia()
{
	background.setXaxis(0);	
	background.setYaxis(0);
	pvp.setYaxis(0);
	pvp.setXaxis(0);
	
	//Loading success flag
	bool success = true;
	if (!playMusic("src/back.wav")) {
		printf("Failed to load music!\n");
		success = false;
	}
	if (!background.loadFromFile("src/backmap1.png"))
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	if (!pvp.loadFromFile("src/pvp.png"))
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	if (!bPvp2.loadFromFile("src/pve.png"))
	{
		printf("Failed to load texture image!\n");
		success = false;
	}

	gButton bPvp(pvp);
	for (int i = 0; i < totalButton; i++) {
		gButtons.push_back(bPvp);
	}
	
	
	gButtons[0].insTexture(2, bPvp2);
	return success;
}

void close()
{
	//Free loaded texture
	background.free();
	pvp.free();

	//Destroy window    
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;
	Mix_FreeMusic(gMusic);
	gMusic = NULL;

	//Quit SDL subsystems
	Mix_Quit();
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
					for (int i = 0; i < totalButton; ++i)
					{
						gButtons[i].handleEvent(&e);
					}
				}
				//Clear screen (background -> color in gRenderer) (giong nhu xoa background)
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);

				//Render texture to screen
				
				background.render();
				pvp.render();

				//Update screen
				SDL_RenderPresent(gRenderer);
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}

