#include "VoxelChunk.h"
#include "VoxelDataAccessor.h"
#include "VoxelDataStorage.h"
#include "VoxelWorld.h"
#include "VoxelMesher.h"
#include "VoxelMeshComponentWrapper.h"
#include "VoxelGlobalManager.h"

UVoxelChunk::UVoxelChunk()
{
	DataStorage = new FVoxelDataStorage(this);
}

void UVoxelChunk::TickChunk()
{
}

void UVoxelChunk::Setup(AVoxelWorld* World, const FIntVector& ChunkPos)
{
	VoxelWorld = World;
	ChunkIndex = ChunkPos;
}

void UVoxelChunk::GenerateWorld()
{
	FVoxelWorldGenAccessor* Accessor = new FVoxelWorldGenAccessor(this);
	VoxelWorld->WorldGenInstance->GenerateWorld(this, Accessor);
	delete Accessor;
}

void UVoxelChunk::PolygonizeNow()
{
	auto Mesher = VoxelWorld->GetMesher(this);
	Mesher->Polygonize(MeshData);

	auto Mesh = VoxelWorld->GetFreeMeshComponent();
	Mesh->SetPosition(GetWorldPos());
	Mesh->SetVisiblity(true);
	Mesh->UpdateMeshData(MeshData);
}

void UVoxelChunk::MergeTempChunkNow()
{
	auto TempChunksPtr = TemporaryChunkList.Pop();

	TArray<FTemporaryChunk> TempChunks;
	for (auto& Ptr : TempChunksPtr)
	{
		TempChunks.Add(*Ptr);
	}

	//Sort by Priority
	TempChunks.Sort();

	const FIntVector Offset = GetMinPos();

	for (int X = 0; X < VOX_CHUNKSIZE; X++)
	for (int Y = 0; Y < VOX_CHUNKSIZE; Y++)
	for (int Z = 0; Z < VOX_CHUNKSIZE; Z++)
	{
		const FIntVector LocalPos = FIntVector(X, Y, Z);
		const FIntVector VoxelPos = Offset + LocalPos;
		
		for (int Index = TempChunks.Num() - 1; Index >= 0; Index--)
		{
			FTemporaryChunk& TempChunk = TempChunks[Index];
			
			if (TempChunk.IsDataDirty(LocalPos))
			{
				DataStorage->SetBlock(VoxelPos, TempChunk.GetData(LocalPos));
				break;
			}
		}
	}

	for (auto& Ptr : TempChunksPtr)
	{
		VoxelWorld->ActiveGlobalManager->ReleaseTempChunk(Ptr);
	}
}

FTemporaryChunk* UVoxelChunk::NewTemporaryChunk()
{
	FTemporaryChunk* TempChunk = VoxelWorld->ActiveGlobalManager->GetNewTempChunk(this);
	
	WorldGenRefs.Increment();

	return TempChunk;
}

void UVoxelChunk::ReleaseTemporaryChunk(FTemporaryChunk* TempChunk)
{
	TemporaryChunkList.Add(TempChunk);

	WorldGenRefs.Decrement();
}

FVector UVoxelChunk::GetWorldPos() const
{
	return FVector(GetMinPos()) * VoxelWorld->VoxelSize;
}
