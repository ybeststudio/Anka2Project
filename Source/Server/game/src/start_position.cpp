#include "stdafx.h"
#include "start_position.h"

char g_nation_name[4][32] =
{
	"",
	"Shinsoo",
	"Chunjo",
	"Jinno",
};

long g_start_map[4] =
{
	0,	// reserved
	1,	// Kirmizi Imparatorluk
	21,	// Sari Imparatorluk
	41	// Mavi Imparatorluk
};

DWORD g_start_position[4][2] =
{
	{      0,      0 },	// reserved
	{ 469300, 964200 },	// Kýrmýzý Ýmparatorluk
	{  55700, 157900 },	// Sarý Ýmparatorluk
	{ 969600, 278400 }	// Mavi Ýmparatorluk
};

// Lonca Savaþý Warp
DWORD arena_return_position[4][2] =
{
	{       0,  0       },
	{   347600, 882700  },
	{   138600, 236600  },
	{   857200, 251800  }
};

// Karakter oluþturma
DWORD g_create_position[4][2] =
{
	{      0,      0 },	// reserviert
	{ 469300, 964200 },	// Kýrmýzý Ýmparatorluk
	{  55700, 157900 },	// Sarý Ýmparatorluk
	{ 969600, 278400 }	// Mavi Ýmparatorluk
};

DWORD g_create_position_canada[4][2] =
{
	{      0,      0 },	// reserviert
	{ 469300, 964200 },	// Kýrmýzý Ýmparatorluk
	{  55700, 157900 },	// Sarý Ýmparatorluk
	{ 969600, 278400 }	// Mavi Ýmparatorluk
};
