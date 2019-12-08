#include "VoxelMeshComponentWrapper.h"
#include "VoxelWorld.h"

FRMCWrapper::FRMCWrapper(AVoxelWorld* World)
	: VoxelWorld(World)
{
	RMC = NewObject<URuntimeMeshComponent>(World);
	RMC->SetupAttachment(World->GetRootComponent());
	RMC->RegisterComponent();
}

FRMCWrapper::~FRMCWrapper()
{
	RMC->DestroyComponent();
}

void FRMCWrapper::UpdateMeshData(FVoxelPolygonizedData& Data)
{
	const int Num = Data.Sections.Num();
	for (int Index = 0; Index < Num; Index++)
	{
		auto& Section = Data.Sections[Index];
		RMC->SetMaterial(Index, Section.Material);
		RMC->CreateMeshSection(Index, Section.Vertices, Section.Tris, Section.Normals, Section.UVs, Section.Colors, TArray<FRuntimeMeshTangent>(), Section.bHasCollision);
	}
}

void FRMCWrapper::SetPosition(const FVector WorldPos)
{
	RMC->SetRelativeLocation(WorldPos);
}

void FRMCWrapper::SetVisiblity(bool bIsVisible)
{
	RMC->SetVisibility(bIsVisible);
}
