/*This source code copyrighted by Lazy Foo' Productions 2004-2024
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL.h>
#include <map>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <string>


//Screen dimension constants
int bg = 1;
const int totalBtn = 2;
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
		success = true;
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
		xA = 0;
		yA = 0;
	}
	void load(std::string path) {
		success = true;
		free();
		SDL_Texture* newTexture = NULL;
		newTexture = IMG_LoadTexture(gRenderer, path.c_str());
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
			success = false;
		}
		else
		{

			SDL_QueryTexture(newTexture, NULL, NULL, &mWidth, &mHeight);
		}
		mTexture = newTexture;
	}

	//Deallocates memory
	~gTexture() {
		//Deallocate
		free();
	}

	//Loads image at specified path
	
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
		setXA(x);
		setYA(y);
		SDL_RenderCopy(gRenderer, mTexture, NULL, &box);
	}

	//Gets image dimensions
	int getWidth() {
		return mWidth;
	}
	int getHeight() {
		return mHeight;
	}
	void setXA(int x) {
		xA = x;
	}
	
	void setYA(int y) {
		xA = y;
	}
	int getXA() {
		return xA;
	}
	int getYA() {
		return yA;
	}
	bool check() {
		return success;
	}

private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	//Image dimensions
	bool success;
	int mWidth;
	int mHeight;
	int xA;
	int yA;
};
//no loadfromfile, we construct by string 
gTexture slime;
gTexture background;
gTexture pvp;
gTexture pve;
gTexture ex;
gTexture moff;
gTexture mon;

std::map<std::string, std::pair<int, int>> coorBtn{
	{"pvp",{449,250}},
	{"pve",{449,250}}
};
std::map<std::string, gTexture> myTextures = {
	{"slime", slime },
	{"background", background },
	{"pvp", pvp },
	{"pve", pve },
	{"mon", mon },
	{"moff", moff },
	{"ex", ex }
};
class gBtn {
public:
	gBtn() {
		mPos.x = 0;
		mPos.y = 0;
		mPos.w = 0;
		mPos.h = 0;
		com = "";
	}
	void getInf(std::string temp) {
		mPos.x = coorBtn[temp].first;
		mPos.y = coorBtn[temp].second;
		mPos.w = myTextures[temp].getWidth();
		mPos.h = myTextures[temp].getHeight();
		com = temp;
	}
	void handleEvent(SDL_Event* e)
	{
		if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
		{
			//Get mouse position
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
			//Mouse is outside button
			if (!inside)
			{
				//place holder
			}
			//Mouse is inside button
			else
			{
				//Set mouse over sprite
				switch (e->type)
				{
				/*case SDL_MOUSEMOTION:
					
					break;*/

				case SDL_MOUSEBUTTONDOWN:
					if (com == "pvp" && e->button.button == SDL_BUTTON_LEFT) {
						bg = 2;

					}
					else if (com == "pve" && e->button.button == SDL_BUTTON_LEFT ) {
						bg = 1;
					}
					break;

				/*case SDL_MOUSEBUTTONUP:
					
					break;*/
				}
			}
		}
	}
	
private: 
	SDL_Rect mPos;
	std::string com;

};

gBtn gBtns[totalBtn];


void background1() {
	myTextures["background"].render(0, 0);
	myTextures["pvp"].render(coorBtn["pvp"].first, coorBtn["pvp"].second);
}
void background2() {
	myTextures["background"].render(0, 0);
	myTextures["pve"].render(coorBtn["pve"].first, coorBtn["pve"].second);
}

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
	myTextures["slime"].load("src/slime.png");
	myTextures["background"].load("src/backmap1.png");
	myTextures["pvp"].load("src/pvp.png");
	myTextures["pve"].load("src/pve.png");
	myTextures["ex"].load("src/ex.png");
	myTextures["moff"].load("src/moff.png");
	myTextures["mon"].load("src/mon.png");
	//Loading success flag
	bool success = true;
	if (!playMusic("src/back.wav")) {
		printf("Failed to load music!\n");
		success = false;
	}
	if (!myTextures["slime"].check())
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	if (!myTextures["background"].check())
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	if (!myTextures["pvp"].check())
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	if (!myTextures["pve"].check())
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	if (!myTextures["ex"].check())
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	if (!myTextures["moff"].check())
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	if (!myTextures["mon"].check())
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	gBtns[0].getInf("pvp");
	gBtns[1].getInf("pve");
	return success;
}

void close()
{

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
					
					for (int i = 0; i < totalBtn; ++i)
					{
						int temp = bg;
						gBtns[i].handleEvent(&e);
						if (bg != temp) {
							break;
						}
					}

				}

				//Clear screen (background -> color in gRenderer) (giong nhu xoa background)
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);
				if (bg == 1) {
					background1();
				}
				else if (bg == 2) {
					background2();
				}
				SDL_RenderPresent(gRenderer);
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}

