#pragma once

#include "VoxelData.h"
#include "VoxelDataAccessor.h"

class UVoxelChunk;

//Class for storing(and managing?) chunk block data
class FVoxelDataStorage : public IVoxelDataAccessor
{
	FVoxelBlock InternalData[VOX_ARRAYSIZE];

	UVoxelChunk* Owner;

public:
	FVoxelDataStorage(UVoxelChunk* Owner);

	virtual void SetBlock(const FIntVector& VoxelPos, const FVoxelBlock& Block) override;
	virtual FVoxelBlock GetBlock(const FIntVector& VoxelPos) const override;
	virtual FVoxelBlock* GetInternalArray(const FIntVector& ChunkPos) override;

};