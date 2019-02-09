/***************************************************************************
 *   Copyright (C) 2005 by Michal Turek - Woq                              *
 *   WOQ (zavinac) seznam.cz                                               *
 *                                                                         *
 *   Program vytvori OpenGL okno a vykresli do nej barevny trojuhelnik     *
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
#include <SDL.h>
#include <SDL_opengl.h>


/*
 * Symbolicke konstanty
 */

#define SDL_SUBSYSTEMS SDL_INIT_VIDEO

#define WIN_FLAGS SDL_OPENGL|SDL_RESIZABLE
#define WIN_WIDTH 640
#define WIN_HEIGHT 480
#define WIN_BPP 0

#define WIN_TITLE "SDL a OpenGL"

// Predni a zadni orezavaci rovina pro gluPerspective()
#define NEAR_PLANE 1.0
#define FAR_PLANE 10.0

// Je-li definovano, pouzije se fsaa (karta musi podporovat GL_ARB_multisample)
// Zaroven take definuje pocet vzorku
// #define FSAA 2

// Definovat v Linuxu (aktualizuje vnitrni (SDL) velikost okna)
// Nedefinovat v MS Windows (ztrata OpenGL kontextu)
#define CALL_SETVIDEOMODE_WHEN_RESIZING


/*
 * Funkcni prototypy
 */

bool InitGL();				// Nastavi vlastnosti OpenGL
void ResizeGL(int width, int height);	// Nastavi OpenGL perspektivu
bool Init();				// Hlavni inicializacni funkce
void Draw();				// Rendering sceny
void Destroy();				// Deinicializace
bool ProcessEvent();			// Osetruje udalosti
int  main(int argc, char *argv[]);	// Vstup do programu


/*
 * Globalni promenne
 */

SDL_Surface *g_screen;// Surface okna


/*
 * Inicializuje vlastnosti OpenGL
 */

bool InitGL()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	return true;
}


/*
 * Nastavuje perspektivu, vola se, kdyz uzivatel roztahuje okno
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
	if(SDL_Init(SDL_SUBSYSTEMS) == -1)// Inicializace SDL
	{
		fprintf(stderr, "Unable to initialize SDL: %s.\n",
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
		fprintf(stderr, "Unable to set %dx%d video: %s.\n",
				WIN_WIDTH, WIN_HEIGHT, SDL_GetError());
		return false;
	}

	if(!InitGL())// Inicializace OpenGL
		return false;

	ResizeGL(WIN_WIDTH, WIN_HEIGHT);// Nastavi perspektivu
	SDL_WM_SetCaption(WIN_TITLE, NULL);// Titulek okna

	return true;
}


/*
 * Rendering sceny
 */

void Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glBegin(GL_TRIANGLES);
		glColor3ub(255, 0, 0); glVertex3f(-1.0f,-0.5f,-3.0f);
		glColor3ub(0, 255, 0); glVertex3f( 1.0f,-0.5f,-3.0f);
		glColor3ub(0, 0, 255); glVertex3f( 0.0f, 0.5f,-3.0f);
	glEnd();

	glFlush();
	SDL_GL_SwapBuffers();// Prohodi predni a zadni buffer
}


/*
 * Deinicializacni funkce
 */

void Destroy()
{
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
						"Unable to resize window: %s.\n",
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

	if(!Init())// Inicializace
	{
		Destroy();
		return 1;
	}

	// Hlavni smycka programu
	bool done = false;
	while(!done)
	{
		done = !ProcessEvent();// Osetri udalosti

		Draw();// Rendering
	}

	// Deinicializace a konec
	Destroy();
	return 0;
}
