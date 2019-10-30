#include "VoxelGlobalManager.h"
#include "VoxelWorld.h"
#include "VoxelBlockRegistry.h"

#define INIT_IF_NOT_INITED() if (!bIsInitialized) InitGlobalManager();

AVoxelGlobalManager::AVoxelGlobalManager()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	PrimaryActorTick.bHighPriority = true;
}

void AVoxelGlobalManager::BeginPlay()
{
	INIT_IF_NOT_INITED()
}

void AVoxelGlobalManager::EndPlay(EEndPlayReason::Type EndType)
{
	if (bIsInitialized)
	{
		DeinitGlobalManager();
	}
}

void AVoxelGlobalManager::InitGlobalManager()
{
	check(!bIsInitialized);

	BlockRegistryPtr = FBlockRegistry::GetInstance();
	bIsInitialized = true;
}

void AVoxelGlobalManager::DeinitGlobalManager()
{
	check(bIsInitialized);

	BlockRegistryPtr.Reset();
}

void AVoxelGlobalManager::RegisterVoxelWorld(AVoxelWorld* VoxelWorld)
{
	INIT_IF_NOT_INITED()
	WorldsList.Add(VoxelWorld);
}

void AVoxelGlobalManager::DeregisterVoxelWorld(AVoxelWorld* VoxelWorld)
{
	WorldsList.RemoveSwap(VoxelWorld);
}
