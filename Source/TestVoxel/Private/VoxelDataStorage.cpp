#include "VoxelDataStorage.h"
#include "VoxelChunk.h"
#include "VoxelUtilities.h"

FVoxelDataStorage::FVoxelDataStorage(UVoxelChunk* Owner)
	: Owner(Owner)
{
}

void FVoxelDataStorage::SetBlock(const FIntVector& VoxelPos, const FVoxelBlock& Block)
{
}

FVoxelBlock FVoxelDataStorage::GetBlock(const FIntVector& VoxelPos)
{
	return FVoxelBlock();
}

FVoxelBlock* FVoxelDataStorage::GetInternalArray(const FIntVector& ChunkPos)
{
	return nullptr;
}
