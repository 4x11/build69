#pragma once

/*
typedef struct _PAD_KEYS
{
	uint16_t wWalkLR;
	uint16_t wWalkUD;
	bool bDive;
	bool bSwimJump;
	bool bSprint;
	bool bJump;
	bool bMeleeAttack;
	bool bMeleeAttack2;
	bool bDuck;
} PAD_KEYS;
*/

enum ePadKeys
{
	// KEY_ 						OnFoot 				InCar
	KEY_ACTION = 0, 		// TAB 			| 	ALT GR/LCTRL/NUM0	|	1
	KEY_CROUCH, 			// C 			|	H / CAPSLOCK		| 	2
	KEY_FIRE,				// LCTRL/LMB	|	LALT				| 	4
	KEY_SPRINT,				// SPACE		| 	W 					|	8
	KEY_SECONDARY_ATTACK, 	// ENTER		| 	ENTER 				|	16
	KEY_JUMP,				// LSHIFT		|	S 					| 	32
	KEY_LOOK_RIGHT,			// 				| 	E 					| 	64
	KEY_HANDBRAKE,			// RMB			|	SPACE 				| 	128
	KEY_LOOK_LEFT,			// 				|	Q 					| 	256
	//bool KEY_LOOK_BEHIND;	// KEY_LOOK_LEFT + KEY_LOOK_RIGHT 		| 	320
	KEY_SUBMISSION,			// NUM1/MMB		| 	2/NUMPAD+ 			| 	512
	KEY_WALK,				// LALT			| 	 					| 	1024
	KEY_ANALOG_UP,			// NUM8			| 	NUM8				| 	2048
	KEY_ANALOG_DOWN,		// NUM2 		| 	NUM2				|	4096
	KEY_ANALOG_LEFT,		// NUM4			| 	NUM4				|	8192
	KEY_ANALOG_RIGHT,		// NUM6			| 	NUM6				| 	16384
	KEY_YES,				// Y 			| 	Y 					| 	65536
	KEY_NO,					// N 			| 	N 					| 	131072
	SIZE
};

typedef struct
{
	uint16_t wKeyLR;
	uint16_t wKeyUD;
	bool bKeys[ePadKeys::SIZE];
	bool bIgnoreJump;
} PAD_KEYS;

extern PAD_KEYS LocalPlayerKeys;
extern PAD_KEYS RemotePlayerKeys[PLAYER_PED_SLOTS];