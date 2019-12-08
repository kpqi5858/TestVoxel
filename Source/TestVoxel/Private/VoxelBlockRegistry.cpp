#include "VoxelBlockRegistry.h"
#include "VoxelBlock.h"
#include "UObjectIterator.h"
#include "AssetRegistryModule.h"
#include "Engine/Blueprint.h"

TWeakPtr<FBlockRegistryInstance> FBlockRegistry::InstancePtr = TWeakPtr<FBlockRegistryInstance>();
FBlockRegistryInstance* FBlockRegistry::InstancePtrRaw = nullptr;

FBlockRegistryInstance::FBlockRegistryInstance()
{
	Setup();
}

FBlockRegistryInstance::~FBlockRegistryInstance()
{
	Reset();
}

void FBlockRegistryInstance::Setup()
{
	check(!bIsInitialized);

	TArray<TWeakObjectPtr<UClass>> BlockClasses;
	FBlockRegistry::FindVoxelBlocks(BlockClasses);

	const auto BlockClass = UVoxelBlock::StaticClass();

	for (auto& Class : BlockClasses)
	{
		UClass* RawClass = Class.Get();
		if (!RawClass) continue;

		check(RawClass->IsChildOf(BlockClass));

		UVoxelBlock* Block = NewObject<UVoxelBlock>(GetTransientPackage(), RawClass);

		if (Block->bDontRegister) continue;

		//Prevent it from gc
		Block->AddToRoot();

		FName BlockName = Block->RegistryName;
		if (!BlockName.IsValid())
		{
			UE_LOG(LogTestVoxel, Error, TEXT("%s : RegistryName is not valid"), *Block->GetClass()->GetName());
			continue;
		}
		if (BlockInstanceRegistry.Contains(BlockName))
		{
			UE_LOG(LogTestVoxel, Error, TEXT("%s : Duplicate RegistryName of %s"), *Block->GetClass()->GetName(), *BlockName.ToString());
			continue;
		}

		BlockInstanceRegistry.Add(BlockName, Block);
	}

	checkf(BlockInstanceRegistry.Num() < 65536, TEXT("UniqueIndex collision (There's too many blocks!!), BlockInstanceRegistry.Num() = %d"), BlockInstanceRegistry.Num());

	//TypeId Indexing

	TArray<UVoxelBlock*> BlocksArray;
	BlockInstanceRegistry.GenerateValueArray(BlocksArray);

	//Fill with nullptr first
	UniqueIndices.AddDefaulted(BlockInstanceRegistry.Num());

	//Deal with OverrideTypeId
	for (auto& Block : BlocksArray)
	{
		int OverrideTypeId = Block->OverrideTypeId;
		if (OverrideTypeId >= 0)
		{
			checkf(OverrideTypeId < 65536, TEXT("OverrideTypeId is higher than uint16 max."));

			//Need to resize? (Probably because too high OverrideTypeId)
			if (UniqueIndices.Num() <= OverrideTypeId)
			{
				UE_LOG(LogTestVoxel, Warning, TEXT("OverrideTypeId is higher than UniqueIndices.Num(). Resizing.."));
				UniqueIndices.AddDefaulted(OverrideTypeId - UniqueIndices.Num() + 1);
			}
			//Is there's collision?
			if (UniqueIndices[OverrideTypeId] != nullptr)
			{
				UE_LOG(LogTestVoxel, Error, TEXT("OverrideTypeId collision, OverrideTypeId = %d"), OverrideTypeId);

				//Will be automatically indexed
				Block->OverrideTypeId = -1;
				continue;
			}

			UniqueIndices[OverrideTypeId] = Block;
		}
	}

	//Index the other blocks

	int LastIndex = 0;

	for (auto& Block : BlocksArray)
	{
		if (Block->OverrideTypeId < 0)
		{
			//Iterate through UniqueIndices and find an empty(nullptr) slot
			while (true)
			{
				if (UniqueIndices[LastIndex] == nullptr)
				{
					//And assign it
					UniqueIndices[LastIndex] = Block;
					break;
				}
				LastIndex++;
			}
		}
	}

	//Finally, Assign TypeId
	for (int Index = 0; Index < BlocksArray.Num(); Index++)
	{
		//Should not happen in normal case, but too high OverrideTypeId, this can happen.
		if (UniqueIndices[Index] == nullptr) continue;

		UniqueIndices[Index]->TypeId = Index;
		UniqueIndices[Index]->bIsRegistered = true;
	}

	bIsInitialized = true;
}

void FBlockRegistryInstance::Reset()
{
	BlockInstanceRegistry.Empty();

	for (auto& Block : UniqueIndices)
	{
		if (Block)
		{
			//Now it can be gc-ed
			Block->RemoveFromRoot();
		}
	}

	UniqueIndices.Empty();

	bIsInitialized = false;
}



void FBlockRegistry::FindVoxelBlocks(TArray<TWeakObjectPtr<UClass>>& BlockClassesOut)
{
	const auto BlockClass = UVoxelBlock::StaticClass();

	TArray<UClass*> Objs;
	GetDerivedClasses(BlockClass, Objs);

	//Register native classes
	
	for (auto& Class : Objs)
	{
		check(Class->IsChildOf(BlockClass));

		EClassFlags Flags = Class->GetClassFlags();

		if ((Flags & EClassFlags::CLASS_Native) && !(Flags & EClassFlags::CLASS_Abstract))
		{
			BlockClassesOut.Add(Class);
		}
	}

	//Load, and register BP classes

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(FName("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TArray<FAssetData> BPAssets;
	AssetRegistry.GetAssetsByClass(UBlueprint::StaticClass()->GetFName(), BPAssets);

	for (auto& BPAssetData : BPAssets)
	{
		//BP VoxelBlock needs to end with _VB
		if (BPAssetData.AssetName.ToString().EndsWith(TEXT("_VB")))
		{
			UBlueprint* BPAsset = CastChecked<UBlueprint>(BPAssetData.GetAsset());
			
			UClass* BPClass = BPAsset->GeneratedClass;
			if (BPClass && BPClass->IsChildOf(BlockClass))
			{
				BlockClassesOut.Add(BPClass);
			}
		}
	}
}

TSharedPtr<FBlockRegistryInstance> FBlockRegistry::GetInstance()
{
	//FBlockRegistryInstance exists, return it
	if (InstancePtr.IsValid())
	{
		return InstancePtr.Pin();
	}
	else
	{
		//Else, make new one
		TSharedPtr<FBlockRegistryInstance> Inst = MakeShareable(new FBlockRegistryInstance());

		InstancePtr = Inst;
		InstancePtrRaw = Inst.Get();

		return Inst;
	}
}

FBlockRegistryInstance* FBlockRegistry::GetInstance_Ptr()
{
	checkf(InstancePtr.IsValid(), TEXT("GetInstance_Ptr called with no reference"));
	return InstancePtrRaw;
}
