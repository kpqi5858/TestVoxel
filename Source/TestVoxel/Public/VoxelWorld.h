// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TestVoxel.h"
#include "VoxelData.h"
#include "VoxelChunk.h"
#include "GameFramework/Actor.h"
#include "VoxelWorld.generated.h"

class UVoxelChunk;

UCLASS()
class TESTVOXEL_API AVoxelWorld : public AActor
{
	GENERATED_BODY()
	
public:	
	AVoxelWorld();

	UPROPERTY(EditDefaultsOnly)
	float VoxelSize = 100.f;


protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	TMap<FIntVector, UVoxelChunk*> ChunksLoaded;

	//Can create a chunk
	UVoxelChunk* GetChunk(const FIntVector& VoxelPos);


	inline FIntVector WorldPosToVoxelPos(const FVector& WorldPos) const
	{
		return FIntVector(WorldPos / VoxelSize);
	}

	
};
