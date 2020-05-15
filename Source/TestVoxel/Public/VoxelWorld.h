// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TestVoxel.h"
#include "VoxelData.h"
#include "VoxelWorldGen.h"
#include "GameFramework/Actor.h"
#include "RuntimeMeshComponent.h"

#include "VoxelWorld.generated.h"

class UVoxelChunk;
class UVoxelWorldGenerator;
class FVoxelMesher;
class FMeshComponentWrapper;
class AVoxelGlobalManager;


UENUM(BlueprintType)
enum class EVoxelWorldMode : uint8
{
	//Keep generating world
	//Unload chunk if too far
	WM_WorldGen,
	//Generate chunk by placing block
	//Unload if too far
	WM_Const
};

UENUM(BlueprintType)
enum class EVoxelWorldMesh : uint8
{
	//Use RuntimeMeshComponent
	MESH_RMC,
	//Use ProceduralMeshComponent
	MESH_PMC
};

UCLASS()
class TESTVOXEL_API AVoxelWorld : public AActor
{
	GENERATED_BODY()
	
public:	
	AVoxelWorld();

	UPROPERTY(EditAnywhere)
	float VoxelSize = 100.f;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UVoxelWorldGenerator> WorldGenerator;

	UPROPERTY(EditAnywhere)
	AVoxelGlobalManager* GlobalManagerDefault;

	UPROPERTY(EditAnywhere)
	EVoxelWorldMode VoxelWorldMode;

	UPROPERTY(EditAnywhere)
	EVoxelWorldMesh VoxelMesh;

	UPROPERTY()
	UVoxelWorldGenerator* WorldGenInstance;

	UPROPERTY(BlueprintReadOnly)
	AVoxelGlobalManager* ActiveGlobalManager;


	TArray<FMeshComponentWrapper*> FreeMeshes;
	TArray<FMeshComponentWrapper*> AllMeshes;

	FCriticalSection ChunkListLock;

protected:
	virtual void BeginPlay() override;

	FMeshComponentWrapper* NewMeshComponentInternal();

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(EEndPlayReason::Type Reason) override;

	TMap<FIntVector, UVoxelChunk*> ChunksLoaded;

	//Can create a chunk
	UVoxelChunk* GetChunk(const FIntVector& ChunkPos);

	void InitWorld();

	void SetGlobalManager(AVoxelGlobalManager* Manager);
	AVoxelGlobalManager* GetGlobalManager();

	TSharedPtr<FVoxelMesher> GetMesher(UVoxelChunk* Chunk);


	FMeshComponentWrapper* GetFreeMeshComponent();
	void FreeMeshComponent(FMeshComponentWrapper* MeshComponent);

	void DestroyWorld();

	inline FIntVector WorldPosToVoxelPos(const FVector& WorldPos) const
	{
		return FIntVector(WorldPos / VoxelSize);
	}

	
};
