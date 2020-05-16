#pragma once

#include "CoreMinimal.h"
#include "VoxelData.h"

class UVoxelChunk;
class AVoxelWorld;

//Default chunk manager.
class FVoxelChunkManager
{
public:
	AVoxelWorld* World;

protected:
	TMap<FIntVector, UVoxelChunk*> ChunksLoaded;

	FCriticalSection ChunkListLock;

public:
	FVoxelChunkManager(AVoxelWorld* Parent);
	virtual ~FVoxelChunkManager() {};

	virtual UVoxelChunk* GetChunk(const FIntVector& ChunkPos);

	virtual void RemoveChunk(UVoxelChunk* Chunk);

	virtual void ChunkManagerTick();
	
	virtual void Init();
	virtual void Destroy();

protected:
	virtual UVoxelChunk* CreateNewChunk(const FIntVector& ChunkPos);

};