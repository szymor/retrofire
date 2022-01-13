#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_gfxPrimitives.h>

#define SCREEN_WIDTH		(320)
#define SCREEN_HEIGHT		(240)
#define SCREEN_BPP			(32)

SDL_Surface *screen = NULL;
int fps = 0;
bool fps_on = false;

Uint8 fire[SCREEN_HEIGHT][SCREEN_WIDTH];

Uint32 getPixel(SDL_Surface *s, int x, int y)
{
	if (x < 0 || x >= s->w || y < 0 || y >= s->h)
		return 0;
	int i = y * s->w + x;
	Uint32 *p = (Uint32*)s->pixels;
	return p[i];
}

void setPixel(SDL_Surface *s, int x, int y, Uint32 c)
{
	if (x < 0 || x >= s->w || y < 0 || y >= s->h)
		return;
	int i = y * s->w + x;
	Uint32 *p = (Uint32*)s->pixels;
	p[i] = c;
}

void fps_counter(double dt)
{
	static double total = 0;
	static int count = 0;
	total += dt;
	++count;
	if (total > 1.0)
	{
		fps = count;
		total -= 1.0;
		count = 0;
	}
}

void fps_draw(void)
{
	char string[8] = "";
	sprintf(string, "%d", fps);
	stringRGBA(screen, 0, 0, string, 255, 255, 255, 255);
}

int main(int argc, char *argv[])
{
	srand(time(NULL));
	SDL_Init(SDL_INIT_VIDEO);
	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_HWSURFACE | SDL_DOUBLEBUF);
	//SDL_EnableKeyRepeat(100, 50);
	SDL_ShowCursor(SDL_DISABLE);

    SDL_Surface *palette = IMG_Load("palette.png");
    palette = SDL_DisplayFormat(palette);
    for (int y = 0; y < SCREEN_HEIGHT; ++y)
        for (int x = 0; x < SCREEN_WIDTH; ++x)
        {
            fire[y][x] = 0;
        }

	bool quit = false;
	Uint32 curr = SDL_GetTicks();
	Uint32 prev = curr;
	while (!quit)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_RETURN:
							fps_on = !fps_on;
							break;
						case SDLK_ESCAPE:
							quit = true;
							break;
					}
					break;
				case SDL_QUIT:
					quit = true;
					break;
			}
		}
		curr = SDL_GetTicks();
		Uint32 delta = curr - prev;
		prev = curr;
		double dt = delta / 1000.0;
		fps_counter(dt);

        // processing
        for (int x = 0; x < SCREEN_WIDTH; ++x)
        {
            fire[SCREEN_HEIGHT - 1][x] = rand() % 256;
        }
        for (int y = 0; y < SCREEN_HEIGHT; ++y)
            for (int x = 0; x < SCREEN_WIDTH; ++x)
            {
                fire[y][x] = ((fire[(y + 2) % SCREEN_HEIGHT][x] +
                                fire[(y + 1) % SCREEN_HEIGHT][(x - 1 + SCREEN_WIDTH) % SCREEN_WIDTH] +
                                fire[(y + 1) % SCREEN_HEIGHT][x] +
                                fire[(y + 1) % SCREEN_HEIGHT][(x + 1) % SCREEN_WIDTH])
                                * 1024) / 4097;
            }

		SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
		//SDL_BlitSurface(heightmap, NULL, screen, NULL);
		SDL_LockSurface(screen);
        for (int y = 0; y < SCREEN_HEIGHT; ++y)
            for (int x = 0; x < SCREEN_WIDTH; ++x)
            {
                Uint32 c = getPixel(palette, 255 - fire[y][x], 0);
                setPixel(screen, x, y, c);
            }
		SDL_UnlockSurface(screen);
		if (fps_on)
			fps_draw();
		SDL_Flip(screen);
	}
	SDL_Quit();
	return 0;
}
