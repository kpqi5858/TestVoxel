// Fill out your copyright notice in the Description page of Project Settings.

#include "VoxelWorld.h"
#include "VoxelChunk.h"
#include "VoxelMesher.h"
#include "VoxelMeshComponentWrapper.h"
#include "VoxelGlobalManager.h"
#include "DrawDebugHelpers.h"

AVoxelWorld::AVoxelWorld()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}

void AVoxelWorld::BeginPlay()
{
	Super::BeginPlay();
	InitWorld();
}

FMeshComponentWrapper* AVoxelWorld::NewMeshComponentInternal()
{
	FMeshComponentWrapper* MeshComp = nullptr;

	switch (VoxelMesh)
	{
	case EVoxelWorldMesh::MESH_RMC :
	{
		MeshComp = new FRMCWrapper(this);
		break;
	}
	case EVoxelWorldMesh::MESH_PMC :
		MeshComp = new FPMCWrapper(this);
		break;
	default:
		unimplemented();
		break;
	}

	return AllMeshes.Add_GetRef(MeshComp);
}

void AVoxelWorld::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ChunkManager->ChunkManagerTick();
}

void AVoxelWorld::EndPlay(EEndPlayReason::Type Reason)
{
	DestroyWorld();
}

UVoxelChunk* AVoxelWorld::GetChunk(const FIntVector& ChunkPos)
{
	return ChunkManager->GetChunk(ChunkPos);
}

void AVoxelWorld::InitWorld()
{
	check(!bWorldInited);

	//Init worldgen
	UClass* WorldGenClass = WorldGenerator.Get();
	if (WorldGenClass == nullptr)
	{
		UE_LOG(LogTestVoxel, Error, TEXT("Error : World Generator is null"));
		WorldGenClass = UVoxelEmptyWorldGenerator::StaticClass();
	}

	UVoxelWorldGenerator* WorldGenInst = NewObject<UVoxelWorldGenerator>(this, WorldGenClass, TEXT("WorldGenObj"));

	WorldGenInst->Setup();
	WorldGenInstance = WorldGenInst;

	if (GlobalManagerDefault)
	{
		SetGlobalManager(GlobalManagerDefault);
	}

	ChunkManager = new FVoxelChunkManager(this);
	ChunkManager->Init();

	bWorldInited = true;
}

void AVoxelWorld::SetGlobalManager(AVoxelGlobalManager* Manager)
{
	//If already exists, replace
	if (ActiveGlobalManager)
	{
		ActiveGlobalManager->DeregisterVoxelWorld(this);
	}
	Manager->RegisterVoxelWorld(this);
	ActiveGlobalManager = Manager;
}

AVoxelGlobalManager* AVoxelWorld::GetGlobalManager()
{
	return ActiveGlobalManager;
}

TSharedPtr<FVoxelMesher> AVoxelWorld::GetMesher(UVoxelChunk* Chunk)
{
	FVoxelMesherDefaultSettings Settings;

	Settings.VoxelSize = VoxelSize;
	Settings.bFaceOcclusionBorders = false;

	return MakeShareable(new FVoxelMesherDefault(Chunk, Settings));
}

FMeshComponentWrapper* AVoxelWorld::GetFreeMeshComponent()
{
	if (FreeMeshes.Num())
	{
		return FreeMeshes.Pop(false);
	}
	else
	{
		return NewMeshComponentInternal();
	}
}

void AVoxelWorld::FreeMeshComponent(FMeshComponentWrapper* MeshComponent)
{
	FreeMeshes.Add(MeshComponent);
}

void AVoxelWorld::DrawDebugBlockAt(const FIntVector& VoxelPos)
{
	DrawDebugBox(GetWorld(), FVector(VoxelPos * VoxelSize) + FVector(VoxelSize / 2), FVector(VoxelSize / 2), FColor::Blue);
}

void AVoxelWorld::DrawDebugChunkBorder(const FIntVector& ChunkPos)
{
	DrawDebugBox(GetWorld(), (FVector(ChunkPos * VOX_CHUNKSIZE) + FVector(VOX_CHUNKSIZE / 2)) * VoxelSize, FVector(VoxelSize * VOX_CHUNKSIZE / 2), FColor::Orange);
}

void AVoxelWorld::DestroyWorld()
{
	check(bWorldInited);

	ChunkManager->Destroy();
	delete ChunkManager;
	ChunkManager = nullptr;

	for (auto& MeshComp : AllMeshes)
	{
		delete MeshComp;
	}
	AllMeshes.Empty();
	FreeMeshes.Empty();
}
