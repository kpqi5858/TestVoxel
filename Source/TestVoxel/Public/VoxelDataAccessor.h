#pragma once

#include "VoxelData.h"

class UVoxelChunk;
class UVoxelWorld;
struct FTemporaryChunk;

//Interface for accessing the blocks
class IVoxelDataAccessor
{
public:
	//Sets block at position(Global)
	virtual void SetBlock(const FIntVector& VoxelPos, const FVoxelBlock& Block) = 0;
	//Gets block at position(Global)
	virtual FVoxelBlock GetBlock(const FIntVector& VoxelPos) = 0;

	//Gets ENTIRE internal array at position(chunk index)
	//Use with caution
	virtual FVoxelBlock* GetInternalArray(const FIntVector& ChunkPos) = 0;
};

//This can access every chunk's own data accessor, even it can create chunks.
//Use with caution
class FVoxelGlobalAccessor : public IVoxelDataAccessor
{
	FVoxelGlobalAccessor(UVoxelWorld* World);
	
	const UVoxelWorld* World;

public:
	virtual void SetBlock(const FIntVector& VoxelPos, const FVoxelBlock& Block) override;

	virtual FVoxelBlock GetBlock(const FIntVector& VoxelPos) override;

	virtual FVoxelBlock* GetInternalArray(const FIntVector& ChunkPos) override;

};

//For world generation purpose of that chunk.
//Will create FTemporaryChunk for each chunk accessed.
//And will merged with main storage of that chunk.
class FVoxelWorldGenAccessor : public IVoxelDataAccessor
{
	//Chunk - The chunk that worldgen is working
	//Will create temp chunk for that
	FVoxelWorldGenAccessor(UVoxelChunk* Chunk);
	virtual ~FVoxelWorldGenAccessor();

	UVoxelChunk* Chunk;
	UVoxelWorld* World;

	//Temporary chunks
	TMap<FIntVector, FTemporaryChunk*> TempChunks;

private:
	inline FTemporaryChunk* GetTempChunk(const FIntVector& ChunkPos);
public:
	virtual void SetBlock(const FIntVector& VoxelPos, const FVoxelBlock& Block) override;

	//Can't be used
	virtual FVoxelBlock GetBlock(const FIntVector& VoxelPos) override;
	//Can't be used
	virtual FVoxelBlock* GetInternalArray(const FIntVector& ChunkPos) override;
};
