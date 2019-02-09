/***************************************************************************
 *   Copyright (C) 2005 by Michal Turek - Woq                              *
 *   WOQ (zavinac) seznam.cz                                               *
 *                                                                         *
 *   Program vykresluje rotujici logo knihovny SDL, ktere je nahrano       *
 *   pomoci SDL_image                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>


/*
 * Symbolicke konstanty
 */

#define SDL_SUBSYSTEMS SDL_INIT_VIDEO

#define WIN_FLAGS SDL_OPENGL|SDL_RESIZABLE
#define WIN_WIDTH 640
#define WIN_HEIGHT 480
#define WIN_BPP 0

#define WIN_TITLE "SDL, OpenGL a SDL_image"

// Predni a zadni orezavaci rovina pro gluPerspective()
#define NEAR_PLANE 1.0
#define FAR_PLANE 10.0

// Je-li definovano, pouzije se fsaa (karta musi podporovat GL_ARB_multisample)
// Zaroven take definuje pocet vzorku
// #define FSAA 2

// Definovat v Linuxu (aktualizuje vnitrni (SDL) velikost okna)
// Nedefinovat v MS Windows (ztrata OpenGL kontextu)
#define CALL_SETVIDEOMODE_WHEN_RESIZING

// Trocha matematiky
#define PI 3.1415926535897932384626433832795
#define DEGTORAD(deg) ((PI*(deg))/180.0)
#define RADTODEG(rad) ((180.0*(rad))/PI)


/*
 * Funkcni prototypy
 */

GLuint CreateTexture(const char* filename);	// Vytvori texturu
GLuint CreateTexture(SDL_Surface *surface);
bool SwapSurfaceRows(SDL_Surface *surface);	// Prohodi radky v obrazku
bool InitGL();					// Nastavi vlastnosti OpenGL
void ResizeGL(int width, int height);		// Nastavi OpenGL perspektivu
bool Init();					// Hlavni inicializacni funkce
void CalculateFPS();				// Vypocet FPS
void Draw();					// Rendering sceny
void Update();					// Aktualizace sceny
void Destroy();					// Deinicializace
bool ProcessEvent();				// Osetruje udalosti
int  main(int argc, char *argv[]);		// Vstup do programu


/*
 * Globalni promenne
 */

SDL_Surface *g_screen;			// Surface okna
float g_rotz = 0.0f;			// Uhel rotace okolo osy z

GLuint g_texture = 0;			// ID textury

float g_fps = 1.0f;			// FPS
Uint32 g_last_time = 0;			// Cas minuleho prekresleni (pro FPS)


/*
 * Vytvori OpenGL texturu z obrazku ulozeneho na disku;
 * vrati nulu, pokud soubor neexistuje nebo pokud neco selze
 */

GLuint CreateTexture(const char* filename)
{
	GLuint texture;
	SDL_Surface *img;

	img = IMG_Load(filename);// Nazapomenout prilinkovat SDL_image

	if(img == NULL)
	{
		fprintf(stderr, "Unable to load '%s': %s\n",
				filename, SDL_GetError());
		return 0;
	}

	texture = CreateTexture(img);
	SDL_FreeSurface(img);

	return texture;
}


/*
 * Vytvori OpenGL texturu ze SDL_Surface, v pripade neuspechu vrati nulu
 */

