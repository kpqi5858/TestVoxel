#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VoxelUtilities.h"
#include "VoxelGlobalManager.generated.h"

class AVoxelWorld;
class FBlockRegistryInstance;
struct FTemporaryChunk;
class UVoxelChunk;

//Class for managing thread pools, etc
UCLASS()
class TESTVOXEL_API AVoxelGlobalManager : public AActor
{
	GENERATED_BODY()
public:
	AVoxelGlobalManager();

	TArray<AVoxelWorld*> WorldsList;

	TSharedPtr<FBlockRegistryInstance> BlockRegistryPtr;

	bool bIsInitialized = false;

	//Register player pawn automatically?
	UPROPERTY(EditDefaultsOnly)
	bool bRegisterPlayerPawn = false;

	//Temporary Chunk pool's initial objects
	UPROPERTY(EditDefaultsOnly)
	int TempChunkPoolInitialCount = 500;

	uint64 TickAge = 0L;

private:
	TArray<AActor*> InvokerList;

	TArray<FTemporaryChunk*> TempChunkPool;
	FSpinLock TempChunkPoolLock;

	FTemporaryChunk* TempChunkOriginal = nullptr;

private:
	inline FTemporaryChunk* NewTempChunk();

public:
	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type EndType) override;

	void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	void RegisterInvoker(AActor* Actor);
	UFUNCTION(BlueprintCallable)
	void DeregisterInvoker(AActor* Actor);

	void RemoveInvalids();

	void InitGlobalManager();
	void DeinitGlobalManager();

	void RegisterVoxelWorld(AVoxelWorld* VoxelWorld);
	void DeregisterVoxelWorld(AVoxelWorld* VoxelWorld);

	FTemporaryChunk* GetNewTempChunk(UVoxelChunk* OwnerChunk);
	void ReleaseTempChunk(FTemporaryChunk* TempChunk);;

	//Calculates minimum distance to invokers
	float GetDistanceToInvokers(FVector WorldPos);

	template<typename Lambda>
	void ForEachInvokers(Lambda Func)
	{
		bool HasInvalids = false;

		for (auto& Invoker : InvokerList)
		{
			if (IsValid(Invoker))
			{
				Func(Invoker);
			}
			else
			{
				HasInvalids = true;
			}
		}

		if (HasInvalids)
		{
			RemoveInvalids();
		}
	};

};
