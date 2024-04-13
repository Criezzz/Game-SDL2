/*This source code copyrighted by Lazy Foo' Productions 2004-2024
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <map>
#include <string>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <stack>
#include <queue>
#include<cmath>
//supporting function for scoring
int mind = -1;
int curTime = SDL_GetTicks();
class QItem {
public:
	int row;
	int col;
	int dist;
	QItem(int x, int y, int w) : row(x), col(y), dist(w){	}
};
int minDis(int grid[20][12])
{
	QItem source(0, 0, 0);

	// To keep track of visited QItems. Marking
	// blocked cells as visited.
	bool visited[20][12];
	for (int i = 0; i < 20; i++) {
		for (int j = 0; j < 12; j++)
		{
			if (grid[i][j] == 2) // 2 == body
				visited[i][j] = true;
			else
				visited[i][j] = false;

			// Finding source
			if (grid[i][j] == 1)
			{
				source.row = i;
				source.col = j;
			}
		}
	}

	// applying BFS on matrix cells starting from source
	std::queue<QItem> q;
	q.push(source);
	visited[source.row][source.col] = true;
	while (!q.empty()) {
		QItem p = q.front();
		q.pop();

		// Destination found;
		if (grid[p.row][p.col] == 10)
			return p.dist;

		// moving up
		if (p.row - 1 >= 0 &&
			visited[p.row - 1][p.col] == false) {
			q.push(QItem(p.row - 1, p.col, p.dist + 1));
			visited[p.row - 1][p.col] = true;
		}

		// moving down
		if (p.row + 1 < 20 &&
			visited[p.row + 1][p.col] == false) {
			q.push(QItem(p.row + 1, p.col, p.dist + 1));
			visited[p.row + 1][p.col] = true;
		}

		// moving left
		if (p.col - 1 >= 0 &&
			visited[p.row][p.col - 1] == false) {
			q.push(QItem(p.row, p.col - 1, p.dist + 1));
			visited[p.row][p.col - 1] = true;
		}

		// moving right
		if (p.col + 1 < 12 && visited[p.row][p.col + 1] == false) {
			q.push(QItem(p.row, p.col + 1, p.dist + 1));
			visited[p.row][p.col + 1] = true;
		}
	}
	return -1;
}

int countedFrames = 0;

//Main loop flag
bool quit = false;

//Global variables
TTF_Font* gFont = NULL;
SDL_Color textColor = { 0, 0, 0 };
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
gTexture body;
gTexture Sheads;
gTexture Sheadc;
gTexture apple;
gTexture background;
gTexture playground;
gTexture pve;
gTexture ex;
gTexture moff;
gTexture mon;
gTexture mainMenu;
gTexture p1;
gTexture p2;
gTexture draw;




std::map<std::string, std::pair<int, int>> coorBtn{
	{"pve",{449,250}},
	{"ex",{449,600}},
	{"mon",{850,600}},
	{"moff",{850,600}},
	{"mainMenu",{449,250}},
	
};
std::map<std::string, gTexture> myTextures = {
	{"body", body },
	{"headc", Sheadc },
	{"heads", Sheads },
	{"apple", apple },
	{"background", background },
	{"mainMenu", mainMenu },
	{"pve", pve },
	{"mon", mon },
	{"moff", moff },
	{"ex", ex },
	{"playground",playground}
};
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
class Text
{
public:
	// Game points
	int points = 0;
	//Initializes variables
	Text()
	{
		//Initialize
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;

	}

	//Deallocates memory
	~Text()
	{
		//Deallocate
		free();
	}

	//Creates image from font string
	bool load(std::string textureText, SDL_Color textColor)
	{
		//Get rid of preexisting texture
		free();

		//Render text surface
		SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
		if (textSurface == NULL)
		{
			printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
		}
		else
		{
			//Create texture from surface pixels
			mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
			if (mTexture == NULL)
			{
				printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
			}
			else
			{
				//Get image dimensions
				mWidth = textSurface->w;
				mHeight = textSurface->h;
			}

			//Get rid of old surface
			SDL_FreeSurface(textSurface);
		}

		//Return success
		return mTexture != NULL;
	}

	//Deallocates texture
	void free()
	{
		//Free texture if it exists
		if (mTexture != NULL)
		{
			SDL_DestroyTexture(mTexture);
			mTexture = NULL;
			mWidth = 0;
			mHeight = 0;
		}
	}

	//Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue)
	{
		//Modulate texture rgb
		SDL_SetTextureColorMod(mTexture, red, green, blue);
	}

	//Set blending
	void setBlendMode(SDL_BlendMode blending)
	{
		//Set blending function
		SDL_SetTextureBlendMode(mTexture, blending);
	}

	//Set alpha modulation
	void setAlpha(Uint8 alpha)
	{
		//Modulate texture alpha
		SDL_SetTextureAlphaMod(mTexture, alpha);
	}

	//Renders texture at given point
	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE)
	{
		//Set rendering space and render to screen
		SDL_Rect renderQuad = { x, y, mWidth, mHeight };

		//Set clip rendering dimensions
		if (clip != NULL)
		{
			renderQuad.w = clip->w;
			renderQuad.h = clip->h;
		}

		//Render to screen
		SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
	}

	//Gets image dimensions
	int getWidth()
	{
		return mWidth;
	}
	int getHeight()
	{
		return mHeight;
	}
	std::string init(int x)
	{
		std::string res="";
		for (int i = 0; i < 5; i++) {
			res += char(x % 10 + '0');
			x = x / 10;
		}
		for (int i = 0; i < 2; i++) std::swap(res[i], res[4 - i]);
		return res;
	}

private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	//Image dimensions
	int mWidth;
	int mHeight;
};
Text tim;
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
						tim.points = 60;
						curTime = SDL_GetTicks();
							if ((music) && !playMusic("src/ingame.wav")) {
								printf("Failed to load music!\n");
							}
						
					}
					else if (com == "mainMenu" && e->button.button == SDL_BUTTON_LEFT ) {
						bg = 1;
						if (music) {
							if (!playMusic("src/back.wav") && music == true) {
								printf("Failed to load music!\n");
							}

						}
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
class Snake_pos {
public:
	// Get position of snake parts
	int posX;
	int posY;
	// Get direct of snake parts
	// right = 0, left = 1, up = 2, down = 3
	int cur_dir;
	Snake_pos(int x,int y,int z){
		posX = x;
		posY = y;
		cur_dir = z;
	}
};
std::map<int, std::string> dir{
	{0,"right"},
	{1,"left"},
	{2,"up"},
	{3,"down"}
};
struct Snake {
	// Set up snake head 
	std::string particular;
	int a[20][12];
	Snake_pos head{ 0,0,3 };
	std::vector<Snake_pos> body;
	std::stack<int> PRESSKEY;
	bool Has_Eaten_Apple = 0;
	int appleX = rand() % 12;
	int appleY = rand() % 20;
	Text Score;
	int countedStep = 0;
	void getCD() {
		head.cur_dir = 3;
	}
	bool lose = 0;
	void getPos() {
		std::ifstream in;
		in.open("src/"+particular+".txt",std::ios::trunc);
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
				if (a[i][j] == 10) {
					appleX = j;
					appleY = i;
				}
			}
		}
		in.close();
	} 
	void draw(){
		if (lose) myTextures["head"+particular].load("src/Dead_head_" + dir[head.cur_dir]+".png");
		for (auto c : body) {
			myTextures["body"].render(c.posX * 32 + (particular[0] - 'a') * 32, c.posY * 32 + 64);
		}
			myTextures["head"+particular].render(head.posX * 32 + (particular[0] - 'a')*32, head.posY * 32 + 64);
			myTextures["apple"].render(appleX * 32 + (particular[0] - 'a')*32, appleY * 32 + 64);
	}
	void move()
	{
		if (mind == -1) {
			mind = minDis(a);
		}
		a[appleY][appleX] = 10;
		std::ofstream out;
		int preX = head.posX;
		int preY = head.posY;
		int predir = head.cur_dir;
			if (!PRESSKEY.empty()) head.cur_dir = PRESSKEY.top();
			switch (head.cur_dir) {
			case 0:
					++head.posX;
					myTextures["head"+particular].load("src/Snake_head_right.png");
				break;
			case 1:
					--head.posX;
					myTextures["head" + particular].load("src/Snake_head_left.png");
				break;
			case 2:
					--head.posY;
					myTextures["head" + particular].load("src/Snake_head_up.png");
				break;
			case 3:
					++head.posY;
					myTextures["head" + particular].load("src/Snake_head_down.png");
				break;
			default:
				break;
			}
			if (head.posY == 20 || head.posY == -1 || head.posX == -1 || head.posX == 12) {
				lose = true;
				head.cur_dir = predir;
				return;
			}
			switch (a[head.posY][head.posX]) {
			case 2:
				if (!(head.posY == body[0].posY && head.posX == body[0].posX)) {
					lose = true;
					head.posX = preX;
					head.posY = preY;
					head.cur_dir = predir;

				}
				break;
			case 10:
				Has_Eaten_Apple = 1;
				break;
			}
			if (Has_Eaten_Apple) {
				Score.points +=(100*mind)/countedStep + mind;
				while (a[appleY][appleX] != 0) {
					appleY = rand() % 20;
					appleX = rand() % 12;
				}
				Snake_pos* newbody = new Snake_pos{ preX , preY , predir };
				mind = -1;
				body.push_back(*(newbody));
				countedStep = 0;
			}
			if(!lose) 
			{
				if (body.size() > 1 && !Has_Eaten_Apple) {
					for (int i = 0; i < body.size() - 1; i++) {
						body[i].cur_dir = body[i + 1].cur_dir;
						body[i].posX = body[i + 1].posX;
						body[i].posY = body[i + 1].posY;
					}
				}
				if (!(body.empty())) {
					body[body.size() - 1].cur_dir = predir;
					body[body.size() - 1].posX = preX;
					body[body.size() - 1].posY = preY;
				}
			}
			for (int i = 0; i < 20; i++) {
				for (int j = 0; j < 12; j++) {
					a[i][j] = 0;
				}
			}
			for (auto x : body) {
				a[x.posY][x.posX] = 2;
			}
			Has_Eaten_Apple = 0;
			a[head.posY][head.posX] = 1;
			a[appleY][appleX] = 10;
			out.open("src/"+particular+".txt",std::ios::trunc);
			for (int i = 0; i < 20; i++) {
				for (int j = 0; j < 12; j++) out << a[i][j] << " ";
				out << '\n';
			}
			out.close();
			while (!PRESSKEY.empty()) {
				PRESSKEY.pop();
			}
			countedStep++;

		}
	void GetKey(SDL_Event* e) {
		switch (e->type)
		{
		case SDL_KEYDOWN:
		{
			if (particular == "c")
			{
				switch (e->key.keysym.sym) {
				case SDLK_s:
					if (head.cur_dir != 2)
					{
						PRESSKEY.push(3);
					}
					break;
				case SDLK_w:
					if (head.cur_dir != 3) {
						PRESSKEY.push(2);
					}
					break;
				case SDLK_d:
					if (head.cur_dir != 1) {
						PRESSKEY.push(0);
					}
					break;
				case SDLK_a:
					if (head.cur_dir != 0) {
						PRESSKEY.push(1);
					}
					break;
				}
			}
			else {
				switch (e->key.keysym.sym) {
				case SDLK_DOWN:
					if (head.cur_dir != 2)
					{
						PRESSKEY.push(3);
					}
					break;
				case SDLK_UP:
					if (head.cur_dir != 3) {
						PRESSKEY.push(2);
					}
					break;
				case SDLK_RIGHT:
					if (head.cur_dir != 1) {
						PRESSKEY.push(0);
					}
					break;
				case SDLK_LEFT:
					if (head.cur_dir != 0) {
						PRESSKEY.push(1);
					}
					break;
				}
			}
			break;
		}
		}
	}
	Snake(std::string p) : particular(p) {}
};
Snake player1("c"), player2("s");
void background2() {
	if (SDL_GetTicks() - curTime >= 1000) {
		--tim.points;
		curTime = SDL_GetTicks();
	}
	myTextures["background"].render(0, 0);
	myTextures["mainMenu"].render(coorBtn["mainMenu"].first, coorBtn["mainMenu"].second);
	myTextures["ex"].render(coorBtn["ex"].first, coorBtn["ex"].second);
	myTextures["playground"].render(62,62);
	myTextures["playground"].render(574, 62);
	tim.load(tim.init(tim.points), textColor);
	tim.render(450, 64 - player1.Score.getHeight());
	player1.Score.load("SCORE : " + player1.Score.init(player1.Score.points), textColor);
	player1.Score.render(64,64-player1.Score.getHeight());
	player1.draw();
	player1.getPos();
	player2.Score.load("SCORE : " + player1.Score.init(player2.Score.points), textColor);
	player2.Score.render(576, 64 - player2.Score.getHeight());
	player2.draw();
	player2.getPos();
	if (countedFrames > 12) {
		if (!player1.lose) {
			player1.move();
		}
		if (!player2.lose) {
			player2.move();
			player2.countedFrames = 0;
		}
	}
	else win.check = 1;
	if (!win.check) {
		++player2.countedFrames;
		++player1.countedFrames;
	}
	if (win.check) 
	{
		myTextures["EndGame"].load("src/" + win.status() + ".png");
		myTextures["EndGame"].render(512 - myTextures["EndGame"].getWidth() / 2, 384 - myTextures["EndGame"].getHeight() / 2);
		win.winText.load("Press any keys to continue ...", EndGameColor);
		win.winText.render(512 - win.winText.getWidth()/2, 550);
	}
	++countedFrames;
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
			//Initialize SDL_ttf
			if (TTF_Init() == -1)
			{
				printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
				success = false;
			}
		}
	}
	return success;
}

bool loadMedia()
{
	gFont = TTF_OpenFont("src/snake.ttf", 28);
	myTextures["headc"].load("src/Snake_head_down.png");
	myTextures["heads"].load("src/Snake_head_down.png");
	myTextures["apple"].load("src/apple.png");
	myTextures["body"].load("src/Snake_body.png");
	myTextures["background"].load("src/backmap1.png");
	myTextures["playground"].load("src/playground.png");
	myTextures["pve"].load("src/pve.png");
	myTextures["ex"].load("src/ex.png");
	myTextures["mainMenu"].load("src/pvp.png"); // pvp == place holder
	myTextures["moff"].load("src/moff.png");
	myTextures["mon"].load("src/mon.png");

	//Loading success flag
	bool success = true;
	//Open the font
	if (gFont == NULL)
	{
		printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}
	//Render text
	if (!player1.Score.load("SCORE : "+player1.Score.init(player1.Score.points), textColor))
	{
		printf("Failed to render text texture!\n");
		success = false;
	}
	if (!playMusic("src/back.wav")) {
		printf("Failed to load music!\n");
		success = false;
	}
	if (!myTextures["body"].check())
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	if (!myTextures["apple"].check())
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	if (!myTextures["headc"].check())
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	if (!myTextures["heads"].check())
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

	// Remove snake
	player1.body.clear();
	player1.Score.free();

	//Text destroy
	TTF_CloseFont(gFont);
	gFont = NULL;
	//Destroy window    
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;
	Mix_FreeMusic(gMusic);
	gMusic = NULL;

	//Quit SDL subsystems
	TTF_Quit();
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}


int main(int argc, char* args[])
{

	//Random generate
	srand(time(NULL));
	
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
						if (e.type == SDL_KEYDOWN) {
							if (win.check) bg = 1; else
							{
								player1.GetKey(&e);
								player2.GetKey(&e);
							}
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

