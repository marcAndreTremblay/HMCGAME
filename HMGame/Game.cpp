#include "Game.h"
#include <Windows.h> //Only for debug 

//Math
internal real32 RadiansToDegrees32(real32 radians)
{
	return  radians * 180 / Pi32;
}


//Graphic
internal SDL_Texture* loadTexture(char *path, SDL_Renderer *&gRenderer)
{
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path);
	if (loadedSurface == NULL)
	{
		//printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			//printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	return newTexture;
}
internal void DrawSolidColorTile(SDL_Renderer *&gRenderer, uint8 r, uint8 g, uint8 b, int32 x, int32 y, int32 size)
{
	SDL_SetRenderDrawColor(gRenderer, r, g, b, 0xFF);
	SDL_Rect rect = { x, y, size, size };
	SDL_RenderFillRect(gRenderer, &rect);
}

//Physic
internal void ApplyForce(Game_Object * obj, Force  force)
{
	for (int i = 0; i < MAX_FORCES_BY_OBJECT; i++)
	{
		if (obj->Forces[i].Duration<= 0)
		{
			obj->Forces[i].Duration = force.Duration;
			obj->Forces[i].x = force.x;
			obj->Forces[i].y = force.y;
			return;
		}
	}
}
internal void DoPhysic(Game_Object * obj, real32 timeSinceLastUpdate)
{
	real32 timeElapseInSec = timeSinceLastUpdate*0.001f;
	Vector2f forceTotal = {};


	switch (obj->state)
	{
		case Obj_State_Moving_Down: ApplyForce(obj, { 0, -obj->AccelerationStrenght, timeElapseInSec }); break;
		case Obj_State_Moving_Up: ApplyForce(obj, { 0, obj->AccelerationStrenght, timeElapseInSec }); break;
		case Obj_State_Moving_Right:ApplyForce(obj, { obj->AccelerationStrenght, 0, timeElapseInSec }); break;
		case Obj_State_Moving_Left: ApplyForce(obj, { -obj->AccelerationStrenght, 0, timeElapseInSec }); break;
		default:
			break;
	}

	for (int i = 0; i <= MAX_FORCES_BY_OBJECT-1; i++)
	{
		if (obj->Forces[i].Duration > 0)
		{
			forceTotal.x += obj->Forces[i].x;
			forceTotal.y += obj->Forces[i].y;
			obj->Forces[i].Duration -= timeElapseInSec;
		}
		if (obj->Forces[i].Duration <= 0)
		{
			obj->Forces[i] = {};
		}
	}

	//Friction
	real32 frictionStrengh = {};
	if (obj->Velocity.x == 0 && obj->Velocity.y == 0)
	{
		 frictionStrengh = 9.81f*obj->Mass*obj->FrictionCoeff_S;
		 if (forceTotal.x > 0 && frictionStrengh > forceTotal.x)
		 {
			 forceTotal.x = 0;
		 }
		 if (forceTotal.x < 0 && -(frictionStrengh) > forceTotal.x)
		 {
			 forceTotal.x = 0;
		 }
		 if (forceTotal.y > 0 && frictionStrengh > forceTotal.y)
		 {
			 forceTotal.y = 0;
		 }
		 if (forceTotal.y < 0 && -(frictionStrengh) > forceTotal.y)
		 {
			 forceTotal.y = 0;
		 }
	}
	else
	{
		 frictionStrengh = 9.81f*obj->Mass*obj->FrictionCoeff_K;	

	}




	Vector2f acceleration = {};
		acceleration.x = forceTotal.x / obj->Mass;
		acceleration.y = forceTotal.y / obj->Mass;

		if (acceleration.x > obj->maxAcceleration)
		{
			acceleration.x = obj->maxAcceleration;
		}
		if (acceleration.x < -obj->maxAcceleration)
		{
			acceleration.x = -obj->maxAcceleration;
		}
		if (acceleration.y > obj->maxAcceleration)
		{
			acceleration.y = obj->maxAcceleration;
		}
		if (acceleration.y < -obj->maxAcceleration)
		{
			acceleration.y = -obj->maxAcceleration;
		}


	Vector2f deplacement = {};
		deplacement.x = obj->Velocity.x*(timeElapseInSec) + 0.5f*acceleration.x *powf((timeElapseInSec), 2);
		deplacement.y = obj->Velocity.y*(timeElapseInSec)+ 0.5f*acceleration.y*powf((timeElapseInSec), 2);
	
	//Calculate new velocity
	obj->Velocity.x = obj->Velocity.x + acceleration.x*(timeElapseInSec);
	obj->Velocity.y = obj->Velocity.y + acceleration.y*(timeElapseInSec);

	if (obj->Velocity.x > obj->maxVelovity)
	{
		obj->Velocity.x = obj->maxVelovity;
	}
	if (obj->Velocity.x < -(obj->maxVelovity))
	{
		obj->Velocity.x = -obj->maxVelovity;
	}
	if (obj->Velocity.y > obj->maxVelovity)
	{
		obj->Velocity.y = obj->maxVelovity;
	}
	if (obj->Velocity.y < -(obj->maxVelovity))
	{
		obj->Velocity.y = -obj->maxVelovity;
	}

	obj->possition.x += deplacement.x;
	obj->possition.y += deplacement.y;
}
internal bool CheckCollision(Game_Object *obj1, Game_Object *obj2)
{
	
}
//Game
internal void InitializeGame(Game_Memory *&gameMemory, SDL_Renderer *&gRenderer)
{
	SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_NONE);

	//Set up player 1
	gameMemory->Player1 = &gameMemory->ObjectList[0];

	//Texture loading
	gameMemory->Textures[0] = loadTexture(".\\Img\\test.png", gRenderer);
	gameMemory->LoadedTextureCpt++;

	//Camera
	gameMemory->camera.aspectRation = 32;
	gameMemory->camera.possition = { 0, 1 };
	gameMemory->camera.size = { 10, 8 };

	gameMemory->ObjectList[0] = { 1, Obj_State_Idle, Pi32 / 2, 0.2f, 0.3f, 25 , {}, {}, 2, 5, 250, { 123, 0, 0, 0 }, { 1.5f, 1.5f }, { 1, 1 } };
	gameMemory->ObjectList[1] = { 2, Obj_State_Idle, 3 * (Pi32 / 2), 0.3f, 0.5f, 2, {}, {}, 2, 2, 15, { 0, 123, 0, 0 }, { 2.5f, 2.5f }, { 1, 1 } };

	gameMemory->IsGameInitialized = true;
}
internal void HandleInput(Game_Controler *&controler, Game_Memory *&gameMemory)
{
	if (controler->LeftPress == true && controler->WasLeftPress == false)
	{
		gameMemory->Player1->Orientation = Pi32 / 2;
		gameMemory->Player1->state = Obj_State_Moving_Left;
	}
	if (controler->LeftPress == false && controler->WasLeftPress == true)
	{
		gameMemory->Player1->state = Obj_State_Idle;
	}

	if (controler->DownPress == true && controler->WasDownPress == false)
	{
		gameMemory->Player1->Orientation = 0;
		gameMemory->Player1->state = Obj_State_Moving_Down;
	}
	if (controler->DownPress == false && controler->WasDownPress == true)
	{
		gameMemory->Player1->state = Obj_State_Idle;
	}

	if (controler->RightPress == true && controler->WasRightPress == false)
	{
		gameMemory->Player1->Orientation = 3 * (Pi32 / 2);
		gameMemory->Player1->state = Obj_State_Moving_Right;
	}
	if (controler->RightPress == false && controler->WasRightPress == true)
	{
		gameMemory->Player1->state = Obj_State_Idle;
	}

	if (controler->UpPress == true && controler->WasUpPress == false)
	{
		gameMemory->Player1->Orientation = Pi32;
		gameMemory->Player1->state = Obj_State_Moving_Up;
	}
	if (controler->UpPress == false && controler->WasUpPress == true)
	{
		gameMemory->Player1->state = Obj_State_Idle;
	}
}
external void UpdateAndRenderGame(Game_Memory *&gameMemory, real32 timeSinceLastUpdate, Game_Controler *controler, SDL_Renderer *&gRenderer)
{
	if (gameMemory->IsGameInitialized == false)
	{
		InitializeGame(gameMemory, gRenderer);
	}
	else
	{
		//Handle input
		HandleInput(controler, gameMemory);


		//Calculate physic for all objects
		gameMemory->PEngineUpdateTimer += timeSinceLastUpdate;
		if (gameMemory->PEngineUpdateTimer >= (1000 / gameMemory->UpdatePerSec))
		{
			for (int i = 0; i <= 1; i++)
			{
				DoPhysic(&gameMemory->ObjectList[i], gameMemory->PEngineUpdateTimer);
			}
			gameMemory->PEngineUpdateTimer = 0;
		}
		//Todo(Marc): Collision detection
		

		//Todo(Marc): find a way to change the location of the tile map maybe in a text file?
		//Render all objects to the screen
		int32 xCpt = 0;
		int32 yCpt = 0;
		int32 tilesMap[9][11] = {
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,1 },
		{ 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1 },
		{ 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 }, };
		


		//Render tile
		for each (int32 var in tilesMap)
		{
			SDL_Rect dRect = { (int)(xCpt * TILESIZE), (int)(yCpt * TILESIZE), (int)(1 * TILESIZE), (int)(1 * TILESIZE) };
			if (var == 1)
			{
				SDL_SetRenderDrawColor(gRenderer, 105, 222, 240, 0);
			}	
			if (var == 0)
			{
				SDL_SetRenderDrawColor(gRenderer, 100, 100, 100, 0);
			}
			SDL_RenderFillRect(gRenderer, &dRect);
			
			xCpt++;
			if (xCpt > 10)
			{
				xCpt = 0;
				yCpt++;
			}
		}

		//Render obj
		SDL_Rect sRect = { 0, 0, 64, 64 };
		for (int i = 0; i <= 1; i++)
		{
			SDL_Rect dRect = { (int)(gameMemory->ObjectList[i].possition.x * 64), (int)(gameMemory->ObjectList[i].possition.y * 64), (int)(gameMemory->ObjectList[i].size.x * 64), (int)(gameMemory->ObjectList[i].size.y * 64) };
			SDL_SetRenderDrawColor(gRenderer, gameMemory->ObjectList[i].Color.r, gameMemory->ObjectList[i].Color.g, gameMemory->ObjectList[i].Color.b, gameMemory->ObjectList[i].Color.a);
			SDL_RenderFillRect(gRenderer, &dRect);

			//SDL_RenderCopyEx(gRenderer, gameMemory->Textures[0], &sRect, &dRect, RadiansToDegrees32(gameMemory->ObjectList[i].Orientation), NULL, SDL_FLIP_NONE);
		}
			
		
		
	}	
}
