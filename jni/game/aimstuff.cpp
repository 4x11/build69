#include "main.h"
#include "game.h"

CAMERA_AIM* GameGetInternalAim()
{
	return (CAMERA_AIM*)(g_libGTASA+0x8B0AE0);
}