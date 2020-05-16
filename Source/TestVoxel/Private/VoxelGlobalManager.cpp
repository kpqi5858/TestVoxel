#include "VoxelGlobalManager.h"
#include "VoxelWorld.h"
#include "VoxelBlockRegistry.h"
#include "VoxelData.h"
#include "GameFramework/PlayerController.h"

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

void AVoxelGlobalManager::Tick(float DeltaTime)
{
	const int RemoveInvalidTicks = 600;

	if (TickAge % RemoveInvalidTicks == 0)
	{
		RemoveInvalids();
	}

	TickAge++;
}

void AVoxelGlobalManager::RegisterInvoker(AActor* Actor)
{
	if (!IsValid(Actor))
	{
		UE_LOG(LogTestVoxel, Error, TEXT("%s : RegisterInvoker with invalid actor"), *GetName());
		return;
	}
	if (InvokerList.Contains(Actor))
	{
		UE_LOG(LogTestVoxel, Error, TEXT("%s : Duplicated invoker (%s)"), *GetName(), *Actor->GetName());
		return;
	}
	InvokerList.Add(Actor);
}

void AVoxelGlobalManager::DeregisterInvoker(AActor* Actor)
{
	if (!InvokerList.Contains(Actor))
	{
		UE_LOG(LogTestVoxel, Error, TEXT("%s : InvokerList doesn't contains %s"), *GetName(), *Actor->GetName());
		return;
	}
	InvokerList.RemoveSwap(Actor);
}

void AVoxelGlobalManager::RemoveInvalids()
{
	InvokerList.RemoveAll([](const AActor* ToCheck) {return IsValid(ToCheck); });
	WorldsList.RemoveAll([](const AActor* ToCheck) {return IsValid(ToCheck); });
}

void AVoxelGlobalManager::InitGlobalManager()
{
	check(!bIsInitialized);

	BlockRegistryPtr = FBlockRegistry::GetInstance();
	InvokerList.Reset();

	TempChunkOriginal = new FTemporaryChunk();

	TempChunkPool.Reserve(TempChunkPoolInitialCount);

	for (int Count = 0; Count < TempChunkPoolInitialCount; Count++)
	{
		TempChunkPool.Add(NewTempChunk());
	}

	if (bRegisterPlayerPawn)
	{
		InvokerList.Add(GetWorld()->GetFirstPlayerController()->GetPawn());
	}

	bIsInitialized = true;
}

void AVoxelGlobalManager::DeinitGlobalManager()
{
	check(bIsInitialized);

	BlockRegistryPtr.Reset();
	InvokerList.Reset();
	WorldsList.Reset();

	for (auto& TempChunk : TempChunkPool)
	{
		//We used Malloc, so use Free
		FMemory::Free(TempChunk);
	}

	TempChunkPool.Reset();
	
	delete TempChunkOriginal;
	TempChunkOriginal = nullptr;

	bIsInitialized = false;
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

inline FTemporaryChunk* AVoxelGlobalManager::NewTempChunk()
{
	FTemporaryChunk* NewChunk = reinterpret_cast<FTemporaryChunk*>(FMemory::Malloc(sizeof(FTemporaryChunk)));

	return NewChunk;
}

FTemporaryChunk* AVoxelGlobalManager::GetNewTempChunk(UVoxelChunk* OwnerChunk)
{
	FTemporaryChunk* TempChunk = nullptr;
	
	TempChunkPoolLock.Lock();

	if (TempChunkPool.Num())
	{
		TempChunk = TempChunkPool.Pop(false);
		TempChunkPoolLock.Unlock();
	}
	else
	{
		TempChunkPoolLock.Unlock();
		TempChunk = NewTempChunk();
	}

	//Reset the data
	FMemory::Memcpy(TempChunk, &TempChunkOriginal, sizeof(FTemporaryChunk));

	TempChunk->OwnerChunk = OwnerChunk;

	return TempChunk;
}

void AVoxelGlobalManager::ReleaseTempChunk(FTemporaryChunk* TempChunk)
{
	FScopeSpinLock sl(TempChunkPoolLock);

	TempChunkPool.Add(TempChunk);
}

float AVoxelGlobalManager::GetDistanceToInvokers(FVector WorldPos)
{
	float Result = FLT_MAX;
	
	if (InvokerList.Num() == 0)
	{
		UE_LOG(LogTestVoxel, Error, TEXT("%s : GetDistanceToInvokers with empty InvokerList"), *GetName());
		return Result;
	}

	ForEachInvokers([&](AActor* Invoker)
	{
		Result = FMath::Min(Result, FVector::Dist(WorldPos, Invoker->GetActorLocation()));
	});

	return Result;
}
