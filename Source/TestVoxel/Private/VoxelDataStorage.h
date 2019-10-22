#pragma once

#include "VoxelData.h"
#include "VoxelDataAccessor.h"

class UVoxelChunk;

//Class for storing(and managing?) chunk block data
class FVoxelDataStorage : IVoxelDataAccessor
{
	FVoxelBlock InternalData[VOX_ARRAYSIZE];

	const UVoxelChunk* Owner;

	FVoxelDataStorage(UVoxelChunk* Owner);


	virtual void SetBlock(const FIntVector& VoxelPos, const FVoxelBlock& Block) override;

	virtual FVoxelBlock GetBlock(const FIntVector& VoxelPos) override;

	virtual FVoxelBlock* GetInternalArray(const FIntVector& ChunkPos) override;

};