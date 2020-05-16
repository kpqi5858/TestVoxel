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
	ChunkState = EChunkState::LOADED;
}

void UVoxelChunk::GenerateWorld()
{
	ChunkState = EChunkState::WORLDGEN;

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

	ChunkState = EChunkState::DONE;
}

void UVoxelChunk::OnBlockSet(const FIntVector& LocalPos, const FVoxelBlock& Block)
{
	SetRenderDirty();

	if (FVoxelUtilities::IsInChunkBorder(LocalPos))
	{
		//We (probably) need to set adjacnet chunk dirty

		const FIntVector& VoxelPos = LocalPos + GetMinPos();

		for (int FaceNum = 0; FaceNum < VOX_FACENUM; FaceNum++)
		{
			const EBlockFace Face = static_cast<EBlockFace>(FaceNum);
			const FIntVector& AdjVoxelPos = VoxelPos + FVoxelUtilities::GetFaceOffset(Face);

			const FIntVector AdjChunkPos = FVoxelUtilities::VoxelPosToChunkIndex(AdjVoxelPos);

			if (AdjChunkPos != ChunkIndex)
			{
				//TODO : Calculate face occlusion

				//Set that chunk dirty
				VoxelWorld->GetChunk(AdjChunkPos)->SetRenderDirty();
			}
		}
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

UVoxelChunk* UVoxelChunk::GetAdjacentChunk(EBlockFace Face)
{
	return VoxelWorld->GetChunk(ChunkIndex + FVoxelUtilities::GetFaceOffset(Face));
}

FVector UVoxelChunk::GetWorldPos() const
{
	return FVector(GetMinPos()) * VoxelWorld->VoxelSize;
}
