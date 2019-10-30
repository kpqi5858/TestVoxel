// Fill out your copyright notice in the Description page of Project Settings.

#include "VoxelWorld.h"
#include "VoxelMesher.h"
#include "VoxelMeshComponentWrapper.h"
#include "VoxelGlobalManager.h"

// Sets default values
AVoxelWorld::AVoxelWorld()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}

// Called when the game starts or when spawned
void AVoxelWorld::BeginPlay()
{
	Super::BeginPlay();
	InitWorld();
}

FMeshComponentWrapper* AVoxelWorld::NewMeshComponentInternal()
{
	FMeshComponentWrapper* MeshComp = new FRMCWrapper(this);
	return AllMeshes.Add_GetRef(MeshComp);
}

// Called every frame
void AVoxelWorld::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVoxelWorld::EndPlay(EEndPlayReason::Type Reason)
{
	DestroyWorld();
}

UVoxelChunk* AVoxelWorld::GetChunk(const FIntVector& ChunkPos)
{
	FScopeLock Lock(&ChunkListLock);

	auto Find = ChunksLoaded.Find(ChunkPos);
	if (Find) 
		return *Find;
	
	UVoxelChunk* NewChunk = NewObject<UVoxelChunk>(this);
	NewChunk->Setup(this, ChunkPos);
	return ChunksLoaded.Add(ChunkPos, NewChunk);
}

void AVoxelWorld::InitWorld()
{
	//Init worldgen
	UClass* WorldGenClass = WorldGenerator.Get();
	if (WorldGenClass == nullptr)
	{
		UE_LOG(LogTestVoxel, Error, TEXT("Error : World Generator is null"));
		WorldGenClass = UVoxelEmptyWorldGenerator::StaticClass();
	}

	UVoxelWorldGenerator* WorldGenInst = NewObject<UVoxelWorldGenerator>(this, WorldGenClass);
	WorldGenInst->Setup();

	WorldGenInstance = WorldGenInst;

	if (GlobalManagerDefault)
	{
		SetGlobalManager(GlobalManagerDefault);
	}

	auto Test = GetChunk(FIntVector(-1));

	Test->GenerateWorld();
	Test->MergeTempChunkNow();
	Test->PolygonizeNow();
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

void AVoxelWorld::DestroyWorld()
{
	ChunksLoaded.Empty();
	for (auto& MeshComp : AllMeshes)
	{
		delete MeshComp;
	}
	AllMeshes.Empty();
	FreeMeshes.Empty();
}

