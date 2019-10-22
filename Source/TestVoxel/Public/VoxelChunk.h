#pragma once

#include "CoreMinimal.h"
#include "VoxelData.h"
#include "VoxelDataAccessor.h"

#include "VoxelChunk.generated.h"


class UVoxelWorld;

enum class EChunkState : uint8
{
	//Invalid chunk?
	INVALID,
	//Just loaded, maybe because of other chunk's worldgen
	LOADED,
	//Doing any kind of world generation
	WORLDGEN,
	//World generation is done, (maybe waiting for other worldgen finish, and merging)
	DONE
};

UCLASS()
class TESTVOXEL_API UVoxelChunk : public UObject
{
	GENERATED_BODY()

public:
	UVoxelChunk();

	FVoxelPolygonizedData MeshData;

	FVoxelBlock BlockData[VOX_ARRAYSIZE];

	FTemporaryChunkList TemporaryChunkList;

	//Chunk data accessor
	IVoxelDataAccessor* DataAccessor;

	FIntVector ChunkIndex;

	UVoxelWorld* VoxelWorld;

	bool IsRenderDirty = false;

public:
	//Tick function, Always in game thread.
	//Can be called infrequently
	void TickChunk();

	void GenerateWorld();

	void PolygonizeNow();

	FTemporaryChunk* NewTemporaryChunk();
	
};