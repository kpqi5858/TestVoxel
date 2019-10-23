#pragma once

#include "CoreMinimal.h"
#include "TestVoxel.h"
#include "VoxelUtilities.h"
#include "HAL/ThreadSafeBool.h"

class UVoxelChunk;

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
	uint16 Type;
	FColor Color;

	FVoxelBlock()
		: Type(0), Color(FColor::White)
	{

	}
	FVoxelBlock(uint16 Type, FColor Color = FColor::White)
		: Type(Type), Color(Color)
	{
	}
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

	UVoxelChunk* OwnerChunk;

	FTemporaryChunk(UVoxelChunk* Chunk)
		: OwnerChunk(Chunk)
	{
	}

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

	inline bool IsDataDirty(const int Index)
	{
		return DirtyData[Index];
	}
	inline bool IsDataDirty(const FIntVector& LocalPos)
	{
		return IsDataDirty(VOX_AI(LocalPos.X, LocalPos.Y, LocalPos.Z));
	}

	inline FVoxelBlock GetData(const int Index)
	{
		return Data[Index];
	}
	inline FVoxelBlock GetData(const FIntVector& LocalPos)
	{
		return GetData(VOX_AI(LocalPos.X, LocalPos.Y, LocalPos.Z));
	}

	bool operator<(const FTemporaryChunk& Other) const
	{
		return Other.Priority < Priority;
	}
};

typedef TSimpleLinkedList<FTemporaryChunk*> FTemporaryChunkList;

