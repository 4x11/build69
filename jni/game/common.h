#pragma once

#define PLAYER_PED_SLOTS	120

typedef unsigned short VEHICLEID;
typedef unsigned short PLAYERID;

#define PADDING(x,y) uint8_t x[y]

#define IN_VEHICLE(x) ((x->dwStateFlags & 0x100) >> 8)

//-----------------------------------------------------------

#pragma pack(1)
typedef struct _VECTOR 
{
	float X,Y,Z;
} VECTOR, *PVECTOR;

#pragma pack(1)
typedef struct _MATRIX4X4 
{
	VECTOR right;		// 0-12 	; r11 r12 r13
	uint32_t  flags;	// 12-16
	VECTOR up;			// 16-28	; r21 r22 r23
	float  pad_u;		// 28-32
	VECTOR at;			// 32-44	; r31 r32 r33
	float  pad_a;		// 44-48
	VECTOR pos;			// 48-60
	float  pad_p;		// 60-64
} MATRIX4X4, *PMATRIX4X4;

//-----------------------------------------------------------

#pragma pack(1)
typedef struct _ENTITY_TYPE
{
	// ENTITY STUFF
	uint32_t vtable; 		// 0-4		;vtable
	PADDING(_pad91, 16);	// 4-20
	MATRIX4X4 *mat; 		// 20-24	;mat
	PADDING(_pad92, 10);	// 24-34
	uint16_t nModelIndex; 	// 34-36	;ModelIndex
	PADDING(_pad93, 32);	// 36-68
	VECTOR vecMoveSpeed; 	// 68-80	;vecMoveSpeed
	VECTOR vecTurnSpeed; 	// 80-92	;vecTurnSpeed
	PADDING(_pad94, 88);	// 92-180
	uintptr_t dwUnkModelRel; // 180-184 ;сотка инфа

} ENTITY_TYPE;

//-----------------------------------------------------------

#pragma pack(1)
typedef struct _PED_TYPE
{
	ENTITY_TYPE entity; 		// 0000-0184	;entity
	PADDING(_pad106, 174);		// 0184-0358
	uint32_t _pad107;			// 0358-0362	;dwPedType
	PADDING(_pad101, 734);		// 0362-1096
	uint32_t dwAction;			// 1096-1100	;Action
	PADDING(_pad102, 52);		// 1100-1152
	uint32_t dwStateFlags; 		// 1152-1156	;StateFlags
	PADDING(_pad103, 188);		// 1156-1344
	float fHealth;		 		// 1344-1348	;Health
	float fMaxHealth;			// 1348-1352	;MaxHealth
	float fArmour;				// 1352-1356	;Armour
	PADDING(_pad104, 12);		// 1356-1368
	float fRotation1;			// 1368-1372	;Rotation1
	float fRotation2;			// 1372-1376	;Rotation2
	PADDING(_pad105, 44);		// 1376-1420
	uint32_t pVehicle;			// 1420-1424	;pVehicle
	PADDING(_pad108, 8);		// 1424-1432
	uint32_t dwPedType;			// 1432-1436	;dwPedType
	PADDING(_pad109, 456);		// 1436-1892
	uint32_t* pdwDamageEntity;	// 1892-1896	;pdwDamageEntity
} PED_TYPE;

//-----------------------------------------------------------

#pragma pack(1)
typedef struct _VEHICLE_TYPE
{
	ENTITY_TYPE entity;			// 0000-0184	;entity
	PADDING(_pad201, 892);		// 0184-1076
	uint8_t byteColor1;			// 1076-1077	;byteColor1
	uint8_t byteColor2;			// 1077-1078	;byteColor2
	PADDING(_pad204, 42);		// 1078-1120
	PED_TYPE *pDriver;			// 1120-1124	;driver
	PED_TYPE *pPassengers[7];	// 1124-1152	;pPassengers
	PADDING(_pad202, 72);		// 1152-1224
	float fHealth;				// 1224-1228	;fHealth
	PADDING(_pad203, 56);		// 1228-1284
	uint32_t dwDoorsLocked;		// 1284-1288	;dwDoorsLocked
} VEHICLE_TYPE;

