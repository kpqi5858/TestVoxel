#pragma once

#include "CoreMinimal.h"
#include "TestVoxel.h"
#include "VoxelUtilities.h"
#include "HAL/ThreadSafeBool.h"

struct FVoxelPolygonizedDataSection
{
	TArray<FVector> Vertices;
	TArray<int32> Tris;
	TArray<FVector> Normals;
	TArray<FColor> Colors;
};

struct FVoxelPolygonizedData
{
	TArray<FVoxelPolygonizedDataSection> Sections;

public:
	FVoxelPolygonizedDataSection& NewSection()
	{
		return Sections.AddDefaulted_GetRef();
	}
};


//6byte
struct FVoxelBlock
{
	uint16 Type = 0;
	FColor Color;
};


struct FTemporaryChunk
{
	//For sorting the order of temporary chunk
	int Priority;

	FVoxelBlock Data[VOX_ARRAYSIZE];
	//Indicates which data is modified
	//(Like caves, which sets block to air, but default FVoxelBlock is air)
	//TODO : Convert to bitflags to save memory
	bool DirtyData[VOX_ARRAYSIZE] = {};

	//Sets data, and sets dirty flag
	inline void SetData(const int Index, const FVoxelBlock& Value)
	{
		Data[Index] = Value;
		DirtyData[Index] = true;
	}
	inline void SetData(const FIntVector& LocalPos, const FVoxelBlock& Value)
	{
		SetData(VOX_AI(LocalPos.X, LocalPos.Y, LocalPos.Z), Value);
	}
};

typedef TSimpleLinkedList<FTemporaryChunk> FTemporaryChunkList;

