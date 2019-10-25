// Fill out your copyright notice in the Description page of Project Settings.

#include "VoxelWorld.h"
#include "VoxelMesher.h"

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

// Called every frame
void AVoxelWorld::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UVoxelChunk* AVoxelWorld::GetChunk(const FIntVector& ChunkPos)
{
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

	auto Test = GetChunk(FIntVector(-1));

	Test->GenerateWorld();
	Test->MergeTempChunkNow();
}

TSharedPtr<FVoxelMesher> AVoxelWorld::GetMesher(UVoxelChunk* Chunk)
{
	FVoxelMesherDefaultSettings Settings;

	Settings.VoxelSize = VoxelSize;

	return MakeShareable(new FVoxelMesherDefault(Chunk, Settings));
}

