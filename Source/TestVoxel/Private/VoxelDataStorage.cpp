#include "VoxelDataStorage.h"
#include "VoxelChunk.h"
#include "VoxelUtilities.h"

FVoxelDataStorage::FVoxelDataStorage(UVoxelChunk* Owner)
	: Owner(Owner)
{
}

void FVoxelDataStorage::SetBlock(const FIntVector& VoxelPos, const FVoxelBlock& Block)
{
	check(Owner->ChunkIndex == FVoxelUtilities::VoxelPosToChunkIndex(VoxelPos));
	const FIntVector LocalPos = FVoxelUtilities::VoxelPosToLocalPos(VoxelPos);

	InternalData[VOX_AI(LocalPos.X, LocalPos.Y, LocalPos.Z)] = Block;

	Owner->OnBlockSet(LocalPos, Block);
}

FVoxelBlock FVoxelDataStorage::GetBlock(const FIntVector& VoxelPos) const
{
	check(Owner->ChunkIndex == FVoxelUtilities::VoxelPosToChunkIndex(VoxelPos));
	const FIntVector LocalPos = FVoxelUtilities::VoxelPosToLocalPos(VoxelPos);

	return InternalData[VOX_AI(LocalPos.X, LocalPos.Y, LocalPos.Z)];
}

FVoxelBlock* FVoxelDataStorage::GetInternalArray(const FIntVector& ChunkPos)
{
	check(Owner->ChunkIndex == ChunkPos);
	return InternalData;
}