//-----------------------------------------------------------

#define	VEHICLE_SUBTYPE_CAR				1
#define	VEHICLE_SUBTYPE_BIKE			2
#define	VEHICLE_SUBTYPE_HELI			3
#define	VEHICLE_SUBTYPE_BOAT			4
#define	VEHICLE_SUBTYPE_PLANE			5
#define	VEHICLE_SUBTYPE_PUSHBIKE		6
#define	VEHICLE_SUBTYPE_TRAIN			7

//-----------------------------------------------------------

#define TRAIN_PASSENGER_LOCO			538
#define TRAIN_FREIGHT_LOCO				537
#define TRAIN_PASSENGER					570
#define TRAIN_FREIGHT					569
#define TRAIN_TRAM						449
#define HYDRA							520

//-----------------------------------------------------------

#define ACTION_WASTED					55
#define ACTION_DEATH					54
#define ACTION_INCAR					50
#define ACTION_NORMAL					1
#define ACTION_SCOPE					12
#define ACTION_NONE						0 

//-----------------------------------------------------------

#define WEAPON_MODEL_BRASSKNUCKLE		331 // was 332
#define WEAPON_MODEL_GOLFCLUB			333
#define WEAPON_MODEL_NITESTICK			334
#define WEAPON_MODEL_KNIFE				335
#define WEAPON_MODEL_BAT				336
#define WEAPON_MODEL_SHOVEL				337
#define WEAPON_MODEL_POOLSTICK			338
#define WEAPON_MODEL_KATANA				339
#define WEAPON_MODEL_CHAINSAW			341
#define WEAPON_MODEL_DILDO				321
#define WEAPON_MODEL_DILDO2				322
#define WEAPON_MODEL_VIBRATOR			323
#define WEAPON_MODEL_VIBRATOR2			324
#define WEAPON_MODEL_FLOWER				325
#define WEAPON_MODEL_CANE				326
#define WEAPON_MODEL_GRENADE			342 // was 327
#define WEAPON_MODEL_TEARGAS			343 // was 328
#define WEAPON_MODEL_MOLOTOV			344 // was 329
#define WEAPON_MODEL_COLT45				346
#define WEAPON_MODEL_SILENCED			347
#define WEAPON_MODEL_DEAGLE				348
#define WEAPON_MODEL_SHOTGUN			349
#define WEAPON_MODEL_SAWEDOFF			350
#define WEAPON_MODEL_SHOTGSPA			351
#define WEAPON_MODEL_UZI				352
#define WEAPON_MODEL_MP5				353
#define WEAPON_MODEL_AK47				355
#define WEAPON_MODEL_M4					356
#define WEAPON_MODEL_TEC9				372
#define WEAPON_MODEL_RIFLE				357
#define WEAPON_MODEL_SNIPER				358
#define WEAPON_MODEL_ROCKETLAUNCHER		359
#define WEAPON_MODEL_HEATSEEKER			360
#define WEAPON_MODEL_FLAMETHROWER		361
#define WEAPON_MODEL_MINIGUN			362
#define WEAPON_MODEL_SATCHEL			363
#define WEAPON_MODEL_BOMB				364
#define WEAPON_MODEL_SPRAYCAN			365
#define WEAPON_MODEL_FIREEXTINGUISHER	366
#define WEAPON_MODEL_CAMERA				367
#define WEAPON_MODEL_NIGHTVISION		368	// newly added
#define WEAPON_MODEL_INFRARED			369	// newly added
#define WEAPON_MODEL_JETPACK			370	// newly added
#define WEAPON_MODEL_PARACHUTE			371

#define OBJECT_PARACHUTE				3131
#define OBJECT_CJ_CIGGY					1485
#define OBJECT_DYN_BEER_1				1486
#define OBJECT_CJ_BEER_B_2				1543
#define OBJECT_CJ_PINT_GLASS			1546