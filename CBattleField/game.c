#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "gametime.h"
#include "color.h"

#ifdef WIN32
	#define CLEAR "cls"
	#include "clear_screen.h"

#else
	#define CLEAR "clear"

#endif


#define MAX  			50
#define TMAX 			3
#define MAX_MSG 		5

#define Grass_env 		0
#define RedTeam_env 	1
#define BlueTeam_env 	2
#define Bullet_env  	3
#define Black_env 		4

typedef enum NPC_T { Sold, Tank } NPC_T;
typedef enum Rotation { Left, Up, Down, Right } Rotation;
typedef enum Team { England, France } Team;

typedef enum Weap { Pistol, Rifle, Shotgun, Sniper, AntiVehicle } Weap;

typedef enum State { Attack, Defense, Idle } State;

typedef struct Entites Entites;
typedef struct Propt_s Propt_s;
typedef struct Aim_show Aim_show;

typedef struct Propt_t Propt_t;

struct Aim_show {
	int 	pos[ 2 ];
	bool	is_over;
	bool	isNULL;

};

struct Propt_t {
	float Tank_armor[ 4 ];
	float Tank_health;

	float Tank_speed;

	float Tank_Range;

	float Tank_damage;
	int   fire_time;

	Aim_show Aim;

};

struct Propt_s {
	float Base_Health;
	Weap Weapon_;

	int mags;

	int Damage;
	int Range;
	int Accuracy;


};



struct Entites {
	int   px;
	int   py;

	unsigned count;

	float goes_range;
	char  last;

	NPC_T Type;
	Rotation Rotation;
	Team Team_;

	State State_;

	union Propt {
		Propt_t Tank;
		Propt_s Soldier;

	} Propt;
	
	
};


void clear_s();
void update();
void RandomSpown();
void SpawnNPCs();
void NPC_moves();
void Add_Array( char *Arr );
void change_bar( int v );
void clear_shoot();
void Damage_Tank( Entites *Tank, Entites *Soldier );

bool is_out_map( int h, int w );
bool is_collision( Entites *NPC, int *P2 );

void Shoot();
void NPC_shoot( Entites *NPC );

void Intialize();

float Distance( Entites *A, Entites *B );

char maps[ 40 ][ 100 ];
float h_maps[ 40 ][ 100 ];

Propt_t Tanks_Prot[] = { { { 6., 4., 4., 2.5 }, 25, 0.75, 35, 6, 0, { { 0, 0 }, false, true } }, { { 3., 2., 2.1, 1. }, 20, 2, 35, 3, 0, { { 0, 0 }, false, true } } };

char TankUD[ 2 ][ 5 ][ 5 ] = {
	{
		{ "__|_\0" },
		{ "[ |]\0" },
		{ "| O|\0" },
		{ "|==|\0" },
		{ "####\0" }
	},
	{
		{ "####\0" },
		{ "[==]\0" },
		{ "| O|\0" },
		{ "[ |]\0" },
		{ "--|-\0" }
	}

};

char TankLR[ 2 ][ 4 ][ 8 ] = {
	{
		{ "#__--_?\0" },
		{ "#|   |?\0" },
		{ "#|O----\0" },
		{ "#-----?\0" },
	},

	{
		{ "?_--__#\0" },
		{ "?|   |#\0" },
		{ "----O|#\0" },
		{ "?-----#\0" },
	},

};

char TankLR_Destroy[ 2 ][ 5 ][ 8 ] = {
	{
		{ "#?___??\0" },
		{ "##|   *\0" },
		{ "?#|  /*\0" },
		{ "?#\?\?/--\0" },
		{ "\?\?-O???\0" },
	},
	
	{
		{ "\?\?/-__#\0" },
		{ "?/   |#\0" },
		{ "?|  O|?\0" },
		{ "?--/--#\0" },
		{ "<=/===>\0" },
	},

};

char TankUD_Destroy[ 2 ][ 5 ][ 6 ] = {
	{
		{ "|___/\0" },
		{ "|[ /]\0" },
		{ "|| O*\0" },
		{ "||***\0" },
		{ "|#??#\0" }
	},
	{
		{ "__##?\0" },
		{ "[====\0" },
		{ "|  |?\0" },
		{ "[ ]??\0" },
		{ "++=/?\0" }
	}

};

const int maps_w = 40;
const int maps_h = 100;  


int random_seed = 0; 
float act = 0.;

char gt;
char ch;

char bar[ 6 ];

unsigned global_random_seed;

char Msg[ MAX_MSG ][ 60 ];
int len_msg;
int turns;

float apsilon = 1.;
float dk, db; 

/*
			  $
			 $
			$
			O



*/


Entites Player;
Entites Signs;

Entites NPCs[ MAX ];
NPC_T NPC_types[ MAX ];

int Tank_List[ TMAX ] = { 3, 5, 7 };
int Flag_List[ 2 ][ 2 ] = { { 5, 25 }, { 35, 75 } };


char aim[ 2 ][ 3 ][ 5 ] = { { "-----", "| + |", "-----" }, { "..*..", ".***.", "..*.." }};

unsigned short NPC_want_to[ MAX ];

void clear_shoot() {
	for ( int i = 0; i < maps_w; i++ ) {
		for ( int j = 0; j < maps_h; j++ ) {
			if ( maps[ i ][ j ] == '$' ) {
				maps[ i ][ j ] = ".`"[ rand() % 2 ];
			}
		}
	}
}

inline bool is_out_map( int h, int w ) {
	return h < 0 || h > maps_h - 1 || w < 0 || w >= maps_w - 1;
}

inline float Distance( Entites *A, Entites *B ) {
	if ( A != NULL && B != NULL ) {
		return sqrt( pow( A->px - B->px, 2 ) + pow( A->py - B->py, 2 ) );
	}

	return 0.;
}

