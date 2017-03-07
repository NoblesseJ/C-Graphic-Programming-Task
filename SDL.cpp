#include <SDL/SDL.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
const Uint32 delay = 1000, rectDelay = 100;
const Uint32 width = 500, height = 500;
const Uint32 hmiddle = height / 2, wmiddle = width / 2;
const int hourNo = 0, minNo = 1, secNo = 2;
const double pi = 3.14;
const int handLen[3] = { 80,140,200 };
const int handThick[3] = { 5,3,2 };
const SDL_Point centre[3] = { 5,0,3,0,2,0 };
SDL_Event timerEvent[2];
SDL_UserEvent timerUserEvent[2];
struct _time {
	int hms[3];
}tme;
#define hh hms[hourNo]
#define mm hms[minNo]
#define ss hms[secNo]

struct _sdl_struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Event enent;
	SDL_TimerID timerId;
	SDL_TimerID rectId;
	SDL_Texture* imageBackground;
	SDL_Surface* rectSurface;
	SDL_Surface* nothing;
	SDL_Surface* handSurface;
	SDL_Rect backgroundRect;
}sdlStruct;
SDL_Rect hands[3];
void fillTime() {
	time_t timep;
	struct tm *p;
	time(&timep);
	p = gmtime(&timep);
	tme.hh = (p->tm_hour + 8) % 24;
	tme.mm = p->tm_min;
	tme.ss = p->tm_sec;
}
double timeToAngle(int t) {
	double x = t;
	return x / 60 * 2 * pi;
}
double hourToAngle(int t) {
	double x = t % 12;
	return x / 12 * 2 * pi;
}
double timeToAngleEx(int type, int t) {
	if (type == hourNo)return(hourToAngle(t));
	else
	{
		return timeToAngle(t);
	}
}
void drawHand(int type) {
	SDL_FillRect(sdlStruct.handSurface, &sdlStruct.backgroundRect, 0x0000009f|rand());
	SDL_Texture* hand = SDL_CreateTextureFromSurface(sdlStruct.renderer, sdlStruct.handSurface);
	SDL_RenderCopyEx(sdlStruct.renderer, hand, &sdlStruct.backgroundRect, hands + type, 180/pi*(pi+timeToAngleEx(type, tme.hms[type])), centre+type, SDL_FLIP_VERTICAL);
	SDL_DestroyTexture(hand);
}
void drawBackground() {
	SDL_RenderCopy(sdlStruct.renderer, sdlStruct.imageBackground, NULL, &sdlStruct.backgroundRect);
}

Uint32 timerCallback(Uint32 interval, void* param) {
	SDL_PushEvent(timerEvent);
	return(interval);

}
Uint32 rectCallback(Uint32 interval, void* param) {
	SDL_PushEvent(&timerEvent[1]);
	return interval;
}
void eventInit(){
    for(int i=0;i<2;i++){
        timerUserEvent[i].type = SDL_USEREVENT;
        timerUserEvent[i].code = i;
        timerUserEvent[i].data1 = NULL;
        timerUserEvent[i].data2 = NULL;

        timerEvent[i].type = SDL_USEREVENT;
        timerEvent[i].user = timerUserEvent[i];
    }
}
void drawRect() {
	SDL_Texture *rect = SDL_CreateTextureFromSurface(sdlStruct.renderer, sdlStruct.rectSurface);
	SDL_RenderCopy(sdlStruct.renderer, rect, NULL, &sdlStruct.backgroundRect);
	SDL_DestroyTexture(rect);
}
void addNewRect() {
	SDL_Rect rect;
	rect.x = rand() % width;
	rect.y = rand() % height;
	rect.h = rand() % (height - rect.y);
	rect.w = rand() % (width - rect.x);
	SDL_FillRect(sdlStruct.rectSurface, &rect, rand()& 0xffffff1f);
}
void deleteRect() {
	SDL_FillRect(sdlStruct.nothing, &sdlStruct.backgroundRect, UINT_MAX);
	SDL_BlitSurface(sdlStruct.nothing, &sdlStruct.backgroundRect, sdlStruct.rectSurface, NULL);
}
void init() {
	Uint32 rmask, gmask, bmask, amask;
	SDL_Init(SDL_INIT_EVERYTHING);
	sdlStruct.window = SDL_CreateWindow("Time",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height, SDL_WINDOW_SHOWN);
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
	sdlStruct.rectSurface = SDL_CreateRGBSurface(0, width, height, 32,
		rmask, gmask, bmask, amask);
	sdlStruct.nothing = SDL_CreateRGBSurface(0, width, height, 32,
		rmask, gmask, bmask, amask);
	sdlStruct.handSurface = SDL_CreateRGBSurface(0, width, height, 32,
		rmask, gmask, bmask, amask);
	sdlStruct.renderer = SDL_CreateRenderer(sdlStruct.window, -1, SDL_RENDERER_ACCELERATED);
	sdlStruct.timerId = SDL_AddTimer(delay, timerCallback, NULL);
	sdlStruct.rectId = SDL_AddTimer(rectDelay, rectCallback, NULL);
	sdlStruct.imageBackground = IMG_LoadTexture(sdlStruct.renderer, "background.jpg");
	if (!sdlStruct.imageBackground)
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!", "could not open backgorund.jpg", sdlStruct.window);
	sdlStruct.backgroundRect.h = height;
	sdlStruct.backgroundRect.w = width;
	sdlStruct.backgroundRect.x = 0;
	sdlStruct.backgroundRect.y = 0;
	srand((unsigned)time(NULL));
	SDL_FillRect(sdlStruct.handSurface, &sdlStruct.backgroundRect, 0x000000ff);
	for (int i = 0; i < 3; i++) {
		hands[i].x = wmiddle - handThick[i];
		hands[i].y = hmiddle ;
		hands[i].w = 2 * handThick[i];
		hands[i].h = handLen[i];
	}
	eventInit();
}
void eventLoop() {
	while (1) {
		while (SDL_PollEvent(&sdlStruct.enent)) {
			SDL_RenderClear(sdlStruct.renderer);
			switch (sdlStruct.enent.type) {
			case SDL_QUIT:return;
			case SDL_USEREVENT:
				switch (sdlStruct.enent.user.code) {
				case 0:
					fillTime();
					/*for (int type = 0; type<3; type++)setDraw(type);*/
					setDraw();
					printf("timer 0\n");
					break;
				case 1:
					addNewRect();
					printf("timer 1\n");
					break;

				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				printf("mouse!\n");
				deleteRect();
				break;
			}
			drawBackground();
			drawRect();
			for (int type = 0; type<3; type++)drawHand(type);
			SDL_RenderPresent(sdlStruct.renderer);
		}
	}
}
void done() {
	SDL_DestroyWindow(sdlStruct.window);
	SDL_DestroyRenderer(sdlStruct.renderer);
	SDL_FreeSurface(sdlStruct.rectSurface);
	SDL_FreeSurface(sdlStruct.nothing);
	SDL_FreeSurface(sdlStruct.handSurface);
	SDL_RemoveTimer(sdlStruct.timerId);
	SDL_RemoveTimer(sdlStruct.rectId);
	SDL_Quit();
}
int main(int argc, char** argv)
{
	init();//启动SDL
	eventLoop();
	done();
	return 0;
}
