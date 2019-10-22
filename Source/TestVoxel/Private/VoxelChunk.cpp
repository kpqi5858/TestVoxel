#include "VoxelChunk.h"

void UVoxelChunk::TickChunk()
{
}

void UVoxelChunk::GenerateWorld()
{
}

void UVoxelChunk::PolygonizeNow()
{
}

FTemporaryChunk* UVoxelChunk::NewTemporaryChunk()
{
	return TemporaryChunkList.Add();
}
