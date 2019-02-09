/***************************************************************************
 *   Copyright (C) 2005 by Michal Turek - Woq                              *
 *   WOQ (zavinac) seznam.cz                                               *
 *                                                                         *
 *   Program implementuje velice jednoduchy CD prehravac, ktery umoznuje   *
 *   prechody mezi stopami, posuny pri prehravani, pozastaveni a vysunuti  *
 *   mechaniky. Po spusteni programu s volbou -h nebo --help se zobrazi    *
 *   ovladani. Vsechny informace se vypisuji do konzole.                   *
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
#include <string.h>
#include <SDL.h>


/*
 * Symbolicke konstanty
 */

#define SDL_SUBSYSTEMS SDL_INIT_VIDEO|SDL_INIT_CDROM
#define WIN_WIDTH 320
#define WIN_HEIGHT 240
#define WIN_BPP 0
#define WIN_TITLE "Simple CD Player"

// O kolik sekund se posouva prehravani
#define TIME 5


/*
 * Funkcni prototypy
 */

void DrivesInfo();				// Informace o mechanikach
void TrackInfo(SDL_CD *cdrom, int track = -1);	// Informace o stopach na CD
void InsertCD(SDL_CD *cdrom);			// Ceka na vlozeni CD

bool Init();					// Inicializace
void Destroy();					// Deinicializace
bool ProcessEvent();				// Osetruje udalosti
int  main(int argc, char *argv[]);		// Vstup do programu


/*
 * Globalni promenne
 */

Uint8 g_win_flags = SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_RESIZABLE;
SDL_Surface *g_screen;				// Surface okna

SDL_CD *g_cdrom = NULL;				// Indetifikator CD mechaniky


/*
 * Vypise pocet CD mechanik v systemu a jejich jmena
 */

void DrivesInfo()
{
	putchar('\n');
	int num_drives = SDL_CDNumDrives();

	printf("Drives available: %d\n", num_drives);
	for(int i = 0; i < num_drives; i++)
		printf("\tDrive %d: %s\n", i, SDL_CDName(i));
}


/*
 * Vypise informace o stope, pokud track == -1, o vsech
 */

void TrackInfo(SDL_CD *cdrom, int track)
{
	int m, s, f;
	char* trtype;

	SDL_CDStatus(cdrom);

	if(track == -1)// Vsechny stopy
	{
		printf("\nDrive tracks: %d\n", cdrom->numtracks);

		for(int i = 0; i < cdrom->numtracks; i++)
		{
			FRAMES_TO_MSF(cdrom->track[i].length, &m, &s, &f);

			if(f > 0)
				s++;

			switch(cdrom->track[i].type)
			{
				case SDL_AUDIO_TRACK: trtype="audio"; break;
				case SDL_DATA_TRACK: trtype="data"; break;
				default: trtype="unknown"; break;
			}

			printf("\tTrack %d: %d:%2.2d / %d\t[%s track]\n",
					cdrom->track[i].id, m, s,
					cdrom->track[i].length, trtype);
		}

		putchar('\n');
	}
	else// Pouze dana stopa
	{
		if(track >= 0 && track < g_cdrom->numtracks)
		{
			FRAMES_TO_MSF(cdrom->track[track].length, &m, &s, &f);

			if(f > 0)
				s++;

			switch(cdrom->track[track].type)
			{
				case SDL_AUDIO_TRACK: trtype="audio"; break;
				case SDL_DATA_TRACK: trtype="data"; break;
				default: trtype="unknown"; break;
			}

			printf("Track %d: %d:%2.2d / %d\t[%s track]\n",
					cdrom->track[track].id, m, s,
					cdrom->track[track].length, trtype);
		}
		else
		{
			fprintf(stderr, "Track %d is not valid.\n", track);
		}
	}
}


/*
 * Otevre mechaniku a ceka na vlozeni CD
 */

void InsertCD(SDL_CD *cdrom)
{
	SDL_CDStop(cdrom);
	SDL_CDEject(cdrom);

	// Ceka na vlozeni CD, testy kazde pul sekundy
	while(!CD_INDRIVE(SDL_CDStatus(cdrom)))
	{
		SDL_Delay(500);

		// ESC bude stale dostupny...
		SDL_PumpEvents();
		if(SDL_GetKeyState(NULL)[SDLK_ESCAPE] == SDL_PRESSED)
			return;
	}

	TrackInfo(cdrom);
	SDL_CDPlayTracks(cdrom, 0, 0, 0, 0);
}


/*
 * Inicializacni funkce
 */