bool is_collision( Entites *NPC, int *P2 ) {
	float ly = NPC -> py;
	float lx = NPC -> px;

	float accuracy = 0.75;

	for ( int i = 0; i < MAX; i++ ) {
		if ( Distance( &NPCs[ i ], NPC ) > NPC -> Propt.Tank.Tank_Range ) {
			continue;
		}

		//printf( "T%d, [%d,%d] [%d, %d], %f, %f\n", NPC -> Type, P2[ 0 ], P2[ 1 ],  NPC -> px, NPC-> py,  dk, db );

		if ( P2[ 0 ] > NPC -> px ) {
			//puts( "P2[ 0 ] > NPC -> px" );
			//if ( P2[ 1 ] == NPC -> px )
			for ( ; lx < ( float )P2[ 0 ] && lx < maps_h - 1 && lx > -1 && ly < maps_w - 1 && ly > -1; lx += accuracy ) {
				if ( !( ( lx < maps_h - 1 && lx > -1 ) || ( ly < maps_w - 1 && ly > -1 ) ) )
					break;

				ly = dk * lx + db;
				//printf( "[%f, %f]\n", lx, ly );
				maps[ ( int )ly ][ ( int )lx ] = '$';
				
			}

			//getch();

			if ( fabs( dk * NPC -> px + db - NPC -> py ) < apsilon && NPCs[ i ].px > NPC -> px ) {
				return true;
			}

			return false;

		}
		else if ( P2[ 0 ] < NPC -> px ) {
			//puts( "P2[ 0 ] > NPC -> px2" );
			for ( ; lx > ( float )P2[ 0 ] && lx < maps_h - 1 && lx > -1 && ly < maps_w - 1 && ly > -1; lx -= accuracy ) {
				if ( !( ( lx < maps_h - 1 && lx > -1 ) || ( ly < maps_w - 1 && ly > -1 ) ) )
					break;
					
				ly = dk * lx + db;
				maps[ ( int )ly ][ ( int )lx ] = '$';
				//printf( "[%f, %f]\n", lx, ly );
			}

			if ( fabs( dk * NPC -> px + db - NPC -> py ) < apsilon && NPCs[ i ].px < NPC -> px ) {
				return true;
			}

			//getch();

			return false;
		}

		else if ( P2[ 0 ] == NPC -> px ) {
			
			if ( P2[ 1 ] > NPC -> py ) {
				//puts( "P2[ 0 ] > NPC -> px3" );
				for ( ; ly < ( float )P2[ 0 ] && lx < maps_h - 1 && lx > -1 && ly < maps_w - 1 || ly > -1; ly += accuracy ) {
					//ly = dk * lx + db;
					if ( !( ( lx < maps_h - 1 && lx > -1 ) || ( ly < maps_w - 1 && ly > -1 ) ) )
						break;

					maps[ ( int )ly ][ ( int )lx ] = '$';
					//printf( "[%f, %f]\n", lx, ly );
				}

				//getch();

				if ( NPCs[ i ].px == P2[ 0 ] && NPCs[ i ].py > NPC -> py && NPCs[ i ].py < P2[ 1 ] ) {
					return true;
				}
				return false;
			}

			if ( P2[ 1 ] < NPC -> py ) {
				//puts( "P2[ 0 ] > NPC -> px4" );
				for ( ; ly > ( float )P2[ 0 ] && lx < maps_h - 1 && lx > -1 && ly < maps_w - 1 && ly > -1; ly -= accuracy ) {
					//ly = dk * lx + db;

					if ( !( ( lx < maps_h - 1 && lx > -1 ) || ( ly < maps_w - 1 && ly > -1 ) ) )
						break;

					maps[ ( int )ly ][ ( int )lx ] = '$';
					//printf( "[%f, %f]\n", lx, ly );
				}

				//getch();

				if ( NPCs[ i ].px == P2[ 0 ] && NPCs[ i ].py < NPC -> py && NPCs[ i ].py > P2[ 1 ] ) {
					return true;
				}
				return false;
			}
		}


	}

	return false;

}


void Add_Array( char *Arr ) {
	memset( Msg[ 0 ], 0, 60);

	for ( int i = 0; i < MAX_MSG; i++ ) {
		memcpy( Msg[ i ], Msg[ i + 1 ], 60 );
		memset( Msg[ i + 1 ], 0, 60 );
	}

	memcpy( Msg[ MAX_MSG - 1 ], Arr, 60 );


}

void change_bar( int v ) {
	for ( int i = 0; i < 6; i++ ) {
		if ( i < v ) {
			bar[ i ] = '#';
		}
		else {
			bar[ i ] = ' ';
		}
	}

}


void Intailize() {
	unsigned a = ( unsigned )time( NULL );
	//char *RandomColor[ 2 ] = {"\033[47;32m.\033[0m\0", "\033[47;32m`\033[0m\0"};

	srand( a );

	for ( int i = 0; i < MAX; i++ ) {
		NPCs[ i ].Type = Sold;
		NPCs[ i ].Propt.Soldier.Base_Health = 6;

		NPCs[ i ].count = 0;

		for ( int j = 0; j < TMAX; j++ ) {
			if ( i == Tank_List[ j ] ) {
				NPCs[ i ].Type = Tank;
				NPCs[ i ].Propt.Tank = Tanks_Prot[ 0 ];

				break;
			}
		}

	}

	for ( int i = 0; i < maps_w; i++ ) {
		for ( int j = 0; j < maps_h - 1; j++ ) {
			a = ( unsigned )time( NULL );

			srand( a / ( i + 1 ) * j + rand() % 5260 );

			maps[ i ][ j ] =  ".`"[ rand() % 2 ];
			
			h_maps[ i ][ j ] = Grass_env;
		}

		maps[ i ][ maps_h - 1 ] = '\0';
		h_maps[ i ][ maps_h - 1 ] = 0.;

	};

	maps[ Flag_List[ 0 ][ 0 ] ][ Flag_List[ 0 ][ 1 ] ] = 'P';
	maps[ Flag_List[ 1 ][ 0 ] ][ Flag_List[ 1 ][ 1 ] ] = 'P';

	h_maps[ Flag_List[ 0 ][ 0 ] ][ Flag_List[ 0 ][ 1 ] ] = RedTeam_env;
	h_maps[ Flag_List[ 1 ][ 0 ] ][ Flag_List[ 1 ][ 1 ] ] = BlueTeam_env;


};


