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
#include <ctime>
#include <cmath>
//supporting function for scoring
clock_t cur_time = clock();
int TotalTimeSetting = 60;
int TotalTime = TotalTimeSetting;
class QItem {
public:
	int row;
	int col;
	int dist;
	QItem(int x, int y, int w) : row(x), col(y), dist(w) {	}
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
		if (p.row - 1 >= 0 && visited[p.row - 1][p.col] == false) {
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

//Main loop flag
bool quit = false;
bool start = 0;
std::string number;

//Global variables
TTF_Font* gFont = NULL;
SDL_Color textColor = { 0, 0, 0 }, EndGameColor = { 240,240,240 };
SDL_Surface* iconSurface;

int bg = 1;
bool showguidebox = 0;
bool showsettingbox = 0;
const int totalBtn1 = 8;
const int totalBtn2 = 2;
const int totalSBtn = 12;
const int totalskin = 8;
const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;
bool init();
std::string operator+(std::string s, char c) {
	s += c;
	return s;
}

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
	void setBlendMode(SDL_BlendMode blending)
	{
		//Set blending function
		SDL_SetTextureBlendMode(mTexture, blending);
	}
	void setColor(int x, int y, int z) {
		SDL_SetTextureColorMod(mTexture, x, y, z);
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
bool cdMusic = 0;
Mix_Chunk* NB = NULL;
Mix_Chunk* eat = NULL;
Mix_Chunk* GO = NULL;
Mix_Chunk* click = NULL;
gTexture Sbodys;
gTexture Sbodyc;
gTexture Sheads;
gTexture Sheadc;
gTexture preheadc;
gTexture prebodyc;
gTexture preheads;
gTexture prebodys;
gTexture apple;
gTexture background;
gTexture playground;
gTexture boxtime;
gTexture pause;
gTexture ex;
gTexture newgame;
gTexture resume;
gTexture guide;
gTexture EndGame;
gTexture guidebox;
gTexture settingbox;
gTexture X;
gTexture Sbutton;
gTexture magicapple; 
gTexture goldenapple;
gTexture wallapple;
gTexture iceapple;
gTexture cherry;
gTexture wall;
gTexture countdown;
gTexture title;
gTexture settingbutton;
std::map<std::string, std::pair<int, int>> coorBtn{
	{"pause",{449,250}},
	{"ex",{376,600}},
	{"newgame",{376,235}},
	{"resume",{376,355}},
	{"guide",{376,475}},
	{"setting",{688,600}},
	{"X",{872,215}},
};
std::map<std::string, gTexture> myTextures = {
	{"bodys", Sbodys },
	{"bodyc", Sbodyc },
	{"headc", Sheadc },
	{"heads", Sheads },
	{"preheadP1", preheadc },
	{"prebodyP1", prebodyc },
	{"preheadP2", preheads },
	{"prebodyP2", prebodys },
	{"apple0", apple },
	{"background", background },
	{"MusicLeft", Sbutton},
	{"MusicRight", Sbutton},
	{"SFXLeft", Sbutton},
	{"SFXRight", Sbutton},
	{"TimeLeft", Sbutton},
	{"TimeRight", Sbutton},
	{"FSLeft", Sbutton},
	{"FSRight", Sbutton},
	{"P1Left", Sbutton},
	{"P1Right", Sbutton},
	{"P2Left", Sbutton},
	{"P2Right", Sbutton},
	{"newgame", newgame },
	{"resume", resume },
	{"guide", guide },
	{"pause", pause },
	{"ex", ex },
	{"timebox", boxtime },
	{"guide", guidebox },
	{"EndGame", EndGame },
	{"X", X },
	{"playground",playground},
	{"apple1",magicapple},
	{"apple3",wallapple},
	{"apple2",goldenapple},
	{"apple4",iceapple},
	{"wall",wall},
	{"Title",title},
	{"countdown",countdown},
	{"setting",settingbutton},
	{"settingbox",settingbox},
	{"apple5",cherry}
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
	std::string GetNumber(int x, int length)
	{
		std::string res = "";
		for (int i = 0; i < length; i++) {
			res += char(x % 10 + '0');
			x = x / 10;
		}
		for (int i = 0; i < length / 2; i++) std::swap(res[i], res[length - i - 1]);
		return res;
	}

private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	//Image dimensions
	int mWidth;
	int mHeight;
};
struct GameStatus {
	bool check = 0;
	std::string status(); 
	Text winText;
};
std::string NBtoS(int x) {
	std::string res = "";
	for (int i = 0; i < 5; i++) {
		res += char(x % 10 + '0');
		x = x / 10;
	}
	while (res.length()>0 && res[res.length() - 1] == '0') res.pop_back();
	for (int i = 0; i < res.length() / 2; i++) std::swap(res[i], res[res.length() - i - 1]);
	return (res=="") ? "0" : res;
}
GameStatus win;
Text timebox;
Text percentMusic;
int percentMusicNum = 100;
Text percentSFX;
int percentSFXNum = 100;
void SFXchange(int x) {
	Mix_VolumeChunk(eat, x);
	Mix_VolumeChunk(NB, x);
	Mix_VolumeChunk(GO, x);
	Mix_VolumeChunk(click, x);
}
bool FScreen = 0;
Text Time;
Text FullScreen;
void newGame();
bool checkSave();
void createSave();
class gBtn {
public:
	gBtn() {
		mPos.x = 0;
		mPos.y = 0;
		mPos.w = 0;
		mPos.h = 0;
		com = "";
	}
	void getInf(std::string temp, int x = -1, int y = -1) {
		if(x == -1)
		{
			mPos.x = coorBtn[temp].first;
			mPos.y = coorBtn[temp].second;
		}
		else {
			mPos.x = x;
			mPos.y = y;
		}
		mPos.w = myTextures[temp].getWidth();
		mPos.h = myTextures[temp].getHeight();
		com = temp;
	}
	void handleEvent(SDL_Event* e);
private:
	SDL_Rect mPos;
	std::string com;
};


gBtn gBtns1[totalBtn1];
gBtn gBtns2[totalBtn2];
gBtn SBT[totalSBtn];

void background1() {
	myTextures["background"].render(0, 0);
	myTextures["Title"].render(512 - myTextures["Title"].getWidth() / 2, 50);
	if (showguidebox) {
		myTextures["guidebox"].render(512 - myTextures["guidebox"].getWidth() / 2, 384 - myTextures["guidebox"].getHeight() / 2 + 35);
	}
	else {
		if (showsettingbox) {
			percentMusic.load(NBtoS(percentMusicNum) + "%", textColor);
			percentSFX.load(NBtoS(percentSFXNum) + "%", textColor);
			Time.load(NBtoS(TotalTimeSetting), textColor);
			FullScreen.load((FScreen) ? "on" : "off", textColor);
			myTextures["settingbox"].render(512 - myTextures["settingbox"].getWidth() / 2, 384 - myTextures["settingbox"].getHeight() / 2 + 35);
			percentMusic.render(400 - percentMusic.getWidth() / 2, 258);
			percentSFX.render(785 - percentSFX.getWidth() / 2, 258);
			Time.render(400 - Time.getWidth() / 2, 358);
			FullScreen.render(785 - FullScreen.getWidth() / 2, 358);
			myTextures["preheadP1"].render(320, 531);
			myTextures["prebodyP1"].render(288, 531);
			myTextures["preheadP2"].render(704, 531);
			myTextures["prebodyP2"].render(672, 531);
		}
		else {
			if (checkSave()) {
				myTextures["resume"].setBlendMode(SDL_BLENDMODE_BLEND);
			}
			else myTextures["resume"].setBlendMode(SDL_BLENDMODE_MUL);
			myTextures["newgame"].render(coorBtn["newgame"].first, coorBtn["newgame"].second);
			myTextures["setting"].render(coorBtn["setting"].first, coorBtn["setting"].second);
			myTextures["guide"].render(coorBtn["guide"].first, coorBtn["guide"].second);
			myTextures["resume"].render(coorBtn["resume"].first, coorBtn["resume"].second);
			myTextures["ex"].render(coorBtn["ex"].first, coorBtn["ex"].second);
		}
	}
}
class Snake_pos {
public:
	// Get position of snake parts
	int posX;
	int posY;
	// Get direct of snake parts
	// right = 0, left = 1, up = 2, down = 3
	int cur_dir;
	Snake_pos(int x, int y, int z) {
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
std::map<int, std::string> Skin{
	{0,"0"},
	{1,"1"},
	{2,"2"},
	{3,"3"},
	{4,"4"},
	{5,"5"},
	{6,"6"},
	{7,"7"},
	{8,"8"},
};
//Set time for unique apple
std::map<char, int> AT{
	{'0',10},
	{'1',7},
	{'2',5},
	{'3',9},
	{'4',9},
	{'5',9},
};
struct Swall {
	int posX;
	int posY;
	Swall(int x, int y) : posX(x), posY(y) {};
};
struct Snake {
	// Set up snake head 
	const std::string particular;
	std::string realKey = "0";
	int a[20][12];
	Snake_pos head{ 1,1,3 };
	std::vector<Snake_pos> body;
	std::vector<Swall> wall;
	std::stack<int> PRESSKEY;
	bool Has_Eaten_Apple = 0;
	int appleX = 10;
	int appleY = 10;
	Text Score;
	int countedStep = 0;
	int countedFrames = 0;
	int setSpeed = 10;
	bool lose = 0;
	int mind = -1;
	bool gotRead = 0;
	int stun = 0;
	int countwall = 0;
	std::string status = "Snake";
	Uint8 SkinCode = 0;
	int Appletime = 10;
	void randApple() {
		//normal: 0, magic : 1, golden: 2, wall: 3, ice: 4, cherry: 5
		// prob 0 : 30   magic:20   golden:5   wall:15   ice:15   cherry: 15
		int r = rand() % 100;
		int prob[6] = { 30,20,5,15,15,15 };
		char aKey = '0';
		for (int i = 0; i < 6; i++) {
			if (r < prob[i]) {
				aKey = i + 48;
				break;
			}
			r -= prob[i];
		}
		realKey = "";
		realKey += aKey;
		Appletime = AT[aKey];
	}
	void getPos() {
		std::ifstream in;
		in.open("src/" + particular + ".txt", std::ios::trunc);
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
	void draw() {
		if (lose) status = "Dead";
		for (auto c : body) {
			myTextures["body" + particular].render(c.posX * 32 + (particular[0] - 'a') * 32, c.posY * 32 + 64);
		}
		for (auto c : wall) {
			myTextures["wall"].render(c.posX * 32 + (particular[0] - 'a') * 32 + 1, c.posY * 32 + 65);
		}
		myTextures["head" + particular].load("src/Skin/"+Skin[SkinCode] + "/" + status + "_head_" + dir[head.cur_dir] + ".png");
		myTextures["head" + particular].render(head.posX * 32 + (particular[0] - 'a') * 32, head.posY * 32 + 64);
		myTextures["apple"+realKey].render(appleX * 32 + (particular[0] - 'a') * 32, appleY * 32 + 64);
		for (int i = 0; i < Appletime; i++)
			myTextures["apple" + realKey].render(i*32 + (particular[0] - 'a') * 32,  5);
	}
	void move()
	{
		status = "Snake";
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
			break;
		case 1:
			--head.posX;
			break;
		case 2:
			--head.posY;
			break;
		case 3:
			++head.posY;
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
			Mix_PlayChannel(6, eat, 0);
			break;
		default:
			break;
		}
		if (Has_Eaten_Apple) {
			if (realKey[0] == '1')
			{
				int temp = rand() % 3;
				std::string t;
				t += char(temp + '3');
				realKey = t;
			}
			switch (realKey[0]) {
			case '0':
				Score.points += 100 * mind / countedStep + mind;
				break;
			case '2':
				if (setSpeed > 4)
				{
					Score.points *= 2;
					setSpeed -= 4;
				}
				else {
					Score.points += 100 * mind / countedStep + mind;
				}
				break;
			case '3':
				countwall = rand() % 3 + 1;
				Score.points += (100 * mind / countedStep + mind);
				break;
			case '4':
				stun = 5;
				Score.points += (100 * mind / countedStep + mind);
				break;
			case '5':
				Score.points += (100 * mind / countedStep + mind) * 2;
				break;
			default:
				break;
			}
			Snake_pos* newbody = new Snake_pos{ preX , preY , predir };
			body.push_back(*(newbody));
		}
		if (Has_Eaten_Apple || Appletime == 0) {
			mind = -1;
			countedStep = 0;
			randApple();
			while (a[appleY][appleX] != 0) {
				appleY = rand() % 20;
				appleX = rand() % 12;
			}
		}
		if (!lose)
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
		for (auto c : wall) {
			a[c.posY][c.posX] = 2;
		}
		Has_Eaten_Apple = 0;
		a[head.posY][head.posX] = 1;
		a[appleY][appleX] = 10;
		out.open("src/" + particular + ".txt", std::ios::trunc);
		for (int i = 0; i < 20; i++) {
			for (int j = 0; j < 12; j++) out << a[i][j] << " ";
			out << '\n';
		}
		out.close();
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
				default:
					break;
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
	void newGame() {
		wall.clear();
		body.clear();
		Appletime = 10;
		lose = 0;
		Score.points = 0;
		head.posX = 1;
		head.posY = 1;
		head.cur_dir = 3;
		appleX = 10;
		appleY = 10;
		a[10][10] = 10;
		realKey = "0";
		countedFrames = 0;
		countedStep = 0;
		mind = -1;
		setSpeed = 10;
		countwall = 0;
		stun = 0;
		status = "Snake";
		for (int i = 0; i < 20; i++) {
			for (int j = 0; j < 12; j++) {
				a[i][j] = 0;
			}
		}
	}
	void free() {
		wall.clear();
		body.clear();
		Score.free();
	}
	void save() {
		std::ofstream o;
		o.open("src/" + particular + "save.txt", std::ios::trunc);
		o << head.posX << " " << head.posY << " " << head.cur_dir << '\n';
		o << appleX << " " << appleY << '\n';
		o << countedFrames << " " << countedStep << " " << setSpeed << " " << TotalTime << " " << mind << " " << realKey << " " << stun << " " << status << '\n';
		o << Appletime << '\n';
		o << body.size() << '\n';
		for (auto c : body)
			o << c.posX << " " << c.posY << " " << c.cur_dir << '\n';
		o << wall.size() << '\n';
		for (auto c : wall)
			o << c.posX << " " << c.posY << '\n';
		o << Score.points;
		o.close();
	}
	void read() {
		std::ifstream r;
		r.open("src/" + particular + "save.txt"); 
		r >> head.posX >> head.posY >> head.cur_dir;
		r >> appleX >> appleY >> countedFrames >> countedStep >> setSpeed >> TotalTime >> mind >> realKey >> stun >> status>>Appletime;
		Snake_pos temp(0,0,0);
		int s;
		r >> s;
		for (int i = 0; i < s; i++) {
			r >> temp.posX >> temp.posY >> temp.cur_dir;
			body.push_back(temp);
		}
		Swall wtemp(0, 0);
		r >> s;
		for (int i = 0; i < s; i++) {
			r >> wtemp.posX >> wtemp.posY;
			wall.push_back(wtemp);
		}
		r >> Score.points;
		r.close();
	}
};
Snake player1("c"), player2("s");
void gBtn::handleEvent(SDL_Event* e)
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
				if (com == "newgame" && e->button.button == SDL_BUTTON_LEFT) {
					bg = 2;
					newGame();
					Mix_HaltMusic();
					Mix_FreeMusic(gMusic);
					gMusic = NULL;
				}
				else if (com == "resume" && e->button.button == SDL_BUTTON_LEFT) {
					if (checkSave())
					{
						myTextures["headc"].load("src/Skin/" + Skin[player1.SkinCode] + "/Snake_head_down.png");
						myTextures["bodyc"].load("src/Skin/" + Skin[player1.SkinCode] + "/Snake_body.png");
						myTextures["heads"].load("src/Skin/" + Skin[player2.SkinCode] + "/Snake_head_down.png");
						myTextures["bodys"].load("src/Skin/" + Skin[player2.SkinCode] + "/Snake_body.png");
						start = 0;
						number = "3";
						cdMusic = 0;
						bg = 2;
						Mix_HaltMusic();
						Mix_FreeMusic(gMusic);
						gMusic = NULL;
					}
				}
				else if (com == "X" && e->button.button == SDL_BUTTON_LEFT) {
					showguidebox = 0;
					showsettingbox = 0;
					std::ofstream st;
					st.open("src/setting.txt");
					st << percentMusicNum << " " << percentSFXNum << " " << TotalTimeSetting << " " << FScreen;
					st.close();
				}
				else if (com == "pause" && e->button.button == SDL_BUTTON_LEFT) {
					createSave();
					if (!win.check) bg = 1;
					if (!playMusic("src/back.wav")) {
						printf("Failed to load music!\n");
					}
				}
				else if (com == "ex" && e->button.button == SDL_BUTTON_LEFT) {
					quit = true;
				}
				else if (com == "setting" && e->button.button == SDL_BUTTON_LEFT) {
					showsettingbox = 1;
				}
				else if (com == "MusicLeft" && e->button.button == SDL_BUTTON_LEFT) {
					if (percentMusicNum == 0) percentMusicNum = 100; else
						percentMusicNum -= 25;
					Mix_VolumeMusic(128 / 100 * percentMusicNum);
				}
				else if (com == "SFXLeft" && e->button.button == SDL_BUTTON_LEFT) {
					if (percentSFXNum == 0) percentSFXNum = 100; else
						percentSFXNum -= 25;
					SFXchange(percentSFXNum);
				}
				else if (com == "SFXRight" && e->button.button == SDL_BUTTON_LEFT) {
					if (percentSFXNum == 100) percentSFXNum = 0; else
						percentSFXNum += 25;
					SFXchange(percentSFXNum);
				}
				else if (com == "MusicRight" && e->button.button == SDL_BUTTON_LEFT) {
					if (percentMusicNum == 100) percentMusicNum = 0; else
						percentMusicNum += 25;
					Mix_VolumeMusic(128 / 100 * percentMusicNum);
				}
				else if (com == "TimeLeft" && e->button.button == SDL_BUTTON_LEFT) {
					if (TotalTimeSetting == 30) TotalTimeSetting = 999; else
						if (TotalTimeSetting == 999) TotalTimeSetting = 120; else
							TotalTimeSetting /= 2;
				}
				else if (com == "TimeRight" && e->button.button == SDL_BUTTON_LEFT) {
					if (TotalTimeSetting == 999) TotalTimeSetting = 30; else
						if (TotalTimeSetting == 120) TotalTimeSetting = 999; else
							TotalTimeSetting *= 2;
				}
				else if ((com == "FSLeft" || com == "FSRight") && e->button.button == SDL_BUTTON_LEFT) {
					if (!FScreen) {
						SDL_SetWindowFullscreen(gWindow, SDL_WINDOW_FULLSCREEN);
						FScreen = 1;
					}
					else {
						SDL_SetWindowFullscreen(gWindow,0);
						FScreen = 0;
					}
				}
				else if (com == "P1Left" && e->button.button == SDL_BUTTON_LEFT) {
					if (player1.SkinCode == 0) player1.SkinCode = totalskin; else
						player1.SkinCode--;
					myTextures["preheadP1"].load("src/Skin/" + Skin[player1.SkinCode] + "/Snake_head_right.png");
					myTextures["prebodyP1"].load("src/Skin/" + Skin[player1.SkinCode] + "/Snake_body.png");
				}
				else if (com == "P1Right" && e->button.button == SDL_BUTTON_LEFT) {
					if (player1.SkinCode == totalskin) player1.SkinCode = 0; else
						player1.SkinCode++;
					myTextures["preheadP1"].load("src/Skin/" + Skin[player1.SkinCode] + "/Snake_head_right.png");
					myTextures["prebodyP1"].load("src/Skin/" + Skin[player1.SkinCode] + "/Snake_body.png");
				}
				else if (com == "P2Left" && e->button.button == SDL_BUTTON_LEFT) {
					if (player2.SkinCode == 0) player2.SkinCode = totalskin; else
						player2.SkinCode--;
					myTextures["preheadP2"].load("src/Skin/" + Skin[player2.SkinCode] + "/Snake_head_right.png");
					myTextures["prebodyP2"].load("src/Skin/" + Skin[player2.SkinCode] + "/Snake_body.png");
				}
				else if (com == "P2Right" && e->button.button == SDL_BUTTON_LEFT) {
					if (player2.SkinCode == totalskin) player2.SkinCode = 0; else
						player2.SkinCode++;
					myTextures["preheadP2"].load("src/Skin/" + Skin[player2.SkinCode] + "/Snake_head_right.png");
					myTextures["prebodyP2"].load("src/Skin/" + Skin[player2.SkinCode] + "/Snake_body.png");
				}
				else if (com == "guide" && e->button.button == SDL_BUTTON_LEFT) {
					showguidebox = 1;
				}
				break;
			default:
				break;
				/*case SDL_MOUSEBUTTONUP:

					break;*/
			}
		}
	}
}
std::string GameStatus::status() {
	if (player1.lose && !player2.lose) return "p2";
	if (player2.lose && !player1.lose) return "p1";
	if (player1.Score.points == player2.Score.points) return "Draw";
	return (player1.Score.points > player2.Score.points) ? "p1" : "p2";
}
void CD() {
	if (!cdMusic) {
		if (!Mix_Playing(5)) {
			if(number[0] > '0')
				Mix_PlayChannel(5, NB, 0); 
			else
				Mix_PlayChannel(5, GO, 0);
		}
		cdMusic = 1;
	}
	myTextures["countdown"].load("src/" + number + ".png");
	myTextures["countdown"].render(512 - myTextures["countdown"].getWidth() / 2, 384 - myTextures["countdown"].getHeight() / 2);
	if (int((clock() - cur_time) / CLOCKS_PER_SEC) == 1) {
		number[0]--;
		cur_time = clock();
		cdMusic = 0;
	}
	if (number[0] == '0'-1) {
		start = 1;
		cur_time = clock();
			if (!playMusic("src/ingame.wav")) {
				printf("Failed to load music!\n");
			}
	}
}
bool wrongwall(int x, int y ,int grid[20][12]) {
	if (grid[x][y] != 0) return 1;
	for (int i = -1; i < 2; i++)
		for (int j = -1; j < 2; j++) {
			if (x + i < 0 || x + i>19 || y + j < 0 || y + j>11) continue;
			if (grid[x+i][y+j] == 1) return 1;
		}
	return 0;
}
void background2() {
	myTextures["background"].render(0, 0);
	timebox.load(timebox.GetNumber(TotalTime, 3), textColor);
	if (!win.check)
		myTextures["pause"].render(coorBtn["pause"].first, coorBtn["pause"].second);
	myTextures["playground"].render(62, 62);
	myTextures["playground"].render(574, 62);
	myTextures["timebox"].render(452, 120);
	timebox.render(480, 127);
	player1.Score.load("SCORE : " + player1.Score.GetNumber(player1.Score.points, 5), textColor);
	player1.Score.render(64, 64 - player1.Score.getHeight());
	player1.draw();
	player1.getPos();
	player2.Score.load("SCORE : " + player1.Score.GetNumber(player2.Score.points, 5), textColor);
	player2.Score.render(576, 64 - player2.Score.getHeight());
	player2.draw();
	player2.getPos();
	if(start)
	{
		while(player1.countwall>0) {
			if (player2.wall.size() > 20) player1.countwall = 0;
			int x = rand() % 20;
			int y = rand() % 12;
			while (wrongwall(x, y, player2.a)) {
				x = rand() % 20;
				y = rand() % 12;
			}
			Swall* newwall = new Swall{ y,x };
			player2.wall.push_back(*(newwall));
			--player1.countwall;
		}
		while(player2.countwall>0) {
			if (player1.wall.size() > 20) player2.countwall = 0;
			int x = rand() % 20;
			int y = rand() % 12;
			while (wrongwall(x,y,player1.a)) {
				x = rand() % 20;
				y = rand() % 12;
			}
			Swall* newwall = new Swall{ y,x };
			player1.wall.push_back(*(newwall));
			--player2.countwall;
		}
		if (TotalTime == 0) {
			player1.lose = true;
			player2.lose = true;
		}
		if (int((clock() - cur_time) / CLOCKS_PER_SEC) == 1 && !player1.lose && !player2.lose) {
			cur_time = clock();
			--TotalTime;
			--player1.Appletime;
			--player2.Appletime;
		}
		if (!(player1.lose)) {
			if (player1.countedFrames > player1.setSpeed) {
				if (player2.stun > 0)
				{
					player1.status = "Stunned";
					--player2.stun;
				}
				else player1.move();
				player1.countedFrames = 0;
			}
		}
		else {
			win.check = 1;
		}
		if (!(player2.lose)) {
			if (player2.countedFrames > player2.setSpeed) {
				if (player1.stun > 0)
				{
					--player1.stun;
					player2.status = "Stunned";
				}
				else 
				player2.move();
				player2.countedFrames = 0;
			}
		}
		else {
			win.check = 1;
		}
		if (!win.check) {
			++player2.countedFrames;
			++player1.countedFrames;
		}
	}
	else {

		CD();
	}
	if (win.check)
	{
		myTextures["EndGame"].load("src/" + win.status() + ".png");
		myTextures["EndGame"].render(512 - myTextures["EndGame"].getWidth() / 2, 384 - myTextures["EndGame"].getHeight() / 2);
		win.winText.render(512 - win.winText.getWidth() / 2, 550);
	}
}
void newGame() {
	myTextures["headc"].load("src/Skin/" + Skin[player1.SkinCode] + "/Snake_head_down.png");
	myTextures["bodyc"].load("src/Skin/" + Skin[player1.SkinCode] + "/Snake_body.png");
	myTextures["heads"].load("src/Skin/" + Skin[player2.SkinCode] + "/Snake_head_down.png");
	myTextures["bodys"].load("src/Skin/" + Skin[player2.SkinCode] + "/Snake_body.png");
	cdMusic = 0;
	start = 0;
	number = "3";
	win.check = 0;
	player1.newGame();
	while (!player1.PRESSKEY.empty()) {
		player1.PRESSKEY.pop();
	}
	player2.newGame();
	while (!player2.PRESSKEY.empty()) {
		player2.PRESSKEY.pop();
	}
	TotalTime = TotalTimeSetting;
}
bool checkSave() {
	std::ifstream o1, o2;
	o1.open("src/ssave.txt");
	o2.open("src/csave.txt");
	if ((o1) && (o2)) {
		if (!player1.gotRead) {
			player1.read();
			player1.gotRead = 1;
		}
		if (!player2.gotRead) {
			player2.read();
			player2.gotRead = 1;
		}
		o1.close();
		o2.close();
		return 1;
	}
	o1.close();
	o2.close();
	return 0;
}
void createSave() {
	player1.save();
	player1.gotRead = 0;
	player1.free();
	player2.save();
	player2.gotRead = 0;
	player2.free();
}
bool init()
{
	//Initialization flag
	bool success = true;
	//Create window
	gWindow = SDL_CreateWindow("SNAKE", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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
			if (!(Mix_Init(MIX_INIT_MP3) & MIX_INIT_MP3))
			{
				printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
				success = false;
			}
			if (Mix_OpenAudio(48000, AUDIO_S16SYS, 2, 2048) < 0)
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
	//Load setting
	std::ifstream st;
	st.open("src/setting.txt");
	st >> percentMusicNum >> percentSFXNum >> TotalTimeSetting >> FScreen;
	Mix_VolumeMusic(128 / 100 * percentMusicNum);
	SFXchange(percentSFXNum);
	if (FScreen) {
		SDL_SetWindowFullscreen(gWindow, SDL_WINDOW_FULLSCREEN);
	}
	else {
		SDL_SetWindowFullscreen(gWindow, 0);
	}
	st.close();
	//Loading image and music
	iconSurface = IMG_Load("src/windowIcon.png");
	SDL_SetWindowIcon(gWindow, iconSurface);
	gFont = TTF_OpenFont("src/snake.ttf", 28);
	GO = Mix_LoadWAV("src/GO.wav");
	NB = Mix_LoadWAV("src/NB.wav");
	click = Mix_LoadWAV("src/click.wav");
	eat = Mix_LoadWAV("src/eat.wav");
	myTextures["headc"].load("src/Skin/" + Skin[player1.SkinCode] + "/Snake_head_down.png");
	myTextures["heads"].load("src/Skin/" + Skin[player2.SkinCode] + "/Snake_head_down.png");
	myTextures["preheadP1"].load("src/Skin/" + Skin[player1.SkinCode] + "/Snake_head_right.png");
	myTextures["preheadP2"].load("src/Skin/" + Skin[player2.SkinCode] + "/Snake_head_right.png");
	myTextures["apple0"].load("src/apple.png");
	myTextures["bodyc"].load("src/Skin/"+Skin[player1.SkinCode]+"/Snake_body.png");
	myTextures["bodys"].load("src/Skin/"+Skin[player2.SkinCode]+"/Snake_body.png");
	myTextures["prebodyP1"].load("src/Skin/" + Skin[player1.SkinCode] + "/Snake_body.png");
	myTextures["prebodyP2"].load("src/Skin/" + Skin[player2.SkinCode] + "/Snake_body.png");
	myTextures["background"].load("src/backmap1.png");
	myTextures["playground"].load("src/playground.png");
	myTextures["timebox"].load("src/time_box.png");
	myTextures["ex"].load("src/ex.png");
	myTextures["newgame"].load("src/newgame.png");
	myTextures["resume"].load("src/resume.png");
	myTextures["guide"].load("src/guide.png");
	myTextures["pause"].load("src/pause.png");
	myTextures["guidebox"].load("src/guidebox.png");
	myTextures["X"].load("src/X.png");
	myTextures["apple1"].load("src/magicapple.png");
	myTextures["apple4"].load("src/iceapple.png");
	myTextures["apple2"].load("src/goldenapple.png");
	myTextures["apple3"].load("src/wallapple.png");
	myTextures["apple5"].load("src/cherry.png");
	myTextures["wall"].load("src/wall.png");
	myTextures["Title"].load("src/title.png");
	myTextures["setting"].load("src/setting.png");
	myTextures["settingbox"].load("src/setting_box.png");
	win.winText.load("Press SPACE to continue ...", EndGameColor);
	myTextures["MusicLeft"].load("src/Sbutton.png");
	myTextures["MusicRight"].load("src/Sbutton.png");
	myTextures["SFXLeft"].load("src/Sbutton.png");
	myTextures["SFXRight"].load("src/Sbutton.png");
	myTextures["TimeLeft"].load("src/Sbutton.png");
	myTextures["TimeRight"].load("src/Sbutton.png");
	myTextures["FSLeft"].load("src/Sbutton.png");
	myTextures["FSRight"].load("src/Sbutton.png");
	myTextures["P1Left"].load("src/Sbutton.png");
	myTextures["P1Right"].load("src/Sbutton.png");
	myTextures["P2Left"].load("src/Sbutton.png");
	myTextures["P2Right"].load("src/Sbutton.png");
	//Loading success flag
	bool success = true;
	//Open the font

	if (gFont == NULL)
	{
		printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}
	if (!iconSurface)
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	if (!myTextures["apple1"].check())
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	if (!myTextures["MusicLeft"].check())
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	if (!myTextures["apple" + '2'].check())
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	if (!myTextures["countdown"].check())
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	if (!myTextures["wall"].check())
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	if (!myTextures["apple3"].check())
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	if (!myTextures["apple4"].check())
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	if (!myTextures["apple5"].check())
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	if (!myTextures["guide"].check())
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	if (!myTextures["guidebox"].check())
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	if (!myTextures["pause"].check())
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	if (!myTextures["resume"].check())
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	if (!myTextures["newgame"].check())
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	if (!myTextures["settingbox"].check())
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	if (!myTextures["setting"].check())
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	if (!playMusic("src/back.wav")) {
		printf("Failed to load music!\n");
		success = false;
	}

	if (!myTextures["bodyc"].check())
	{
		printf("Failed to load texture image!\n");
		success = false;
	}if (!myTextures["bodys"].check())
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	if (!myTextures["apple0"].check())
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	if (!myTextures["timebox"].check())
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
	gBtns1[0].getInf("newgame");
	gBtns1[1].getInf("guide");
	gBtns1[2].getInf("resume");
	gBtns1[3].getInf("ex");
	gBtns1[6].getInf("X");
	gBtns1[7].getInf("setting");

	gBtns2[0].getInf("pause");

	SBT[0].getInf("MusicLeft" , 328,251);
	SBT[1].getInf("MusicRight" , 448,251);
	SBT[2].getInf("SFXLeft" , 712,251);
	SBT[3].getInf("SFXRight" , 832,251);
	SBT[4].getInf("TimeLeft" , 328,347);
	SBT[5].getInf("TimeRight" , 448,347);
	SBT[6].getInf("FSLeft" , 712,347);
	SBT[7].getInf("FSRight" , 832,347);
	SBT[8].getInf("P1Left" , 168,523);
	SBT[9].getInf("P1Right" , 448,523);
	SBT[10].getInf("P2Left" , 552,523);
	SBT[11].getInf("P2Right" , 832,523);

	return success;
}

void close()
{

	// Remove snake
	player1.free();
	player2.free();


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
	SDL_FreeSurface(iconSurface);

	//Quit SDL subsystems
	TTF_Quit();
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//Random generate
	srand(time(nullptr));

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
					if (e.type == SDL_MOUSEBUTTONDOWN) {
							Mix_PlayChannel(-1, click, 0);
					}
					if (bg == 1) {
						if (showguidebox) {
							gBtns1[6].handleEvent(&e);
						}
						else
						{
							if (showsettingbox) 
							{
								gBtns1[6].handleEvent(&e);
								for (int i = 0; i < totalSBtn; i++) {
									SBT[i].handleEvent(&e);
								}
							}
							else {
								for (int i = 0; i < totalBtn1; ++i)
								{
									int temp = bg;
									gBtns1[i].handleEvent(&e);
									if (bg != temp) {
										break;
									}
								}
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
						if (win.check) {
							if (e.key.keysym.sym == SDLK_SPACE) 
							{
									if (!playMusic("src/back.wav")) {
										printf("Failed to load music!\n");
									}
								bg = 1;
								std::remove("src/ssave.txt");
								std::remove("src/csave.txt");
							}
						}
						if (e.type == SDL_KEYDOWN) {
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
					cur_time = clock();
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

