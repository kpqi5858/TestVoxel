#pragma once

#include "VoxelData.h"

class UVoxelChunk;
class AVoxelWorld;
struct FTemporaryChunk;

//Interface for accessing the blocks
class IVoxelDataAccessor
{
public:
	//Sets block at position(Global)
	virtual void SetBlock(const FIntVector& VoxelPos, const FVoxelBlock& Block) = 0;
	//Gets block at position(Global)
	virtual FVoxelBlock GetBlock(const FIntVector& VoxelPos) const = 0;

	//Gets ENTIRE internal array at position(chunk index)
	//Use with caution
	virtual FVoxelBlock* GetInternalArray(const FIntVector& ChunkPos) = 0;
};

//This can access every chunk's own data accessor, even it can create chunks.
//Use with caution
class FVoxelGlobalAccessor : public IVoxelDataAccessor
{
	FVoxelGlobalAccessor(AVoxelWorld* World);
	
	const AVoxelWorld* World;

public:
	virtual void SetBlock(const FIntVector& VoxelPos, const FVoxelBlock& Block) override;

	virtual FVoxelBlock GetBlock(const FIntVector& VoxelPos) const override;

	virtual FVoxelBlock* GetInternalArray(const FIntVector& ChunkPos) override;

};

//For world generation purpose of that chunk.
//Will create FTemporaryChunk for each chunk accessed.
//And will merged with main storage of that chunk.
class FVoxelWorldGenAccessor : public IVoxelDataAccessor
{
public:
	//Chunk - The chunk that worldgen is working
	FVoxelWorldGenAccessor(UVoxelChunk* Chunk);
	virtual ~FVoxelWorldGenAccessor();

private:
	UVoxelChunk* Chunk;
	AVoxelWorld* World;

	//Temporary chunks
	TMap<FIntVector, FTemporaryChunk*> TempChunks;

	inline FTemporaryChunk* GetTempChunk(const FIntVector& ChunkPos);

public:
	//Releases temp chunks
	virtual void ReleaseTempChunks();

	virtual void SetBlock(const FIntVector& VoxelPos, const FVoxelBlock& Block) override;
	//Can't be used
	virtual FVoxelBlock GetBlock(const FIntVector& VoxelPos) const override;
	//Can't be used
	virtual FVoxelBlock* GetInternalArray(const FIntVector& ChunkPos) override;
};

//For polygonizing the chunk
//Can access near chunk's block
class FVoxelMesherAccessor : public IVoxelDataAccessor
{

};