void NPC_shoot( Entites *NPC ) {
	unsigned a = ( unsigned )time( NULL );

	float b = 0.;
	char am[ 60 ];
	

	int pos[ 2 ] = { 0, 0 };

	Entites *Pd_Enemy = NULL;

	srand( a );

	//if ( rand() % 2 ) {
	//	return ;
	//}

	for ( int i = 0; i < MAX; i++ ) {
		srand( a + i );
		if ( &NPCs[ rand() % MAX ] != NPC && NPCs[ rand() % MAX ].Team_!= NPC -> Team_ && \
		NPCs[ rand() % MAX ].Propt.Soldier.Base_Health > 0 ) {
			
			Pd_Enemy = &NPCs[ rand() % MAX ];
			break;

		}
	}

	if ( Pd_Enemy == NULL ) {
		if ( rand() % 2 && NPC -> Team_ != Player.Team_ && NPC -> Propt.Soldier.Base_Health > 0 ) {
			//Pd_Enemy = &Player;
		}

		else {
			//printf( "NPC [%d, %d] T(%d) deside to not attack\n", NPC -> px, NPC -> py, NPC -> Team_ );
			return ;
		}

	}

	for ( int i = 0; i < MAX; i++ ) {
		if ( NPCs[ i ].Team_ != NPC -> Team_ && &NPCs[ i ] != NPC && &NPCs[ i ] != Pd_Enemy && Distance( &NPCs[ i ], NPC ) < Distance( Pd_Enemy, NPC ) && \
		NPCs[ rand() % MAX ].Propt.Soldier.Base_Health > 0 ) {
			
			Pd_Enemy = &NPCs[ i ];
		}

		if ( NPCs[ i ].Team_ != Player.Team_ && Distance( &Player, NPC ) < Distance( Pd_Enemy, NPC ) ) {
			Pd_Enemy = &Player;
		}
	}

	if ( NPC -> Type == Sold ) {
		if ( NPC -> Team_ == Pd_Enemy -> Team_ || ( ( int )Distance( Pd_Enemy, NPC ) > NPC -> Propt.Soldier.Range ) || ( Pd_Enemy -> Type == Tank && NPC -> Propt.Soldier.Weapon_ != AntiVehicle ) ) {
			return ;
		}
	}

	else if ( NPC -> Type == Tank ) {
		if ( NPC -> Team_ == Pd_Enemy -> Team_ || ( int )Distance( Pd_Enemy, NPC ) > NPC -> Propt.Tank.Tank_Range ) {
			return ;
		}
	}

	srand( a + rand() % 1000 );



	if ( NPC -> Type == Sold ) {

		bool is_tank = false;

		dk = ( pos[ 0 ] != NPC -> px ) ? ( pos[ 1 ] - NPC -> py ) / ( pos[ 0 ] - NPC -> px ) : -1;
		db = ( pos[ 0 ] != NPC -> px ) ? pos[ 1 ] - dk * pos[ 0 ] : -1;

		if ( true /*!( is_collision( NPC, ( int[] ){ Pd_Enemy -> px, Pd_Enemy -> py } ) )/*/ )  {

			if ( NPC -> Propt.Soldier.Weapon_ == AntiVehicle && rand() % 10 >= 3 ) {
				for ( int i1 = 0; i1 < MAX; i1++ ) {
					if ( NPCs[ i1 ].Team_ != NPC -> Team_ && Distance( &NPCs[ i1 ], NPC ) < NPC -> Propt.Soldier.Range && NPCs[ i1 ].Type == Tank && NPCs[ i1 ].Propt.Tank.Tank_health > 0 ) {
						Pd_Enemy = &NPCs[ i1 ];
						is_tank = true;
						//NPCs[ i1 ].Propt.Tank.Tank_health -= ( float )NPC -> Propt.Soldier.Damage / 3;

					}
				}
			}

			b = Distance( Pd_Enemy, NPC ) * NPC -> Propt.Soldier.Accuracy;
			for ( ; b > 1; ) {
				b /= 10.;
			}

			b = 1. - b;

			if ( !is_tank ) {

				Pd_Enemy -> Propt.Soldier.Base_Health -= (float)( NPC -> Propt.Soldier.Damage ) * b;

				if ( Pd_Enemy == &Player )
					sprintf( am, "NPC [%d,%d], T(%d) killed YOU [%d,%d], T(%d) by Weapon %d", NPC -> px, NPC -> py, NPC -> Team_, Pd_Enemy -> px, Pd_Enemy -> py, Pd_Enemy -> Team_, NPC -> Propt.Soldier.Weapon_ );
				
				else
					sprintf( am, "NPC [%d,%d], T(%d) killed NPC [%d,%d], T(%d) by Weapon %d", NPC -> px, NPC -> py, NPC -> Team_, Pd_Enemy -> px, Pd_Enemy -> py, Pd_Enemy -> Team_, NPC -> Propt.Soldier.Weapon_ );


				if ( Pd_Enemy -> Propt.Soldier.Base_Health <= 0 ) {
					if ( len_msg < MAX_MSG )
						memmove( Msg[ len_msg++ ], am, strlen( am ) );

					else
						Add_Array( am );

				}
			}
			else {
				Pd_Enemy -> Propt.Tank.Tank_health -= (float)( NPC -> Propt.Soldier.Damage ) * b / 3;
				
				sprintf( am, "NPC [%d,%d], T(%d) Destroy Tank [%d,%d], T(%d) by AntiTank", NPC -> px, NPC -> py, NPC -> Team_, Pd_Enemy -> px, Pd_Enemy -> py, Pd_Enemy -> Team_ );
				
				if ( Pd_Enemy -> Propt.Tank.Tank_health <= 0 ) {
					if ( len_msg < MAX_MSG )
						memmove( Msg[ len_msg++ ], am, strlen( am ) );

					else
						Add_Array( am );
				}

			}
		}

		return ;

	}

	if ( NPC -> Type == Tank ) {
		/*
		-----		 * 
		| + |	    ***
		-----        *
		*/

		if ( NPC -> Propt.Tank.fire_time < 1 ) {
			srand( a + rand() % 1000 + NPC -> px - Pd_Enemy -> py );

			pos[ 0 ] = Pd_Enemy -> px + ( -5 + rand() % 10 );
			pos[ 1 ] = Pd_Enemy -> py + ( -5 + rand() % 10 );
			dk = ( pos[ 0 ] != NPC -> px ) ? ( float )( pos[ 1 ] - NPC -> py ) / ( float )( pos[ 0 ] - NPC -> px ) : -1;
			db = ( pos[ 0 ] != NPC -> px ) ? pos[ 1 ] - dk * pos[ 0 ] : -1;

			printf( "fire_time=%d\n", NPC -> Propt.Tank.fire_time );

			if ( !( is_collision( NPC, pos ) ) ) {
				


				sprintf( am, "Tank at [%d,%d] T(%d) Shoot!", NPC -> px, NPC -> py, NPC -> Team_ );
				if ( len_msg < MAX_MSG )
					memmove( Msg[ len_msg++ ], am, strlen( am ) );

				else
					Add_Array( am );

				Aim_show Aim;
				Aim.pos[ 0 ] = pos[ 0 ];
				Aim.pos[ 1 ] = pos[ 1 ];

				Aim.is_over = false;

				NPC -> Propt.Tank.Aim = Aim;

				NPC -> Propt.Tank.fire_time++;
			}

			return ;
		}

		else if ( NPC -> Propt.Tank.fire_time == 30 ) {
			if ( !( NPC -> Propt.Tank.Aim.isNULL ) ) {
				NPC -> Propt.Tank.Aim.is_over = true;

				sprintf( am, "Tank at [%d,%d] T(%d) Exploded", NPC -> px, NPC -> py, NPC -> Team_ );
				if ( len_msg < MAX_MSG )
					memmove( Msg[ len_msg++ ], am, strlen( am ) );

				else
					Add_Array( am );
			}

		}

		else if ( NPC -> Propt.Tank.fire_time > 120 ) {
			NPC -> Propt.Tank.fire_time = 0;
		}

		else {
			NPC -> Propt.Tank.fire_time++;
		}

	} 

}

