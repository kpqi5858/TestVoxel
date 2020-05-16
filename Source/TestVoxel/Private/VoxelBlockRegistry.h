#pragma once

#include "CoreMinimal.h"
#include "TestVoxel.h"


#define GETBLOCKREG() FBlockRegistry::GetInstance_Ptr()

//Templated GetBlock, accepts FName, FString, const TCHAR*, uint16(Index)
#define GETBLOCK(x) GETBLOCKREG()->GetBlock(x)

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

	bool IsValidIndex(const uint16 Index)
	{
		return UniqueIndices.IsValidIndex(Index);
	}

	UVoxelBlock* GetBlockByIndex(const uint16 Index)
	{
		if (!ensureMsgf(IsValidIndex(Index), TEXT("Invalid index in GetBlockByIndex : %d"), Index))
		{
			return GetBlockByIndex(0);
		}

		return UniqueIndices[Index];
	}

	UVoxelBlock* GetBlockInternal(FName Name)
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

	template<typename T>
	UVoxelBlock* GetBlock(T Name)
	{
		static_assert(false, "GetBlock with that type is not implemented");
		return nullptr;
	}

	template<>
	UVoxelBlock* GetBlock<>(const FName Name)
	{
		return GetBlockInternal(Name);
	}

	template<>
	UVoxelBlock* GetBlock<>(const TCHAR* Name)
	{
		//Avoid unnecessary allocations on FName
		const FName Nam = FName(Name, EFindName::FNAME_Find);
		if (Nam != NAME_None)
		{
			return GetBlockInternal(Nam);
		}
		else
		{
			UE_LOG(LogTestVoxel, Error, TEXT("Unknown block name string : %s"), Name);
			return GetBlockByIndex(0);
		}
	}

	template<>
	UVoxelBlock* GetBlock<>(const FString Name)
	{
		return GetBlock(*Name);
	}

	template<>
	UVoxelBlock* GetBlock<>(const uint16 Id)
	{
		return GetBlockByIndex(Id);
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