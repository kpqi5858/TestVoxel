// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTestVoxel, Log, All)

#define VOX_CHUNKSIZE 16
#define VOX_ARRAYSIZE (VOX_CHUNKSIZE*VOX_CHUNKSIZE*VOX_CHUNKSIZE)

//Do VOX_AI parameter checks?
//This can degrade performance somewhat slightly.
#define DO_VOX_AICHECKS 1


#define VOX_AI_UNCHECKED(x,y,z) ((z*VOX_CHUNKSIZE*VOX_CHUNKSIZE)+(y*VOX_CHUNKSIZE)+x)
#define VOX_AI_CHECKED(x,y,z) GetVoxelArrayIndex_Checked(x, y, z)

#define VOX_AI_ISVALID(x,y,z) ((x >= 0 && x < VOX_CHUNKSIZE) && (y >= 0 && y < VOX_CHUNKSIZE) && (z >= 0 && z < VOX_CHUNKSIZE))

static FORCEINLINE int GetVoxelArrayIndex_Checked(const int X, const int Y, const int Z)
{
	check(VOX_AI_ISVALID(X, Y, Z));

	return VOX_AI_UNCHECKED(X, Y, Z);
}

#if DO_VOX_AICHECKS
#define VOX_AI(x,y,z) VOX_AI_CHECKED(x,y,z)
#else
#define VOX_AI(x,y,z) VOX_AI_UNCHECKED(x,y,z)
#endif