void Shoot() {
	Signs.px = Player.px;
	Signs.py = Player.py;		
	
	int i;
	int turns = 0;
	char am[ 60 ];
	unsigned a = 0;
	unsigned an = 0;
	
	char message[ 5 ] = "ME\0";
	
	update();
	
	printf( "使用wsad移动 z键继续\n血量[%s]\n", message );
	printf( "距离:%g\n", act );
	
	while ( ( gt = getch() ) != 'z' ) {
		SpawnNPCs();
		clear_s();

		switch ( gt ) {
			case 'w':	( Signs.py - 1 < 0 		) ? Signs.py : Signs.py--;	break;
			case 's':	( Signs.py + 1 > maps_w ) ? Signs.py : Signs.py++;	break;
			case 'a':	( Signs.px - 1 < 0	    ) ? Signs.px : Signs.px--;	break; 
			case 'd': 	( Signs.px + 1 > maps_h ) ? Signs.px : Signs.px++;	break;
			
			default:				break;
			
		}
		
		
		switch ( maps[ Signs.py ][ Signs.px ] ) {
			case '.':	message[ 0 ] = 'N'; message[ 1 ] = 'L'; break;
			case '`':	message[ 0 ] = 'N'; message[ 1 ] = 'L'; break;
			case '@':	message[ 0 ] = 'M'; message[ 1 ] = 'E'; break; 
			case 'E':	message[ 0 ] = 'E'; message[ 1 ] = 'M'; break;
			case 'F':	message[ 0 ] = 'F'; message[ 1 ] = 'D'; break;
			case '&':	message[ 0 ] = 'C'; message[ 1 ] = 'R'; break;
				
			default:
				for ( int m = 0; m < MAX; m++ ) {
					if ( NPCs[ m ].Type == Tank ) {
						if ( abs( Signs.px - NPCs[ m ].px ) <= 2 && abs( Signs.py - NPCs[ m ].py ) <= 3) {
							message[ 0 ] = 'T'; message[ 1 ] = 'K';
							break;
						}
					}
				}

				break;
				
		};
		
		message[ 2 ] = '\0'; 
		
		maps[ Signs.py ][ Signs.px ] = '^';
		update();

		if ( turns++ >= Player.Propt.Soldier.Accuracy ) {
			NPC_moves();
			turns = 0;

		}
		
		
		act = ( float )sqrt( pow( Signs.px - Player.px , 2 ) + pow( Signs.py - Player.py, 2 ) );

		printf( "使用wsad移动 z键继续 g键射击\n血量[%s]\n", message );
		printf( "距离:%g\n", act );

		if ( act > Player.Propt.Soldier.Range ) {
			puts( "警告，距离过远，精度可能会下降" );
		}
		
		
	}
	
	if ( !( strcmp( message, "NL" ) ) ) {
		puts( "miss" );
	}
	
	else if ( !( strcmp( message, "EM" ) ) ) {
		

		for ( i = 0; i < MAX; i++ ) {
			if ( Signs.py == NPCs[ i ].py && Signs.px == NPCs[ i ].px ) {
				a = ( unsigned )time( NULL );

				an = rand() % ( unsigned )act;

				srand( a + Player.py - NPCs[ i ].px + ( ( unsigned )act ) * 2 - 13334 );

				if ( rand() % ( unsigned )act - 5 > abs( an / 2 + 14 ) ) {
					puts( "未命中" );

				}

				else {
					//NPCs[ i ].py = NPCs[ i ].px = 114514;
					NPCs[ i ].Propt.Soldier.Base_Health = -1.;

					puts( "击中敌人" );
					sprintf( am, "You, T(1) killed NPC [%d,%d], T(%d) by Weapon %d",NPCs[ i ].px, NPCs[ i ].py, NPCs[ i ].Team_, NPCs[ i ].Propt.Soldier.Weapon_ );
					if ( len_msg < MAX_MSG )
						memmove( Msg[ len_msg++ ], am, strlen( am ) );

					else
						Add_Array( am );

				}

			}
		}		
		
	}
	
	else if ( !( strcmp( message, "ME" ) ) ) {
		puts( "你自尽了" );
		exit( 0 );
	}

	else if ( !( strcmp( message, "TK" ) ) ) {
		puts( "未能击穿装甲" );
		ch = maps[ Signs.py ][ Signs.px ];

	}
	
	puts( "按下任意键返回" );
	
	act = 0.;

	getch();
	
	Signs.px = Player.px;
	Signs.py = Player.py;

	clear_s();
	SpawnNPCs();
}

void NPC_moves() {
	int i = 0;
	int Temp_tank = 0;

	Rotation TmpRotation;
	
	unsigned a = (unsigned)time(NULL);
	
	for ( i = 0; i < MAX - 1; i++ ) {
		srand( a + i );
		
		if ( NPCs[ i ].Propt.Soldier.Base_Health > 0 && NPCs[ i ].Type != Tank ) {
		
			switch ( rand() % 5 ) {
				case 0:	
					NPCs[ i ].goes_range += ( float )( rand() % 9 ) / 100. + ( float )( rand() % 6 / 10. );
					if ( NPCs[ i ].goes_range >= 1. ) {
						NPCs[ i ].goes_range -= 1.;
						maps[ NPCs[ i ].py ][ NPCs[ i ].px ] = ( !ch ) ? ".`"[ rand() % 2 ] : NPCs[ i ].last; ( !( NPCs[ i ].px + 1 > maps_h ) ) ? NPCs[ i ].px++ : NPCs[ i ].px;
						NPCs[ i ].last = maps[ NPCs[ i ].py ][ NPCs[ i ].px ];

					}

					break;
					
				case 1:	
					NPCs[ i ].goes_range += ( float )( rand() % 9 ) / 100. + ( float )( rand() % 6 / 10. );
					
					if ( NPCs[ i ].goes_range >= 1. ) {
						NPCs[ i ].goes_range -= 1.;
						maps[ NPCs[ i ].py ][ NPCs[ i ].px ] = ( !ch ) ? ".`"[ rand() % 2 ] : NPCs[ i ].last; ( !( NPCs[ i ].py - 1 < 0	   ) ) ? NPCs[ i ].py-- : NPCs[ i ].py;
						NPCs[ i ].last = maps[ NPCs[ i ].py ][ NPCs[ i ].px ];

					}	

					break;

				case 2:	
					NPCs[ i ].goes_range += ( float )( rand() % 9 ) / 100. + ( float )( rand() % 6 / 10. );
					
					if ( NPCs[ i ].goes_range >= 1. ) {
						NPCs[ i ].goes_range -= 1.;
						maps[ NPCs[ i ].py ][ NPCs[ i ].px ] = ( !ch ) ? ".`"[ rand() % 2 ] : NPCs[ i ].last; ( !( NPCs[ i ].px - 1 < 0 	   ) ) ? NPCs[ i ].px-- : NPCs[ i ].px;
						NPCs[ i ].last = maps[ NPCs[ i ].py ][ NPCs[ i ].px ];

					}
					
					break;

				case 3:	
					NPCs[ i ].goes_range += ( float )( rand() % 9 ) / 100. + ( float )( rand() % 6 / 10. );
					
					if ( NPCs[ i ].goes_range >= 1. ) {
						NPCs[ i ].goes_range -= 1.;
						maps[ NPCs[ i ].py ][ NPCs[ i ].px ] = ( !ch ) ? ".`"[ rand() % 2 ] : NPCs[ i ].last; ( !( NPCs[ i ].py + 1 > maps_w ) ) ? NPCs[ i ].py++ : NPCs[ i ].py;
						NPCs[ i ].last = maps[ NPCs[ i ].py ][ NPCs[ i ].px ];

					}

					break;

				case 4: break;
				
			} 
		}

		else if ( NPCs[ i ].Propt.Tank.Tank_health > 0 && NPCs[ i ].Type == Tank ) {
			//switch ( rand() % 5 ) {
			//	case 0:	
			NPCs[ i ].goes_range += ( float )( rand() % 9 ) / 100. + ( float )( rand() % 6 / 10. );
			if ( NPCs[ i ].goes_range >= 1. + NPCs[ i ].Propt.Tank.Tank_speed ) {
				NPCs[ i ].goes_range -= 1. + NPCs[ i ].Propt.Tank.Tank_speed;
				
				//if ( NPCs[ i ].Rotation  )

				TmpRotation = ( Rotation[] ){ Up, Down, Left, Right }[ rand() % 4 ];

				if ( NPCs[ i ].Rotation == Left || NPCs[ i ].Rotation == Right ) {
					Temp_tank = ( TmpRotation == Left ) ? 1 : 0;
					for ( int j = 0, k = -2; j < 4; j++, k++ ) {
						for ( int z = -2; z < 6; z++ ) {
							maps[ NPCs[ i ].py + k ][ NPCs[ i ].px + z ] = '.';
						}
						
					}
				}

				else if ( NPCs[ i ].Rotation == Up || NPCs[ i ].Rotation == Down ) {
					Temp_tank = ( TmpRotation == Up ) ? 0 : 1;
					for ( int j = 0, k = -2; j < 5; j++, k++ ) {
						for ( int z = -2; z < 3; z++ ) {
							maps[ NPCs[ i ].py + k ][ NPCs[ i ].px + z ] = '.';
						}
					}

				}


				if ( NPCs[ i ].Rotation == TmpRotation ) {
					if ( !( rand() % 2 ) ) {

						switch ( TmpRotation ) {
							case Up: 		( NPCs[ i ].py != 2 ) ? NPCs[ i ].py-- : NPCs[ i ].py; 			break;
							case Down:		( NPCs[ i ].py + 3 != maps_w ) ? NPCs[ i ].py++ : NPCs[ i ].py; break;
							case Left:		( NPCs[ i ].px != 4 ) ? NPCs[ i ].px-- : NPCs[ i ].px; 			break;
							case Right:		( NPCs[ i ].px + 5 != maps_h ) ? NPCs[ i ].px++ : NPCs[ i ].px; break;
						}
					}

					//NPCs[ i ].py -= 1;

					if ( TmpRotation == Left || TmpRotation == Right ) {
						for ( int j = 0, k = -2; j < 4; j++, k++ ) {
							for ( int z = -2; z < 6; z++ ) {
								maps[ NPCs[ i ].py + k ][ NPCs[ i ].px + z ] = ( TankLR[ Temp_tank ][ 2 + k ][ 2 + z ] == '\0' || TankLR[ Temp_tank ][ 2 + k ][ 2 + z ] == '?' ) ? '.' : TankLR[ Temp_tank ][ 2 + k ][ 2 + z ];
							}
						}
					}

					else if ( TmpRotation == Up || TmpRotation == Down ) {
						for ( int j = 0, k = -2; j < 5; j++, k++ ) {
							for ( int z = -2; z < 3; z++ ) {
								maps[ NPCs[ i ].py + k ][ NPCs[ i ].px + z ] = ( TankUD[ Temp_tank ][ 2 + k ][ 2 + z ] == '\0' ) ? '.' : TankUD[ Temp_tank ][ 2 + k ][ 2 + z ];
							}
						}
					}

					//break;
				}

				else {
					NPCs[ i ].Rotation = TmpRotation;

				}

					//maps[ NPCs[ i ].py ][ NPCs[ i ].px ] = '.'; ( !( NPCs[ i ].px + 1 > maps_h ) ) ? NPCs[ i ].px++ : NPCs[ i ].px;

			}

				//break;
					
					
				//case 4: break;
				
		} 

	}
}
	


