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

	//If false, no collision
	UPROPERTY(EditDefaultsOnly)
	bool bDoCollisions = true;

	//Blocks with same VisitlibyType will occlude each other
	UPROPERTY(EditDefaultsOnly)
	int VisiblityType = 0;

	//If nullptr, the block will not visible
	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* Material;

	//Overrides the type id if not negative number, Do not write high values
	UPROPERTY(EditDefaultsOnly)
	int32 OverrideTypeId = -1;

	//Don't register this block
	UPROPERTY(EditDefaultsOnly)
	bool bDontRegister = false;


	//Unique type id, DON'T SAVE THIS VALUE, Save RegistryName instead
	UPROPERTY(Transient, BlueprintReadOnly)
	uint16 TypeId = 0;

	UPROPERTY(Transient, BlueprintReadOnly)
	bool bIsRegistered = false;
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