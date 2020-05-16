#include "VoxelDataAccessor.h"
#include "VoxelChunk.h"
#include "VoxelWorld.h"

FVoxelWorldGenAccessor::FVoxelWorldGenAccessor(UVoxelChunk* Chunk)
	: Chunk(Chunk), World(Chunk->VoxelWorld)
{
	TempChunks.Reserve(10);
}

FVoxelWorldGenAccessor::~FVoxelWorldGenAccessor()
{
	ReleaseTempChunks();
}

inline FTemporaryChunk* FVoxelWorldGenAccessor::GetTempChunk(const FIntVector& ChunkPos)
{
	//Special handling
	if (ChunkPos == Chunk->ChunkIndex)
	{
		if (LocalTempChunk == nullptr)
		{
			LocalTempChunk = Chunk->NewTemporaryChunk();
			TempChunks.Add(ChunkPos, LocalTempChunk);
		}

		return LocalTempChunk;
	}

	auto Temp = TempChunks.Find(ChunkPos);

	if (Temp)
	{
		return *Temp;
	}
	else
	{
		FTemporaryChunk* NewChunk = World->GetChunk(ChunkPos)->NewTemporaryChunk();

		//This chunk is the chunk we're generating
		if (Chunk->ChunkIndex == ChunkPos)
		{
			//So give it high priority
			NewChunk->Priority = 0;
		}
		else
		{
			NewChunk->Priority = 10;
		}

		return TempChunks.Add(ChunkPos, NewChunk);
	}
}

void FVoxelWorldGenAccessor::ReleaseTempChunks()
{
	TArray<FTemporaryChunk*> TempChunkList;
	TempChunks.GenerateValueArray(TempChunkList);

	for (auto TempChunk : TempChunkList)
	{
		TempChunk->OwnerChunk->ReleaseTemporaryChunk(TempChunk);
	}
	TempChunks.Reset();
}

void FVoxelWorldGenAccessor::SetBlock(const FIntVector& VoxelPos, const FVoxelBlock& Block)
{
	FTemporaryChunk* TempChunk = GetTempChunk(FVoxelUtilities::VoxelPosToChunkIndex(VoxelPos));
	TempChunk->SetData(FVoxelUtilities::VoxelPosToLocalPos(VoxelPos), Block);
}

FVoxelBlock FVoxelWorldGenAccessor::GetBlock(const FIntVector& VoxelPos) const
{
	check(false);
	return FVoxelBlock();
}

FVoxelBlock* FVoxelWorldGenAccessor::GetInternalArray(const FIntVector& ChunkPos)
{
	check(false);
	return nullptr;
}