void RandomSpawn() {
	unsigned a = (unsigned)time(NULL);
	srand( a );

	int i;
	
	for ( i = 0 ; i < MAX; i ++ ) {
	
		srand( a + i + rand() % 1000 );
		
		//NPCs[ i ].px = 5 + rand() % 60;
		

		NPCs[ i ].Rotation = Up;
		//NPCs[ i ].Type = Sold;
		NPCs[ i ].Team_ = ( Team[] ){ England, France }[ rand() % 2 ];

		NPCs[ i ].px = 3 + rand() % ( maps_h - 3 ); 

		//NPCs[ i ].px = Flag_List[ NPCs[ i ].Team_ ][ 1 ] + ( -5 + rand() % 10 );

		//NPCs[ i ].Propt.Soldier.Base_Health = 6;

		srand( a - i + ( unsigned )time( NULL ) % ( 1 + rand() % 1000 ) );
		if ( NPCs[ i ].Type == Sold )
			NPCs[ i ].Propt.Soldier.Weapon_ = ( Weap[] ){ Pistol, Rifle, Shotgun, Sniper, AntiVehicle }[ rand() % 5 ];

			switch ( NPCs[ i ].Propt.Soldier.Weapon_ ) {
				case Pistol:
					NPCs[ i ].Propt.Soldier.mags = 5;

					NPCs[ i ].Propt.Soldier.Damage = 1;
					NPCs[ i ].Propt.Soldier.Range = 15;
					NPCs[ i ].Propt.Soldier.Accuracy = 2;
					break;

				case Rifle:
					NPCs[ i ].Propt.Soldier.mags = 10;
					NPCs[ i ].Propt.Soldier.Damage = 2;
					NPCs[ i ].Propt.Soldier.Range = 25;
					NPCs[ i ].Propt.Soldier.Accuracy = 3;
					break;

				case Shotgun:
					NPCs[ i ].Propt.Soldier.mags = 5;
					NPCs[ i ].Propt.Soldier.Damage = 5;
					NPCs[ i ].Propt.Soldier.Range = 13;
					NPCs[ i ].Propt.Soldier.Accuracy = 1;
					break;

				case Sniper:
					NPCs[ i ].Propt.Soldier.mags = 1;
					NPCs[ i ].Propt.Soldier.Damage = 10;
					NPCs[ i ].Propt.Soldier.Range = 50;
					NPCs[ i ].Propt.Soldier.Accuracy = 5;
					break;

				case AntiVehicle:
					NPCs[ i ].Propt.Soldier.mags = 1;
					NPCs[ i ].Propt.Soldier.Damage = 20;
					NPCs[ i ].Propt.Soldier.Range = 30;
					NPCs[ i ].Propt.Soldier.Accuracy = 4;
					break;

				default:
					break;

			}

	}
	
	a = (unsigned)time(NULL) + ( rand() % 300 );
	
	for ( i = 0 ; i < MAX; i ++ ) {
	
		srand( a + i );
		
		NPCs[ i ].py = 3 + rand() % ( maps_w - 3 ); 
		//NPCs[ i ].py = Flag_List[ NPCs[ i ].Team_ ][ 0 ] + ( -5 + rand() % 10 );
		

	}
	
}


