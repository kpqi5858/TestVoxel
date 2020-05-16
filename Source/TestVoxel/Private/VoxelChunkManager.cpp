#include "VoxelChunkManager.h"
#include "VoxelWorld.h"
#include "VoxelChunk.h"
#include "VoxelGlobalManager.h"

FVoxelChunkManager::FVoxelChunkManager(AVoxelWorld* Parent)
	: World(Parent)
{
}

UVoxelChunk* FVoxelChunkManager::GetChunk(const FIntVector& ChunkPos)
{
	FScopeLock Lock(&ChunkListLock);

	auto Find = ChunksLoaded.Find(ChunkPos);
	if (Find)
		return *Find;

	return CreateNewChunk(ChunkPos);
}

void FVoxelChunkManager::RemoveChunk(UVoxelChunk* Chunk)
{
	FScopeLock Lock(&ChunkListLock);
	
	check(ChunksLoaded.FindAndRemoveChecked(Chunk->ChunkIndex) == Chunk);
}

void FVoxelChunkManager::ChunkManagerTick()
{
	World->GetGlobalManager()->ForEachInvokers([&](AActor* Invoker)
	{
		FVector ActorLoc = Invoker->GetActorLocation();
		FIntVector VoxelPos = World->WorldPosToVoxelPos(ActorLoc);
		FIntVector ChunkPos = FVoxelUtilities::VoxelPosToChunkIndex(VoxelPos);

		UVoxelChunk* Chunk = GetChunk(ChunkPos);
		
		World->DrawDebugBlockAt(VoxelPos);
		World->DrawDebugChunkBorder(Chunk->ChunkIndex);

		if (Chunk->ChunkState == EChunkState::LOADED)
		{
			Chunk->GenerateWorld();
			Chunk->MergeTempChunkNow();
			Chunk->PolygonizeNow();
		}
	});
}

void FVoxelChunkManager::Init()
{
}

void FVoxelChunkManager::Destroy()
{
	TArray<UVoxelChunk*> AllChunks;
	
	ChunksLoaded.GenerateValueArray(AllChunks);

	for (auto& Chunk : AllChunks)
	{
		Chunk->RemoveFromRoot();
	}

	ChunksLoaded.Empty();
}

UVoxelChunk* FVoxelChunkManager::CreateNewChunk(const FIntVector& ChunkPos)
{
	UVoxelChunk* NewChunk = NewObject<UVoxelChunk>(World, *FString::Printf(TEXT("VoxelChunk-%d_%d_%d"), ChunkPos.X, ChunkPos.Y, ChunkPos.Z));

	NewChunk->AddToRoot();
	NewChunk->Setup(World, ChunkPos);

	ChunksLoaded.Add(ChunkPos, NewChunk);

	return NewChunk;
}
