#pragma once

#include "CoreMinimal.h"
#include "TestVoxel.h"

//Templated GetBlock, accepts FName, FString, const TCHAR*, uint16(Index)
#define GETBLOCK(x) FBlockRegistry::GetInstance_Ptr()->GetBlock(x)

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


	UVoxelBlock* GetBlockByIndex(const uint16 Index)
	{
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
			return GetBlock(Nam);
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

	/*
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
	*/

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