GLuint CreateTexture(SDL_Surface *surface)
{
	GLuint texture;		// Vysledna textura
	Uint32 saved_flags;	// Pomocne pro ulozeni flagu
	Uint8  saved_alpha;
	SDL_Surface *tmp_img;	// Obrazek pro vytvoreni textury

	// Vytvori prazdny RGB surface
	tmp_img = SDL_CreateRGBSurface(
			SDL_SWSURFACE,		// Softwarovy surface
			surface->w, surface->h,	// Sirka, vyska
			32,			// Barevna hloubka
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
			0x000000FF,		// OpenGL RGBA maska
			0x0000FF00,
			0x00FF0000,
			0xFF000000
#else
			0xFF000000,
			0x00FF0000,
			0x0000FF00,
			0x000000FF
#endif
			);

	if(tmp_img == NULL)
		return 0;

	// Ulozi atributy alfa blendingu
	saved_flags = surface->flags & (SDL_SRCALPHA|SDL_RLEACCELOK);
	saved_alpha = surface->format->alpha;

	if((saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA)
		SDL_SetAlpha(surface, 0, 0);

	// Zkopiruje surface do obrazku pro texturu
	SDL_Rect area;
	area.x = 0;
	area.y = 0;
	area.w = surface->w;
	area.h = surface->h;
	SDL_BlitSurface(surface, &area, tmp_img, &area);

	// Obnovi atributy alfa blendingu
	if((saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA)
		SDL_SetAlpha(surface, saved_flags, saved_alpha);

	// Prohodi radky
	if(!SwapSurfaceRows(tmp_img))
	{
		SDL_FreeSurface(tmp_img);
		return 0;
	}

	// Vytvori texturu
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,
		     0,
		     GL_RGBA,
		     surface->w, surface->h,
		     0,
		     GL_RGBA,
		     GL_UNSIGNED_BYTE,
		     tmp_img->pixels);

	SDL_FreeSurface(tmp_img);
	return texture;
}


/*
 * Prohodi radky v obrazku (prvni za posledni, druhy za predposledni atd.),
 */

bool SwapSurfaceRows(SDL_Surface *surface)
{
	// Ukazatele na prohazovane radky
	Uint8 *hi_row, *lo_row, *tmp_row;

	// Alokace pameti pro pomocny radek
	tmp_row = (Uint8 *)malloc(surface->pitch);

	if(tmp_row == NULL)
	{
		fprintf(stderr, "Unable to allocate memory.\n");
		return false;
	}

	// Ukazatele na prvni a posledni radek
	hi_row = (Uint8 *)surface->pixels;
	lo_row = hi_row + (surface->h * surface->pitch) - surface->pitch;

	if(SDL_MUSTLOCK(surface))
	{
		if(SDL_LockSurface(surface) == -1)
		{
			fprintf(stderr, "Unable to lock surface.\n");
			free(tmp_row);
			return false;
		}
	}

	for(int i = 0; i < surface->h / 2; i++)
	{
		memcpy(tmp_row, hi_row, surface->pitch);
		memcpy(hi_row, lo_row, surface->pitch);
		memcpy(lo_row, tmp_row, surface->pitch);

		hi_row += surface->pitch;// Dalsi/predchozi radek
		lo_row -= surface->pitch;
	}

	if (SDL_MUSTLOCK(surface))
	{
		SDL_UnlockSurface(surface);
	}

	free(tmp_row);// Uvolni pomocnou pamet

	return true;
}


/*
 * Inicializuje OpenGL
 */

bool InitGL()
{
	glClearColor(1.0, 1.0, 1.0, 0.5);
	glClearDepth(1.0);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Nahraje obrazek a vytvori texturu
	if((g_texture = CreateTexture("./sdllogo.png")) == 0)
		return false;

	return true;
}


/*
 * Nastavuje perspektivu. Vola se, kdyz uzivatel roztahuje okno,
 * v parametrech nova velikost
 */

void ResizeGL(int width, int height)
{
	if(height == 0)// Deleni nulou
		height = 1;

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0, (GLdouble)width/(GLdouble)height,
			NEAR_PLANE, FAR_PLANE);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


/*
 * Hlavni inicializacni funkce
 */

bool Init()
{
	if(SDL_Init(SDL_SUBSYSTEMS) == -1)	// Inicializace SDL
	{
		fprintf(stderr, "Unable to initialize SDL: %s\n",
				SDL_GetError());
		return false;
	}

	// Atributy rendering kontextu
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);

	SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 0);

