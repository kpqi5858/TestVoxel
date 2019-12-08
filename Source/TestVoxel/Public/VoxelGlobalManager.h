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

	TArray<TWeakObjectPtr<AVoxelWorld>> WorldsList;

	TSharedPtr<FBlockRegistryInstance> BlockRegistryPtr;

	bool bIsInitialized = false;
	
	//Register player pawn automatically?
	UPROPERTY(BlueprintReadWrite)
	bool bRegisterPlayerPawn = false;

	//Temporary Chunk pool's initial objects
	UPROPERTY(BlueprintReadWrite)
	int TempChunkPoolInitialCount = 500;

private:
	TArray<AActor*> InvokerList;

	TArray<FTemporaryChunk*> TempChunkPool;
	FSpinLock TempChunkPoolLock;

	FTemporaryChunk* TempChunkOriginal = nullptr;

public:
	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type EndType) override;

	void Tick(float DeltaTime) override;

	void RegisterInvoker(AActor* Actor);
	void DeregisterInvoker(AActor* Actor);

	void InitGlobalManager();
	void DeinitGlobalManager();

	void RegisterVoxelWorld(AVoxelWorld* VoxelWorld);
	void DeregisterVoxelWorld(AVoxelWorld* VoxelWorld);

	inline FTemporaryChunk* NewTempChunk();
	FTemporaryChunk* GetNewTempChunk(UVoxelChunk* OwnerChunk);
	void ReleaseTempChunk(FTemporaryChunk* TempChunk);;

	//Calculates minimum distance to invokers
	float GetDistanceToInvokers(FVector WorldPos);
};