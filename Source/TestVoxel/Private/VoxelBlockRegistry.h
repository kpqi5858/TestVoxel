#pragma once

#include "CoreMinimal.h"
#include "TestVoxel.h"

//Parameter is FName, FString
#define GETBLOCK(x) FBlockRegistry::GetInstance_Ptr()->GetBlock(x)
//Parameter is TCHAR
#define GETBLOCK_T(x) FBlockRegistry::GetInstance_Ptr()->GetBlock(FName(x))
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

	UVoxelBlock* GetBlockByIndex(uint16 Index)
	{
		return UniqueIndices[Index];
	}

	UVoxelBlock* GetBlock(FName Name)
	{
		UVoxelBlock** Find = BlockInstanceRegistry.Find(Name);
		if (Find)
		{
			return *Find;
		}
		else
		{
			UE_LOG(LogTestVoxel, Error, TEXT("Unknown block name : %s"), *Name.ToString());
			return GetBlockByIndex(0);
		}
	}
	UVoxelBlock* GetBlock(FString Name)
	{
		FName Nam = FName(*Name, EFindName::FNAME_Find);
		if (Nam != NAME_None)
		{
			return GetBlock(Nam);
		}
		else
		{
			UE_LOG(LogTestVoxel, Error, TEXT("Unknown block name string : %s"), *Name);
			return GetBlockByIndex(0);
		}
	}

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