#ifdef FSAA
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, FSAA);
#endif

	// Vytvori okno s definovanymi vlastnostmi
	g_screen = SDL_SetVideoMode(WIN_WIDTH, WIN_HEIGHT, WIN_BPP, WIN_FLAGS);

	if(g_screen == NULL)
	{
		fprintf(stderr, "Unable to set %dx%d video: %s\n",
				WIN_WIDTH, WIN_HEIGHT, SDL_GetError());
		return false;
	}

	if(!InitGL())				// Inicializace OpenGL
		return false;

	ResizeGL(WIN_WIDTH, WIN_HEIGHT);	// Nastavi perspektivu
	SDL_WM_SetCaption(WIN_TITLE, NULL);	// Titulek okna
	g_last_time = SDL_GetTicks();		// Pro FPS

	return true;
}


/*
 * Vypocet poctu snimku za sekundu, pohyby budou nezavisle na rychlosti pocitace
 */

void CalculateFPS()
{
	// SDL_GetTicks() vraci cas v sekundach od inicializace SDL
	Uint32 miliseconds = SDL_GetTicks() - g_last_time;

	if(miliseconds == 0)// Proti deleni nulou
		miliseconds = 1;

	g_fps = 1.0f / (miliseconds / 1000.0f);
	g_last_time = SDL_GetTicks();
}


/*
 * Rendering sceny
 */

void Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glTranslatef(	1.35f * (float)(cos(DEGTORAD(g_rotz))),
			0.8f * (float)(sin(DEGTORAD(g_rotz))),
			-4.0f);

	glRotatef(g_rotz, 0.0f, 0.0f, 1.0f);

	glBindTexture(GL_TEXTURE_2D, g_texture);
	glColor4f(1.0f,1.0f,1.0f,0.5f);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,-1.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,-1.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
	glEnd();

	glFlush();
	SDL_GL_SwapBuffers();// Prohodi predni a zadni buffer
}


/*
 * Aktualizuje scenu
 */

void Update()
{
	g_rotz -= 90.0f / g_fps;
}


/*
 * Deinicializacni funkce
 */

void Destroy()
{
	if(glIsTexture(g_texture))
	{
		glDeleteTextures(1, &g_texture);
		g_texture = 0;
	}

	SDL_Quit();
}


/*
 * Osetruje udalosti (bude probrano nekdy v budoucnu)
 * V tomto pripade reaguje na roztahovani okna uzivatelem a ceka na klavesu ESC,
 * ktera ukonci program
 */

bool ProcessEvent()
{
	SDL_Event event;

	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			// Klavesnice
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						return false;
						break;

					default:
						break;
				}
				break;

			// Zmena velikosti okna
			case SDL_VIDEORESIZE:
#ifdef CALL_SETVIDEOMODE_WHEN_RESIZING
				g_screen = SDL_SetVideoMode(event.resize.w,
					event.resize.h, WIN_BPP, WIN_FLAGS);

				if(g_screen == NULL)
				{
					fprintf(stderr,
						"Unable to resize window: %s\n",
						SDL_GetError());
					return false;
				}
#endif
				ResizeGL(event.resize.w, event.resize.h);
				break;

			// Pozadavek na ukonceni
			case SDL_QUIT:
				return false;
				break;

			default:
				break;
		}
	}

	return true;
}


/*
 * Vstup do programu
 */

int main(int argc, char *argv[])
{
	printf(WIN_TITLE);
	printf("\nPress ESC key to quit.\n");

	if(!Init())				// Inicializace
	{
		Destroy();
		return 1;
	}

	bool done = false;			// Hlavni smycka programu
	while(!done)
	{
		done = !ProcessEvent();		// Osetri udalosti

		CalculateFPS();			// Vypocet FPS
		Update();			// Aktualizace sceny
		Draw();				// Rendering
	}

	Destroy();				// Deinicializace a konec
	return 0;
}
