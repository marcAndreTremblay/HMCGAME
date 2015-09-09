#include "Game.h"
#include "Game.cpp"


global_variable bool globalRunning;
global_variable int64 GlobalPerfCountFrequency;



inline real32 Win32GetSecondsElapsed(LARGE_INTEGER Start, LARGE_INTEGER End)
{
	real32 Result = ((real32)(End.QuadPart - Start.QuadPart) /
		(real32)GlobalPerfCountFrequency);
	return(Result);
}

inline LARGE_INTEGER Win32GetWallClock(void)
{
	LARGE_INTEGER Result;
	QueryPerformanceCounter(&Result);
	return(Result);
}


internal SDL_Surface* LoadBMPFile(char *path)
{
	SDL_Surface* surface = SDL_LoadBMP(path);
	if (surface == NULL)
	{
		//("Unable to load image %s! SDL Error: %s\n", "", SDL_GetError());
	}
	return surface;
}

internal SDL_Surface* loadOptimizedBMPSurface(char *path, SDL_PixelFormat *PixelFormat)
{
	//The final optimized image
	SDL_Surface* optimizedSurface = NULL;
	
	//Load image at specified path
	SDL_Surface* loadedSurface = SDL_LoadBMP(path);
	if (loadedSurface == NULL)
	{
		//printf("Unable to load image %s! SDL Error: %s\n", path, SDL_GetError());
	}
	else
	{
		//Convert surface to screen format
		optimizedSurface = SDL_ConvertSurface(loadedSurface, PixelFormat, NULL);
		if (optimizedSurface == NULL)
		{
			//printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	return optimizedSurface;
}



internal bool InitgRenderer(SDL_Window *&Window, SDL_Renderer *&gRenderer)
{
	bool success = true;

	gRenderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);
	if (gRenderer != NULL)
	{
		//Initialize PNG loading
		int imgFlags = IMG_INIT_PNG;
		if (!(IMG_Init(imgFlags) & imgFlags))
		{
			//printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
			success = false;
		}
	}
	else
	{
		//printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		success = false;
	}

	return success;
}

internal bool InitWindow(SDL_Window *&Window, SDL_Surface *&ScreenSurface, Game_Memory *&gameMemory)
{
	bool success = true;

	if (SDL_Init(SDL_INIT_VIDEO) == 0)
	{
		//Create window
		Window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, gameMemory->ScreenWidth, gameMemory->ScreenHeight, SDL_WINDOW_SHOWN);
		if (Window != NULL)
		{
			ScreenSurface = SDL_GetWindowSurface(Window);
		}
		else
		{
			//printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
	}
	else
	{
		//printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}

	return success;
}

internal void UpdateControler(Game_Controler *controler, SDL_Event *e)
{
	controler->WasDownPress = controler->DownPress;
	controler->WasLeftPress = controler->LeftPress;
	controler->WasRightPress = controler->RightPress;
	controler->WasUpPress= controler->UpPress;



	const Uint8 *state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_A]) {
		controler->LeftPress = true;
	}
	else
	{
		controler->LeftPress = false;
	}
	if (state[SDL_SCANCODE_W]) {
		controler->UpPress = true;
	}
	else
	{
		controler->UpPress = false;
	}
	if (state[SDL_SCANCODE_S]) {
		controler->DownPress = true;
	}
	else
	{
		controler->DownPress = false;
	}
	if (state[SDL_SCANCODE_D]) {
		controler->RightPress = true;
	}
	else
	{
		controler->RightPress = false;
	}
}

int main(int argc, char* args[])
{


	SDL_Window* Window = NULL;
	SDL_Surface* ScreenSurface = NULL;
	SDL_Renderer* gRenderer = NULL;
	Game_Controler controler = {};
	SDL_Event e;
	Game_Memory* GameMemory;
	GameMemory = (Game_Memory*)VirtualAlloc(NULL, sizeof(Game_Memory),
													MEM_RESERVE | MEM_COMMIT,
													PAGE_READWRITE);

	GameMemory->UpdatePerSec = 30;
	GameMemory->ScreenHeight = 600;
	GameMemory->ScreenWidth = 800;
	

	GameMemory->IsPlatformInitialized = true;

	//Information for conter 
	LARGE_INTEGER PerfCountFrequencyResult;
	QueryPerformanceFrequency(&PerfCountFrequencyResult);
	GlobalPerfCountFrequency = PerfCountFrequencyResult.QuadPart;


	//Start up SDL and create window
	if (!InitWindow(Window,ScreenSurface,GameMemory))
	{
		//printf("Failed to initialize!\n");
	}
	else
	{
		InitgRenderer(Window, gRenderer);
		//SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_NONE);
		if (!gRenderer)
		{
			//printf("Failed to load media!\n");
		}
		else
		{
			globalRunning = true;
			
			uint64 LastCycleCount = __rdtsc();
			LARGE_INTEGER LastCounter = Win32GetWallClock();

			while (globalRunning)
			{
				//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					
					if (e.type == SDL_QUIT)
					{
						globalRunning = false;
					}

					if (e.type == SDL_KEYUP)
					{
						switch (e.key.keysym.sym)
						{
							case  SDLK_F10:{
								globalRunning = false;
								}break;
						} 

					}
				}
				

				LARGE_INTEGER EndCounter = Win32GetWallClock();
				real32 MSPerFrame = 1000.0f*Win32GetSecondsElapsed(LastCounter, EndCounter);
				LastCounter = EndCounter;
				
				UpdateControler(&controler, &e);
				UpdateAndRenderGame(GameMemory, MSPerFrame, &controler, gRenderer);
				
				SDL_RenderPresent(gRenderer);
				SDL_SetRenderDrawColor(gRenderer, 0x0F, 0x00, 0x0F, 0x00);
				SDL_RenderClear(gRenderer);			
			}
		}
	}



	//Free loaded image
	for (int i = 0; i <= GameMemory->LoadedTextureCpt;i++)
	{
		SDL_DestroyTexture(GameMemory->Textures[i]);
		GameMemory->Textures[i] = NULL;
	}
	
	

	//Destroy window    
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(Window);
	Window = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();


	VirtualFree(GameMemory, sizeof(Game_Memory), MEM_RELEASE);
	
	return 0;
}
				