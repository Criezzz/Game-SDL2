/*This source code copyrighted by Lazy Foo' Productions 2004-2024
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL.h>
#include <map>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <vector>


//Screen dimension constants
bool quit = false;
int a[20][12];
int bg = 1;
bool music = true;
std::string toggleMusic = "mon";
const int totalBtn1 = 4;
const int totalBtn2 = 2;
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
gTexture head;
gTexture background;
gTexture playground;
gTexture pve;
gTexture ex;
gTexture moff;
gTexture mon;
gTexture mainMenu;

std::map<std::string, std::pair<int, int>> coorBtn{
	{"pve",{449,250}},
	{"ex",{449,600}},
	{"mon",{850,600}},
	{"moff",{850,600}},
	{"mainMenu",{449,250}},
	
};
std::map<std::string, gTexture> myTextures = {
	{"slime", slime },
	{"head", head },
	{"background", background },
	{"mainMenu", mainMenu },
	{"pve", pve },
	{"mon", mon },
	{"moff", moff },
	{"ex", ex },
	{"playground",playground}
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
					if (com == "pve" && e->button.button == SDL_BUTTON_LEFT) {
						bg = 2;

					}
					else if (com == "mainMenu" && e->button.button == SDL_BUTTON_LEFT ) {
						bg = 1;
					}
					else if (com == "mon" && e->button.button == SDL_BUTTON_LEFT) {
						if (music == true) {
							music = false;
							Mix_Pause(-1);
							Mix_PauseMusic();
							toggleMusic = "moff";
						}
						else {
							music = true;
							Mix_Resume(-1);
							Mix_ResumeMusic();
							toggleMusic = "mon";
						}
					}
					else if (com == "ex" && e->button.button == SDL_BUTTON_LEFT) {
						quit = true;
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


gBtn gBtns1[totalBtn1];
gBtn gBtns2[totalBtn2];

void background1() {
	myTextures["background"].render(0, 0);
	myTextures["pve"].render(coorBtn["pve"].first, coorBtn["pve"].second);
	myTextures["ex"].render(coorBtn["ex"].first, coorBtn["ex"].second);
	myTextures[toggleMusic].render(coorBtn["mon"].first, coorBtn["mon"].second);

}
struct Snake_pos {
	int posX;
	int posY;
	int cur_dir;
};
struct Snake {
	Snake_pos head;
	std::vector<Snake_pos> body;
	Snake_pos tail;
	void getPos(){
		std::ifstream in;
		in.open("src/save.txt");
		for (int i = 0; i < 20; i++) {
			for (int j = 0; j < 12; j++) {
				in >> a[i][j];
			}
		}
		for (int i = 0; i < 20; i++) {
			for (int j = 0; j < 12; j++) {
				if (a[i][j] == 1) {
					head.posX = j;
					head.posY = i;
				}
			}
		}
		in.close();
	}

	void draw(){
			
			myTextures["head"].render(head.posX * 32 + 64, head.posY * 32 + 64);


	}
	void move(SDL_Event* e)
	{
		std::ofstream out;
		out.open("src/save.txt");
		switch (e->type)
		{
			case SDL_KEYDOWN:
				switch (e->key.keysym.sym) {
					case SDLK_DOWN:
						a[head.posY][head.posX] = 0;
						a[head.posY+1][head.posX] = 1;
						for (int i = 0; i < 20; i++)
							for (int j = 0; j < 12; j++) out << a[i][j] << " ";
						break;
					case SDLK_UP:
						a[head.posY][head.posX] = 0;
						a[head.posY - 1][head.posX] = 1;
						for (int i = 0; i < 20; i++)
							for (int j = 0; j < 12; j++) out << a[i][j] << " ";
						break;
					case SDLK_RIGHT:
						a[head.posY][head.posX] = 0;
						a[head.posY][head.posX+1] = 1;
						for (int i = 0; i < 20; i++)
							for (int j = 0; j < 12; j++) out << a[i][j] << " ";
						break;
					case SDLK_LEFT:
						a[head.posY][head.posX] = 0;
						a[head.posY][head.posX-1] = 1;
						for (int i = 0; i < 20; i++)
							for (int j = 0; j < 12; j++) out << a[i][j] << " ";
						break;
					default:
					break;
				}
			
		}
		out.close();
	}

};
Snake player;
void background2() {
	myTextures["background"].render(0, 0);
	myTextures["mainMenu"].render(coorBtn["mainMenu"].first, coorBtn["mainMenu"].second);
	myTextures["ex"].render(coorBtn["ex"].first, coorBtn["ex"].second);
	myTextures["playground"].render(62,62);
	myTextures["playground"].render(574, 62);
	player.getPos();
	player.draw();

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
	myTextures["head"].load("src/Snake_head.png");
	myTextures["slime"].load("src/slime.png");
	myTextures["background"].load("src/backmap1.png");
	myTextures["playground"].load("src/playground.png");
	myTextures["pve"].load("src/pve.png");
	myTextures["ex"].load("src/ex.png");
	myTextures["mainMenu"].load("src/pvp.png"); // pvp == place holder
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
	if (!myTextures["head"].check())
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	if (!myTextures["playground"].check())
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	if (!myTextures["background"].check())
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	if (!myTextures["mainMenu"].check())
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
	gBtns1[0].getInf("pve");
	gBtns1[1].getInf("ex");
	gBtns1[2].getInf("mon");
	gBtns1[3].getInf("moff");

	gBtns2[0].getInf("mainMenu");
	gBtns2[1].getInf("ex");
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
					if (bg == 1) {
						for (int i = 0; i < totalBtn1; ++i)
						{
							int temp = bg;
							gBtns1[i].handleEvent(&e);
							if (bg != temp) {
								break;
							}
						}
					}
					else if (bg == 2) {
						for (int i = 0; i < totalBtn2; ++i)
						{
							int temp = bg;
							gBtns2[i].handleEvent(&e);
							if (bg != temp) {
								break;
							}
						}
						player.move(&e);



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