void SpawnNPCs() {
	int i;

	if ( global_random_seed != 0 )
		global_random_seed += ( unsigned )time( NULL ) - rand();

	else
		global_random_seed = ( unsigned )time( NULL );

	for ( i = 0; i < MAX; i++ ) {
		srand( global_random_seed + i - NPCs[ i ].px );

		if ( NPCs[ i ].Type == Sold ) {
			if ( NPCs[ i ].Propt.Soldier.Base_Health > 0 ) {
				maps[ NPCs[ i ].py ][ NPCs[ i ].px ] = ( NPCs[ i ].Team_ == England ) ? 'E' : 'F';

				if ( rand() % 7 > 3 && turns > 10 )
					NPC_shoot( &NPCs[ i ] );
			
			}

			else {
				maps[ NPCs[ i ].py ][ NPCs[ i ].px ] = '&';
			}
		}

		else if ( NPCs[ i ].Type == Tank ) {


			if ( NPCs[ i ].Propt.Tank.Tank_health > 0 ) {
				switch ( NPCs[ i ].Rotation ) {
					case Left:
						for ( int j = 0, k = -2; j < 4; j++, k++ ) {
							for ( int z = -2; z < 6; z++ ) {
								maps[ NPCs[ i ].py + k ][ NPCs[ i ].px + z ] = ( TankLR[ 1 ][ 2 + k ][ 2 + z ] == '?' || TankLR[ 1 ][ 2 + k ][ 2 + z ] == '\0' ) ? '.' : TankLR[ 1 ][ 2 + k ][ 2 + z ];
							}
							
						}

						break;

					case Up:
						for ( int j = 0, k = -2; j < 5; j++, k++ ) {
							for ( int z = -2; z < 3; z++ ) {
								maps[ NPCs[ i ].py + k ][ NPCs[ i ].px + z ] = ( TankUD[ 0 ][ 2 + k ][ 2 + z ] == '\0' ) ? '.' : TankUD[ 0 ][ 2 + k ][ 2 + z ];
							}
						}

						break;

					case Right:
						for ( int j = 0, k = -2; j < 4; j++, k++ ) {
							for ( int z = -2; z < 6; z++ ) {
								maps[ NPCs[ i ].py + k ][ NPCs[ i ].px + z ] = ( TankLR[ 0 ][ 2 + k ][ 2 + z ] == '?' || TankLR[ 0 ][ 2 + k ][ 2 + z ] == '\0' ) ? '.' : TankLR[ 0 ][ 2 + k ][ 2 + z ];
							}
						}

						break;	

					case Down:
						for ( int j = 0, k = -2; j < 5; j++, k++ ) {
							for ( int z = -2; z < 3; z++ ) {
								maps[ NPCs[ i ].py + k ][ NPCs[ i ].px + z ] = ( TankUD[ 1 ][ 2 + k ][ 2 + z ] == '\0' ) ? '.' : TankUD[ 1 ][ 2 + k ][ 2 + z ];
							}
						}

						break;
					

					default: break;

				}

				char tmp = '\0';
				char am[ 60 ];
				float b = 0.;

				if ( turns > 10 ) {
					NPC_shoot( &NPCs[ i ] );
				}

				if ( NPCs[ i ].Propt.Tank.Aim.isNULL != true ) {
					if ( !( NPCs[ i ].Propt.Tank.Aim.is_over ) ) {
						if ( !( NPCs[ i ].Propt.Tank.Aim.pos[ 1 ] > maps_h - 1 || NPCs[ i ].Propt.Tank.Aim.pos[ 1 ] < 0 || NPCs[ i ].Propt.Tank.Aim.pos[ 0 ] < 0 || NPCs[ i ].Propt.Tank.Aim.pos[ 0 ] > maps_w - 1 ) ) {
							maps[ NPCs[ i ].Propt.Tank.Aim.pos[ 1 ] ][ NPCs[ i ].Propt.Tank.Aim.pos[ 0 ] ] = '+';

						}
					}
					else if ( NPCs[ i ].Propt.Tank.Aim.is_over ) {
						if ( !( is_out_map( NPCs[ i ].Propt.Tank.Aim.pos[ 0 ], NPCs[ i ].Propt.Tank.Aim.pos[ 1 ] - 1 ) ) ) {
							maps[ NPCs[ i ].Propt.Tank.Aim.pos[ 1 ] - 1 ][ NPCs[ i ].Propt.Tank.Aim.pos[ 0 ] ] = '*';
							for ( int ep = 0; ep < MAX; ep++ ) {
								if ( NPCs[ ep ].px == NPCs[ i ].Propt.Tank.Aim.pos[ 0 ] && NPCs[ ep ].py == NPCs[ i ].Propt.Tank.Aim.pos[ 1 ] - 1 ) {
									b = Distance( &NPCs[ ep ], &NPCs[ i ] );
									for ( ; b > 1;  ) {
										b /= 10.;
									}

									b = 1. - b;
									if ( NPCs[ ep ].Type == Sold ) {
										NPCs[ ep ].Propt.Soldier.Base_Health -= ( float )( NPCs[ i ].Propt.Tank.Tank_damage * b );


										if ( &NPCs[ ep ] == &Player )
											sprintf( am, "Tank [%d,%d], T(%d) killed YOU [%d,%d], T(%d)", NPCs[ i ].px, NPCs[ i ].py, NPCs[ i ].Team_, NPCs[ ep ].px, NPCs[ ep ].py, NPCs[ ep ].Team_ );
										
										else
											sprintf( am, "Tank [%d,%d], T(%d) killed NPC [%d,%d], T(%d)", NPCs[ i ].px, NPCs[ i ].py, NPCs[ i ].Team_, NPCs[ ep ].px, NPCs[ ep ].py, NPCs[ ep ].Team_ );
										
										if ( NPCs[ ep ].Propt.Soldier.Base_Health <= 0 ) {
											if ( len_msg < MAX_MSG )
												memmove( Msg[ len_msg++ ], am, strlen( am ) );

											else
												Add_Array( am );
									
										}
									}
								}
							}
						}
						if ( !( is_out_map( NPCs[ i ].Propt.Tank.Aim.pos[ 0 ], NPCs[ i ].Propt.Tank.Aim.pos[ 1 ] ) ) ) {
							maps[ NPCs[ i ].Propt.Tank.Aim.pos[ 1 ] ][ NPCs[ i ].Propt.Tank.Aim.pos[ 0 ] ] = '*';
							for ( int ep = 0; ep < MAX; ep++ ) {
								if ( NPCs[ ep ].px == NPCs[ i ].Propt.Tank.Aim.pos[ 0 ] && NPCs[ ep ].py == NPCs[ i ].Propt.Tank.Aim.pos[ 1 ] ) {
									b = Distance( &NPCs[ ep ], &NPCs[ i ] );
									for ( ; b > 1;  ) {
										b /= 10.;
									}

									b = 1. - b;
									if ( NPCs[ ep ].Type == Sold ) {
										NPCs[ ep ].Propt.Soldier.Base_Health -= ( float )( NPCs[ i ].Propt.Tank.Tank_damage * b );


										if ( &NPCs[ ep ] == &Player )
											sprintf( am, "Tank [%d,%d], T(%d) killed YOU [%d,%d], T(%d)", NPCs[ i ].px, NPCs[ i ].py, NPCs[ i ].Team_, NPCs[ ep ].px, NPCs[ ep ].py, NPCs[ ep ].Team_ );
										
										else
											sprintf( am, "Tank [%d,%d], T(%d) killed NPC [%d,%d], T(%d)", NPCs[ i ].px, NPCs[ i ].py, NPCs[ i ].Team_, NPCs[ ep ].px, NPCs[ ep ].py, NPCs[ ep ].Team_ );
										
										if ( NPCs[ ep ].Propt.Soldier.Base_Health <= 0 ) {
											if ( len_msg < MAX_MSG )
												memmove( Msg[ len_msg++ ], am, strlen( am ) );

											else
												Add_Array( am );
									
										}
									}
								}
							}
						}

						if ( !( is_out_map( NPCs[ i ].Propt.Tank.Aim.pos[ 0 ], NPCs[ i ].Propt.Tank.Aim.pos[ 1 ] + 1 ) ) ) {
							maps[ NPCs[ i ].Propt.Tank.Aim.pos[ 1 ] + 1 ][ NPCs[ i ].Propt.Tank.Aim.pos[ 0 ] ] = '*';
							for ( int ep = 0; ep < MAX; ep++ ) {
								if ( NPCs[ ep ].px == NPCs[ i ].Propt.Tank.Aim.pos[ 0 ] && NPCs[ ep ].py == NPCs[ i ].Propt.Tank.Aim.pos[ 1 ] + 1 ) {
									b = Distance( &NPCs[ ep ], &NPCs[ i ] );
									for ( ; b > 1;  ) {
										b /= 10.;
									}

									b = 1. - b;
									if ( NPCs[ ep ].Type == Sold ) {
										NPCs[ ep ].Propt.Soldier.Base_Health -= ( float )( NPCs[ i ].Propt.Tank.Tank_damage * b );


										if ( &NPCs[ ep ] == &Player )
											sprintf( am, "Tank [%d,%d], T(%d) killed YOU [%d,%d], T(%d)", NPCs[ i ].px, NPCs[ i ].py, NPCs[ i ].Team_, NPCs[ ep ].px, NPCs[ ep ].py, NPCs[ ep ].Team_ );
										
										else
											sprintf( am, "Tank [%d,%d], T(%d) killed NPC [%d,%d], T(%d)", NPCs[ i ].px, NPCs[ i ].py, NPCs[ i ].Team_, NPCs[ ep ].px, NPCs[ ep ].py, NPCs[ ep ].Team_ );
										
										if ( NPCs[ ep ].Propt.Soldier.Base_Health <= 0 ) {
											if ( len_msg < MAX_MSG )
												memmove( Msg[ len_msg++ ], am, strlen( am ) );

											else
												Add_Array( am );
									
										}
									}	
								}
							}
						}

						if ( !( is_out_map( NPCs[ i ].Propt.Tank.Aim.pos[ 0 ] + 1, NPCs[ i ].Propt.Tank.Aim.pos[ 1 ] ) ) ) {
							maps[ NPCs[ i ].Propt.Tank.Aim.pos[ 1 ] ][ NPCs[ i ].Propt.Tank.Aim.pos[ 0 ] + 1 ] = '*';
							for ( int ep = 0; ep < MAX; ep++ ) {
								if ( NPCs[ ep ].px == NPCs[ i ].Propt.Tank.Aim.pos[ 0 ] + 1 && NPCs[ ep ].py == NPCs[ i ].Propt.Tank.Aim.pos[ 1 ] ) {
									b = Distance( &NPCs[ ep ], &NPCs[ i ] );
									for ( ; b > 1;  ) {
										b /= 10.;
									}

									b = 1. - b;
									if ( NPCs[ ep ].Type == Sold ) {
										NPCs[ ep ].Propt.Soldier.Base_Health -= ( float )( NPCs[ i ].Propt.Tank.Tank_damage * b );


										if ( &NPCs[ ep ] == &Player )
											sprintf( am, "Tank [%d,%d], T(%d) killed YOU [%d,%d], T(%d)", NPCs[ i ].px, NPCs[ i ].py, NPCs[ i ].Team_, NPCs[ ep ].px, NPCs[ ep ].py, NPCs[ ep ].Team_ );
										
										else
											sprintf( am, "Tank [%d,%d], T(%d) killed NPC [%d,%d], T(%d)", NPCs[ i ].px, NPCs[ i ].py, NPCs[ i ].Team_, NPCs[ ep ].px, NPCs[ ep ].py, NPCs[ ep ].Team_ );
										
										if ( NPCs[ ep ].Propt.Soldier.Base_Health <= 0 ) {
											if ( len_msg < MAX_MSG )
												memmove( Msg[ len_msg++ ], am, strlen( am ) );

											else
												Add_Array( am );
									
										}
									}
								}
							}
						}

						if ( !( is_out_map( NPCs[ i ].Propt.Tank.Aim.pos[ 0 ] - 1, NPCs[ i ].Propt.Tank.Aim.pos[ 1 ]  ) ) ) {
							maps[ NPCs[ i ].Propt.Tank.Aim.pos[ 1 ] ][ NPCs[ i ].Propt.Tank.Aim.pos[ 0 ] - 1 ] = '*';
							for ( int ep = 0; ep < MAX; ep++ ) {
								if ( NPCs[ ep ].px == NPCs[ i ].Propt.Tank.Aim.pos[ 0 ] - 1 && NPCs[ ep ].py == NPCs[ i ].Propt.Tank.Aim.pos[ 1 ] ) {
									b = Distance( &NPCs[ ep ], &NPCs[ i ] );
									for ( ; b > 1;  ) {
										b /= 10.;
									}

									b = 1. - b;
									if ( NPCs[ ep ].Type == Sold ) {
										NPCs[ ep ].Propt.Soldier.Base_Health -= ( float )( NPCs[ i ].Propt.Tank.Tank_damage * b );


										if ( &NPCs[ ep ] == &Player )
											sprintf( am, "Tank [%d,%d], T(%d) killed YOU [%d,%d], T(%d)", NPCs[ i ].px, NPCs[ i ].py, NPCs[ i ].Team_, NPCs[ ep ].px, NPCs[ ep ].py, NPCs[ ep ].Team_ );
										
										else
											sprintf( am, "Tank [%d,%d], T(%d) killed NPC [%d,%d], T(%d)", NPCs[ i ].px, NPCs[ i ].py, NPCs[ i ].Team_, NPCs[ ep ].px, NPCs[ ep ].py, NPCs[ ep ].Team_ );
										
										if ( NPCs[ ep ].Propt.Soldier.Base_Health <= 0 ) {
											if ( len_msg < MAX_MSG )
												memmove( Msg[ len_msg++ ], am, strlen( am ) );

											else
												Add_Array( am );
									
										}
									}
								}
							}
						}

						NPCs[ i ].Propt.Tank.Aim.isNULL = true;

						//update();

						//getch();

					}

					/*
					for ( int i1 = 1, y = 0; i1 > -2; i1--, y++ ) {
						for ( int j1 = -2, x = 0; j1 < 3; j1++, x++ ) {
							if ( NPCs[ i ].Propt.Tank.Aim -> pos[ 0 ] + i1 > maps_w - 1 || NPCs[ i ].Propt.Tank.Aim -> pos[ 1 ] + j1 > maps_h - 1 || NPCs[ i ].Propt.Tank.Aim -> pos[ 0 ] + i1 < 0 || NPCs[ i ].Propt.Tank.Aim -> pos[ 1 ] + j1 < 0 )
								continue;
							maps[ NPCs[ i ].Propt.Tank.Aim -> pos[ 0 ] + i1 ][ NPCs[ i ].Propt.Tank.Aim -> pos[ 1 ] + j1 ] = *( NPCs[ i ].Propt.Tank.Aim -> style[ y ][ x ] );
						}
					} 
					*/
					

				}

			}

			else {
				switch ( NPCs[ i ].Rotation ) {
					case Left:
						for ( int j = 0, k = -2; j < 5; j++, k++ ) {
							for ( int z = -2; z < 6; z++ ) {
								maps[ NPCs[ i ].py + k ][ NPCs[ i ].px + z ] = ( TankLR_Destroy[ 1 ][ 2 + k ][ 2 + z ] == '?' || TankLR_Destroy[ 1 ][ 2 + k ][ 2 + z ] == '\0' ) ? '.' : TankLR_Destroy[ 1 ][ 2 + k ][ 2 + z ];
							}
							
						}

						break;

					case Up:
						for ( int j = 0, k = -2; j < 5; j++, k++ ) {
							for ( int z = -2; z < 3; z++ ) {
								maps[ NPCs[ i ].py + k ][ NPCs[ i ].px + z ] = ( TankUD_Destroy[ 0 ][ 2 + k ][ 2 + z ] == '\0' ) ? '.' : TankUD_Destroy[ 0 ][ 2 + k ][ 2 + z ];
							}
						}

						break;

					case Right:
						for ( int j = 0, k = -2; j < 5; j++, k++ ) {
							for ( int z = -2; z < 6; z++ ) {
								maps[ NPCs[ i ].py + k ][ NPCs[ i ].px + z ] = ( TankLR_Destroy[ 0 ][ 2 + k ][ 2 + z ] == '?' || TankLR_Destroy[ 0 ][ 2 + k ][ 2 + z ] == '\0' ) ? '.' : TankLR_Destroy[ 0 ][ 2 + k ][ 2 + z ];
							}
						}

						break;	

					case Down:
						for ( int j = 0, k = -2; j < 5; j++, k++ ) {
							for ( int z = -2; z < 3; z++ ) {
								maps[ NPCs[ i ].py + k ][ NPCs[ i ].px + z ] = ( TankUD_Destroy[ 1 ][ 2 + k ][ 2 + z ] == '\0' ) ? '.' : TankUD_Destroy[ 1 ][ 2 + k ][ 2 + z ];
							}
						}

						break;
					

					default: break;

				}

			}
		}

	}

	maps[ Flag_List[ 0 ][ 0 ] ][ Flag_List[ 0 ][ 1 ] ] = 'P';
	maps[ Flag_List[ 1 ][ 0 ] ][ Flag_List[ 1 ][ 1 ] ] = 'P';
	
}


