#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VoxelGlobalManager.generated.h"

class AVoxelWorld;
class FBlockRegistryInstance;

//Class for managing thread pools, etc
UCLASS()
class TESTVOXEL_API AVoxelGlobalManager : public AActor
{
	GENERATED_BODY()
public:
	AVoxelGlobalManager();

	TArray<TWeakObjectPtr<AVoxelWorld>> WorldsList;

	TSharedPtr<FBlockRegistryInstance> BlockRegistryPtr;

	bool bIsInitialized = false;

public:
	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type EndType) override;

	void InitGlobalManager();
	void DeinitGlobalManager();

	void RegisterVoxelWorld(AVoxelWorld* VoxelWorld);
	void DeregisterVoxelWorld(AVoxelWorld* VoxelWorld);
};