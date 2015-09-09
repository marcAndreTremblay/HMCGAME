#if !defined(GAME_H)

#include <math.h>
#include <stdint.h>
#include <SDL.h>
#include <SDL_image.h>


#define internal static
#define external static
#define global_variable static


typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32 bool32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float real32;
typedef double real64;

#define Pi32 3.14159265359f

#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL)

global_variable int32 MAX_FORCES_BY_OBJECT = 5;

#define TILESIZE 64

struct Color32
{
	uint8 r;
	uint8 g;
	uint8 b;
	uint8 a;
};


struct Vector2f
{
	real32 x;
	real32 y;
};
struct Vector3f
{
	real32 x;
	real32 y;
	real32 z;
}; 

//
struct Force
{	
	real32 x;
	real32 y;
	real32 Duration;
};




//Game
enum Object_state
{
	Obj_State_Idle, 
	Obj_State_Moving_Left, 
	Obj_State_Moving_Right,
	Obj_State_Moving_Up,
	Obj_State_Moving_Down
};


struct Game_Object
{
	int32 Id;
	
	Object_state state;
	real32 Orientation; //Angle : radian

	real32 FrictionCoeff_K; //Note(Marc): kinetic
	real32 FrictionCoeff_S; //Note(Marc: static
	real32 Mass; //Note(Marc) : In Kg
	Force Forces[5];
	Vector2f Velocity; //m/s

	int32 maxVelovity;
	int32 maxAcceleration;
	int32 AccelerationStrenght; //Kilo newton

	Color32 Color;

	Vector2f possition;
	Vector2f size;
};

struct Game_Camera
{
	Vector2f possition;//World coordinates
	Vector2f size; //World coordinates

	int32 aspectRation; //Note(marc): must be 1/62 ou 1 / 32 Tile size
};

struct Game_Memory
{
	bool32 IsPlatformInitialized;
	bool32 IsGameInitialized;
	
	int ScreenWidth;
	int ScreenHeight;
	
	real32 UpdatePerSec;	//How many update per seconde the physic engine must have
	real32 PEngineUpdateTimer;	//Timer cpt for the physic engine

	Game_Camera camera;
	Game_Object ObjectList[2]; 
	Game_Object* Player1;

	int32 LoadedTextureCpt;
	SDL_Texture *Textures[10];
};



struct Game_Controler
{
	 bool LeftPress;
	 bool RightPress;
	 bool UpPress;
	 bool DownPress;

	 bool WasLeftPress;
	 bool WasRightPress;
	 bool WasUpPress;
	 bool WasDownPress;
};



#define GAME_H
#endif