void update() {
	int i, j;
	
	maps[ Player.py ][ Player.px ] = '@';
	
	maps[ Flag_List[ 0 ][ 1 ] ][ Flag_List[ 0 ][ 0 ] ] = 'P';
	maps[ Flag_List[ 1 ][ 1 ] ][ Flag_List[ 1 ][ 0 ] ] = 'P';

	h_maps[ Flag_List[ 0 ][ 0 ] ][ Flag_List[ 0 ][ 1 ] ] = RedTeam_env;
	h_maps[ Flag_List[ 1 ][ 0 ] ][ Flag_List[ 1 ][ 1 ] ] = BlueTeam_env;

	for ( i = 0; i < maps_w; i++ ) {
		for ( j = 0; j < maps_h; j++ ) {
			//For Color:

			//switch ( maps[ i ][ j ] ) {
			//	case 1:
			//		printf( "\033[47;32m.\033[0m" ); break;
			//	case 2:
			//		printf( "\033[47;32m`\033[0m" ); break;

			//	default:
			/*
			switch ( ( int )h_maps[ i ][ j ] ) {
				case Grass_env:		printf( Grass "%c" EndColor, maps[ i ][ j ] ); 		break;
				case BlueTeam_env:	printf( BlueTeam "%c" EndColor, maps[ i ][ j ] ); 	break;
				case RedTeam_env:	printf( RedTeam "%c" EndColor, maps[ i ][ j ] ); 	break;
				case Bullet_env:	printf( Bullet "%c" EndColor, maps[ i ][ j ] ); 	break;
				case Black_env:		printf( Black "%c" EndColor, maps[ i ][ j ] ); 		break;

				default:			putchar( maps[ i ][ j ] );							break;

			};
			*/

			putchar( maps[ i ][ j ] );

			
			
		} 
		
		putchar( '\n' );
	}
	
	putchar( '\n' );
		
}

