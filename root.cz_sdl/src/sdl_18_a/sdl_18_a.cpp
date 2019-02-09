/***************************************************************************
 *   Copyright (C) 2005 by Michal Turek - Woq                              *
 *   WOQ (zavinac) seznam.cz                                               *
 *                                                                         *
 *   Program demonstruje prehravani vice zvuku najednou. Jeden bude        *
 *   hudbou ve smycce na pozadi a druhy bude spousten vzdy po stisku       *
 *   mezerniku. Pri nahravani funkci LoadSound() se zvuky konvertuji       *
 *   na stejny (hardwarovy) format.                                        *
 *                                                                         *
 *   btw, omlouvam se za ty zvuky, ale nic jineho jsem na disku nenasel :( *
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
#include <malloc.h>
#include <string.h>
#include <SDL.h>


/*
 * Symbolicke konstanty
 */

#define SDL_SUBSYSTEMS SDL_INIT_VIDEO|SDL_INIT_AUDIO
#define WIN_WIDTH 320
#define WIN_HEIGHT 240
#define WIN_BPP 0
#define WIN_TITLE "Konverze zvuku"

#define NUM_SOUNDS 2

// Pomocna struktura pro zvuky
struct SOUND
{
	Uint8 *data;	// Ukazatel na data
	Uint32 len;	// Delka dat
	Uint32 pos;	// Pozice pri prehravani

	// Kdyz len == pos, zvuk neni prehravan
};


/*
 * Funkcni prototypy
 */

void AudioCallback(void *unused, Uint8 *stream, int len);
bool Init();					// Inicializace
void Destroy();					// Deinicializace
bool ProcessEvent();				// Osetruje udalosti
int  main(int argc, char *argv[]);		// Vstup do programu

// Nahraje zvuk z filename, zkonvertuje ho na spec format a ulozi do sound
// Pamet na sound->data MUSI BYT po skonceni pouzivani uvolnena
bool LoadSound(const char *filename, const SDL_AudioSpec *spec, SOUND *sound);


/*
 * Globalni promenne
 */

Uint8 g_win_flags = SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_RESIZABLE;
SDL_Surface *g_screen;

// Index 0 - hudba (smycka)
// Index 1 - zvuk spousteny stiskem mezerniku
SOUND g_sounds[NUM_SOUNDS];


/*
 * Audio callback funkce, posila data do streamu
 */

void AudioCallback(void *unused, Uint8 *stream, int len)
{
	// Hudba (smycka), stejne jako minule

	// Ukazatel na cast, kde se ma zacit prehravat
	Uint8 *wave_ptr = g_sounds[0].data + g_sounds[0].pos;

	// Delka zvuku do konce
	int wave_left = g_sounds[0].len - g_sounds[0].pos;

	// Zbyvajici delka je mensi nez pozadovana
	// Cyklus, protoze cely zvuk muze byt kratsi
	while(wave_left <= len)
	{
		// Posle data na zvukovou kartu
		SDL_MixAudio(stream, wave_ptr, wave_left, SDL_MIX_MAXVOLUME);

		// Posune se o prave zapsana data
		stream += wave_left;
		len -= wave_left;

		// Od zacatku zvuku
		wave_ptr = g_sounds[0].data;
		wave_left = g_sounds[0].len;
		g_sounds[0].pos = 0;
	}

	// Je jistota, ze zbyvajici cast zvuku je delsi nez pozadovana
	SDL_MixAudio(stream, wave_ptr, len, SDL_MIX_MAXVOLUME);
	g_sounds[0].pos += len;


	// Ostatni zvuky
	for(int i = 1; i < NUM_SOUNDS; i++)
	{
		// Delka zbyvajicich dat
		Sint32 amount = (g_sounds[i].len - g_sounds[i].pos);

		// Test preteceni
		if(amount > len)
			amount = len;

		// Posle data do streamu
		SDL_MixAudio(stream, &g_sounds[i].data[g_sounds[i].pos],
				amount, SDL_MIX_MAXVOLUME);

		// Posune se o prehrany usek
		g_sounds[i].pos += amount;
	}
}


