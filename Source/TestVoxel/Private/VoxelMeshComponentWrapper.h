#pragma once

#include "VoxelData.h"
#include "RuntimeMeshComponent.h"
#include "Providers/RuntimeMeshProviderStatic.h"
#include "ProceduralMeshComponent.h"

class AVoxelWorld;

//Interface?
class TESTVOXEL_API FMeshComponentWrapper
{
public:
	virtual ~FMeshComponentWrapper()
	{};

	virtual void UpdateMeshData(FVoxelPolygonizedData& Data) = 0;
	virtual void SetPosition(const FVector WorldPos) = 0;
	virtual void SetVisiblity(bool bIsVisible) = 0;
};

//Wrapper for RuntimeMeshComponent
class TESTVOXEL_API FRMCWrapper : public FMeshComponentWrapper
{
public:
	FRMCWrapper(AVoxelWorld* World);
	~FRMCWrapper() override;

	AVoxelWorld* VoxelWorld;
	URuntimeMeshComponent* RMC;

	URuntimeMeshProviderStatic* StaticProvider;

	virtual void UpdateMeshData(FVoxelPolygonizedData& Data) override;
	virtual void SetPosition(const FVector WorldPos) override;
	virtual void SetVisiblity(bool bIsVisible) override;
};


//Wrapper for ProceduralMeshComponent
class TESTVOXEL_API FPMCWrapper : public FMeshComponentWrapper
{
public:
	FPMCWrapper(AVoxelWorld* World);
	~FPMCWrapper() override;

	AVoxelWorld* VoxelWorld;
	UProceduralMeshComponent* PMC;

	virtual void UpdateMeshData(FVoxelPolygonizedData& Data) override;
	virtual void SetPosition(const FVector WorldPos) override;
	virtual void SetVisiblity(bool bIsVisible) override;
};

