#pragma once

#include "CoreMinimal.h"
#include "TestVoxel.h"

//Parameter is FName, FString, or FText
#define GETBLOCK(x) FBlockRegistry::GetInstance_Ptr()->GetBlock(x)
//Parameter is from TEXT(x) macro
#define GETBLOCK_T(x) FBlockRegistry::GetInstance_Ptr()->GetBlock_(x)
//Parameter is const char*
#define GETBLOCK_C(x) GETBLOCK_T(TEXT(x))
//Parameter is unique index
#define GETBLOCK_INDEX(x) FBlockRegistry::GetInstance_Ptr()->GetBlockByIndex(x)

class UVoxelBlock;

class TESTVOXEL_API FBlockRegistryInstance
{
private:
	TMap<FName, UVoxelBlock*> BlockInstanceRegistry;
	TArray<UVoxelBlock*> UniqueIndices;

public:
	FBlockRegistryInstance();
	~FBlockRegistryInstance();

	bool bIsInitialized = false;

	void Setup();
	void Reset();

	UVoxelBlock* GetBlock(FName Name);
	UVoxelBlock* GetBlock(FText Name);
	UVoxelBlock* GetBlock(FString Name);

	UVoxelBlock* GetBlock_(FName Name)
	{
		return GetBlock(Name);
	}

	UVoxelBlock* GetBlockByIndex(uint16 Index);
};

class TESTVOXEL_API FBlockRegistry
{
private:
	static TWeakPtr<FBlockRegistryInstance> InstancePtr;
	static FBlockRegistryInstance* InstancePtrRaw;

public:
	//Iterates classes that inherits UBlock
	static void FindVoxelBlocks(TArray<TWeakObjectPtr<UClass>>& BlockClassesOut);

	//AVoxelWorld should have this in member
	static TSharedPtr<FBlockRegistryInstance> GetInstance();

	//Macro only
	static FBlockRegistryInstance* GetInstance_Ptr();
};