#pragma once

#include "CoreMinimal.h"
#include "VoxelData.h"
#include "VoxelDataAccessor.h"

#include "VoxelChunk.generated.h"


class AVoxelWorld;
class FVoxelDataStorage;

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

	FVoxelDataStorage* DataStorage;

	FTemporaryChunkList TemporaryChunkList;
	FThreadSafeCounter WorldGenRefs;

	FIntVector ChunkIndex;

	AVoxelWorld* VoxelWorld;

	bool IsRenderDirty = false;

public:
	//Tick function, Always in game thread.
	//Can be called infrequently
	void TickChunk();

	void Setup(AVoxelWorld* World, const FIntVector& ChunkPos);

	void GenerateWorld();

	void PolygonizeNow();

	void MergeTempChunkNow();

	FTemporaryChunk* NewTemporaryChunk();
	void ReleaseTemporaryChunk(FTemporaryChunk* TempChunk);
	

	inline FIntVector GetMinPos() const
	{
		return ChunkIndex * VOX_CHUNKSIZE;
	}
	inline FIntVector GetMaxPos() const
	{
		return ChunkIndex * (VOX_CHUNKSIZE + 1) - FIntVector(1);
	}

	FVector GetWorldPos() const;
};