bool Init()
{
	// Inicializace SDL
	if(SDL_Init(SDL_SUBSYSTEMS) == -1)
	{
		fprintf(stderr, "Unable to initialize SDL: %s\n",
				SDL_GetError());
		return false;
	}

	// Nastaveni ikony
	SDL_Surface *icon = SDL_LoadBMP("./icon.bmp");
	if(icon != NULL)
	{
		SDL_WM_SetIcon(icon, NULL);
		SDL_FreeSurface(icon);
	}

	// Vytvori okno s definovanymi vlastnostmi
	g_screen = SDL_SetVideoMode(WIN_WIDTH, WIN_HEIGHT, WIN_BPP, g_win_flags);
	if(g_screen == NULL)
	{
		fprintf(stderr, "Unable to set %dx%d video: %s\n",
				WIN_WIDTH, WIN_HEIGHT, SDL_GetError());
		return false;
	}

	// Titulek okna
	SDL_WM_SetCaption(WIN_TITLE, NULL);

	// CD-ROM
	if(SDL_CDNumDrives() < 0)
	{
		fprintf(stderr, "There is no available CD-ROM drive\n");
		return false;
	}

	// Informace o mechanikach v systemu
	DrivesInfo();

	// Otevre prvni
	if((g_cdrom = SDL_CDOpen(0)) == NULL)
	{
		fprintf(stderr, "Unable to open CD-ROM: %s\n", SDL_GetError());
		return false;
	}

	// Prehraje cele CD
	if(CD_INDRIVE(SDL_CDStatus(g_cdrom)))
	{
		TrackInfo(g_cdrom);
		SDL_CDPlayTracks(g_cdrom, 0, 0, 0, 0);
	}
	else
	{
		printf("There is no disk in drive. Waiting...\n");
		InsertCD(g_cdrom);
	}

	return true;
}


/*
 * Deinicializacni funkce
 */

void Destroy()
{
	if(g_cdrom != NULL)
	{
		// Zastavi prehravani
		if(SDL_CDStatus(g_cdrom) == CD_PLAYING)
			SDL_CDStop(g_cdrom);

		// Uvolni strukturu
		SDL_CDClose(g_cdrom);
		g_cdrom = NULL;
	}

	SDL_Quit();
}


/*
 * Osetreni udalosti
 */

bool ProcessEvent()
{
	SDL_Event event;

	while(SDL_WaitEvent(&event))
	{
		switch(event.type)
		{
		case SDL_KEYDOWN:
			switch(event.key.keysym.sym)
			{
			// Skok na predchazejici stopu
			case SDLK_UP:
				if(!CD_INDRIVE(SDL_CDStatus(g_cdrom))
						|| g_cdrom->cur_track <= 0)
				{
					break;
				}

				TrackInfo(g_cdrom, g_cdrom->cur_track-1);
				SDL_CDPlayTracks(g_cdrom, g_cdrom->cur_track-1,
						0, 0, 0);
				break;

			// Skok na nasledujici stopu
			case SDLK_DOWN:
				if(!CD_INDRIVE(SDL_CDStatus(g_cdrom))
						|| g_cdrom->cur_track
							> g_cdrom->numtracks-2)
				{
					break;
				}

				TrackInfo(g_cdrom, g_cdrom->cur_track+1);
				SDL_CDPlayTracks(g_cdrom, g_cdrom->cur_track+1,
						0, 0, 0);
				break;

			// Posun ve stope doleva
			case SDLK_LEFT:
			{
				if(SDL_CDStatus(g_cdrom) != CD_PLAYING)
					break;

				int frame = g_cdrom->cur_frame - TIME * CD_FPS;

				if(frame > 0)
				{
					SDL_CDPlayTracks(g_cdrom,
						g_cdrom->cur_track,
						frame, 0, 0);
				}
			}
			break;

			// Posun ve stope doprava
			case SDLK_RIGHT:
			{
				if(SDL_CDStatus(g_cdrom) != CD_PLAYING)
					break;

				int frame = g_cdrom->cur_frame + TIME * CD_FPS;

				if(frame < (int)g_cdrom->track[g_cdrom->cur_track].length)
				{
					SDL_CDPlayTracks(g_cdrom,
						g_cdrom->cur_track,
						frame, 0, 0);
				}
			}
			break;

			// Vymena CD
			case SDLK_BACKSPACE:
				InsertCD(g_cdrom);
				break;

			// Informace o stope
			case SDLK_RETURN:
				if(SDL_CDStatus(g_cdrom) == CD_PLAYING)
					TrackInfo(g_cdrom, g_cdrom->cur_track);
				break;

			// Pozastaveni
			case SDLK_SPACE:
				if(SDL_CDStatus(g_cdrom) == CD_PLAYING)
					SDL_CDPause(g_cdrom);
				else
					SDL_CDResume(g_cdrom);
				break;

			case SDLK_ESCAPE:
				return false;
				break;

			default:
				break;
			}
			break;

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

	if(argc > 1)
	{
		if(strcmp(argv[1], "-h") || strcmp(argv[1], "--help"))
		{
			putchar('\n');
			printf("Usage:\t%s [-h]\n\t%s [--help]\n",
					argv[0], argv[0]);

			printf("Keyboard:\n");
			printf("\t[ESC]\t\texit\n");
			printf("\t[DOWN]\t\tprevious track\n");
			printf("\t[UP]\t\tnext track\n");
			printf("\t[LEFT]\t\tmove %d seconds left\n", TIME);
			printf("\t[RIGHT]\t\tmove %d seconds right\n", TIME);
			printf("\t[ENTER]\t\tinfo about track\n");
			printf("\t[SPACE]\t\tpause/resume\n");
			printf("\t[BACKSPACE]\tnew CD\n");
			return 0;
		}
	}

	printf("\nPress ESC key to quit.\n");

	// Inicializace
	if(!Init())
	{
		Destroy();
		return 1;
	}

	// Hlavni smycka programu
	bool done = false;
	while(!done)
	{
		done = !ProcessEvent();
	}

	// Deinicializace a konec
	Destroy();
	return 0;
}
