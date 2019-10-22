// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTestVoxel, Log, All)

#define VOX_CHUNKSIZE 16
#define VOX_ARRAYSIZE (VOX_CHUNKSIZE*VOX_CHUNKSIZE*VOX_CHUNKSIZE)

//Array index
#define VOX_AI(x,y,z) ((z*VOX_CHUNKSIZE*VOX_CHUNKSIZE)+(y*VOX_CHUNKSIZE)+x)