void clear_s() {
	#ifdef WIN32
		SetPos( 0, 0 );
	#else
		printf( "\033[2J" );
	#endif
	
	maps[ Player.py ][ Player.px ] = ".`"[ rand() % 2 ];
	maps[ Signs.py ][ Signs.px ] = ".`"[ rand() % 2 ];

	h_maps[ Player.py ][ Player.px ] = Grass_env;
	h_maps[ Signs.py ][ Signs.px ] = Grass_env;


	//if ( ch != '\0' ) {
	//	maps[ Signs.py ][ Signs.px ] = ch;
	//	ch = '\0';
	//}
	
}



main() {

	int team1 = 0, team2 = 0;

	Player.px = maps_h / 2;
	Player.py = maps_w / 2;
	Player.Team_ = France;
	Player.Type = Sold;
	Player.Propt.Soldier.Base_Health = 99999999.;
	Player.Propt.Soldier.Weapon_ = Rifle;
	Player.Propt.Soldier.Accuracy = 5;
	Player.Propt.Soldier.Damage = 3;
	Player.Propt.Soldier.Range = 25;

	Player.Propt.Soldier.mags = 5;

	/*
	for ( int i = 1, y = 0; i > -2; i--, y++ ) {
		for ( int j = -2, x = 0; j < 3; j++, x++ ) {
			putchar( aim[ 0 ][ y ][ x ] );
		}
		putchar( '\n' );
	}

	getch();
	*/


	//init

	Intailize();
	
	//init();
	
	RandomSpawn();
	SpawnNPCs();
	
	update();
	
	change_bar( Player.Propt.Soldier.Base_Health );
	
	printf( "Health [%s]\n", bar );


	while ( 1 ) {
		//for ( int i = 0; i < len_msg; i++ )
		//	memset( Msg[ i ], 0, sizeof( Msg[ i ] ) );

		//len_msg = 0;

		clear_shoot();

		team1 = team2 = 0;

		Delay( 50000 );

		if ( _kbhit() ) {
			gt = getch();
			if ( gt == 'q' ) {
				break;

			}
		}

		SpawnNPCs();
		clear_s();

		change_bar( ( int )Player.Propt.Soldier.Base_Health );

		turns++;
		
		switch ( gt ) {
			case 'w':	( Player.py - 1 < 0      ) ? Player.py : Player.py--; gt = '\0';	break;
			case 's':	( Player.py + 1 > maps_w ) ? Player.py : Player.py++; gt = '\0';	break;
			case 'a':	( Player.px - 1 < 0      ) ? Player.px : Player.px--; gt = '\0';	break; 
			case 'd': 	( Player.px + 1 > maps_h ) ? Player.px : Player.px++; gt = '\0';	break;
			
			case 'g':	Shoot(); gt = '\0';		break;
			
			default: gt = '\0';					break;
			
		}
		
		

		update();
		NPC_moves();

		for ( int i = 0; i < MAX; i++ ) {
			if ( NPCs[ i ].Team_ == France && NPCs[ i ].Propt.Soldier.Base_Health > 0 ) {
				team1++;
			}
			else if ( NPCs[ i ].Team_ == England && NPCs[ i ].Propt.Soldier.Base_Health > 0 ) {
				team2++;
			}
		}

		printf( "F:%d人, E:%d人\n", team1, team2 );

		printf( "Health [%s] %g\n", bar, Player.Propt.Soldier.Base_Health );

		for ( int i = 0; i < len_msg; i++ ) {
			printf( "[Message]:%s\n", Msg[ i ] );
		}

		putchar( '\n' );

		if ( Player.Propt.Soldier.Base_Health <= 0 ) {
			printf( "You are dead\n" );
			break;
		}

		
	}


	exit( 0 );
			  
	
}