/*
 * Nahraje zvuk ze souboru filename (.WAV), zkonvertuje ho na spec format
 * a vysledek ulozi na adresu sound
 * Pamet na sound->data MUSI BYT pred zacatkem i po skonceni pouzivani uvolnena
 */

bool LoadSound(const char *filename, const SDL_AudioSpec *spec, SOUND *sound)
{
	SDL_AudioCVT cvt;	// Konverzni struktura
	SDL_AudioSpec wave;	// Format puvodniho zvuku
	Uint8 *data;		// Pomocny ukazatel na data
	Uint32 len;		// Jejich delka

	// Nahraje zvuk
	if(SDL_LoadWAV(filename, &wave, &data, &len) == NULL)
	{
		fprintf(stderr, "Unable to load %s: %s\n",
				filename, SDL_GetError());
		return false;
	}

	// Vytvori konverzni strukturu
	if(SDL_BuildAudioCVT(&cvt, wave.format, wave.channels, wave.freq,
			spec->format, spec->channels, spec->freq) == -1)
	{
		fprintf(stderr, "Unable to build audio CVT (%s): %s\n",
				filename, SDL_GetError());
		SDL_FreeWAV(data);
		return false;
	}

	// Alokace pameti pro zkonvertovana data
	if((cvt.buf = (Uint8 *)malloc(len * cvt.len_mult)) == NULL)
	{
		fprintf(stderr, "Unable to allocate memory.\n");
		SDL_FreeWAV(data);
		return false;
	}

	// Zkopiruje nahrana data
	memcpy(cvt.buf, data, len);
	cvt.len = len;

	// Puvodni data uz nejsou potreba
	SDL_FreeWAV(data);

	// Konverze na pozadovany format
	if(SDL_ConvertAudio(&cvt) == -1)
	{
		fprintf(stderr, "Unable to convert audio (%s): %s\n",
				filename, SDL_GetError());
		free(cvt.buf);
		return false;
	}

	// Preda vystup pres parametr
	if(sound->data != NULL)// Kdyby nahodou...
		free(sound->data);

	sound->data = cvt.buf;
	sound->len = (Uint32)(cvt.len * cvt.len_ratio);
	sound->pos = sound->len;// Implicitne pozastaveny

	return true;
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

	// Nastaveni audia
	SDL_AudioSpec desired, obtained;
	desired.freq = 22050;// 16-bit. stereo na 22 kHz
	desired.format = AUDIO_S16;
	desired.channels = 2;
	desired.samples = 512;// Vhodne pro hry
	desired.callback = AudioCallback;
	desired.userdata = NULL;

	// Otevre audio zarizeni
	if(SDL_OpenAudio(&desired, &obtained) == -1)
	{
		fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
		return false;
	}

	// Loading zvuku
	for(int i = 0; i < NUM_SOUNDS; i++)// Pro jistotu...
		g_sounds[i].data = NULL;

	SDL_LockAudio();
	if(!LoadSound("music.wav", &obtained, &g_sounds[0])
			|| !LoadSound("test.wav", &obtained, &g_sounds[1]))
	{
		return false;
	}
	SDL_UnlockAudio();

	// Zacne prehravat
	SDL_PauseAudio(0);

	return true;
}


/*
 * Deinicializacni funkce
 */

void Destroy()
{
	// Zavre audio a uvolni data wavu
	SDL_CloseAudio();

	// Uvolni data
	for(int i = 0; i < NUM_SOUNDS; i++)
	{
		if(g_sounds[i].data != NULL)
		{
			free(g_sounds[i].data);
			g_sounds[i].data = NULL;
		}
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
			// Spusti zvuk
			case SDLK_SPACE:
				SDL_LockAudio();
				if(g_sounds[1].pos >= g_sounds[1].len)
					g_sounds[1].pos = 0;
				SDL_UnlockAudio();
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
