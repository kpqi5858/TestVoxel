#pragma once

#include "VoxelData.h"
#include "RuntimeMeshComponent.h"

class AVoxelWorld;

class TESTVOXEL_API FMeshComponentWrapper
{
public:
	virtual void UpdateMeshData(FVoxelPolygonizedData& Data) = 0;
	virtual void SetPosition(const FVector WorldPos) = 0;
	virtual void SetVisiblity(bool bIsVisible) = 0;
};

class TESTVOXEL_API FRMCWrapper : public FMeshComponentWrapper
{
public:
	FRMCWrapper(AVoxelWorld* World);

	AVoxelWorld* VoxelWorld;
	URuntimeMeshComponent* RMC;


	virtual void UpdateMeshData(FVoxelPolygonizedData& Data) override;
	virtual void SetPosition(const FVector WorldPos) override;
	virtual void SetVisiblity(bool bIsVisible) override;
};
/*
class TESTVOXEL_API FPMCWrapper : public FMeshComponentWrapper
{

};
*/
