#pragma once

#include "CoreMinimal.h"
#include "Materials/MaterialInstance.h"

#include "VoxelBlock.generated.h"

UCLASS(Blueprintable, Abstract)
class TESTVOXEL_API UVoxelBlock : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FName RegistryName;

	//If false, no collision will be created (Give it proper VisiblityType to work)
	UPROPERTY(EditDefaultsOnly)
	bool bDoCollisions = true;

	//Is this block treated as an empty space?
	UPROPERTY(EditDefaultsOnly)
	bool bIsEmptyBlock = false;

	//Blocks with same VisitlibyType will occlude each other
	UPROPERTY(EditDefaultsOnly)
	int VisiblityType = 0;

	//If none(nullptr), the block will not visible
	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* Material = nullptr;

	//Overrides the type id if not negative number, Do not write high values
	UPROPERTY(EditDefaultsOnly)
	int32 OverrideTypeId = -1;

	//Don't register this block?
	UPROPERTY(EditDefaultsOnly)
	bool bDontRegister = false;


	UPROPERTY(BlueprintReadOnly)
	bool bIsRegistered = false;

	//Unique type id, DON'T SAVE THIS VALUE, Save RegistryName instead
	uint16 TypeId = 0;


	UFUNCTION(BlueprintCallable)
	int GetTypeId() const
	{
		return TypeId;
	}
};


//Default blocks

UCLASS()
class TESTVOXEL_API UDefaultBlockAir : public UVoxelBlock
{
	GENERATED_BODY()

public:
	UDefaultBlockAir()
	{
		RegistryName = TEXT("Air");
		bIsEmptyBlock = true;
		bDoCollisions = false;
		VisiblityType = 0;
		OverrideTypeId = 0;
	}
};

UCLASS()
class TESTVOXEL_API UDefaultBlockSolid : public UVoxelBlock
{
	GENERATED_BODY()

public:
	UDefaultBlockSolid()
	{
		RegistryName = TEXT("SolidDefault");
		bDoCollisions = true;
		VisiblityType = 1;
		OverrideTypeId = 1;
		Material = UMaterial::GetDefaultMaterial(EMaterialDomain::MD_Surface);
	}
};