#pragma once

#include "VoxelChunk.h"
#include "TestVoxel.h"
#include "VoxelWorldGen.generated.h"

class UVoxelChunk;
class IVoxelDataAccessor;

UCLASS()
class TESTVOXEL_API UVoxelWorldGenerator : public UObject
{
	GENERATED_BODY()

public:
	UVoxelWorldGenerator()
	{}

	virtual ~UVoxelWorldGenerator()
	{}

	//Called at worldgen initialization
	virtual void Setup()
	{

	};

	virtual void GenerateWorld(UVoxelChunk* Chunk, IVoxelDataAccessor* DataAccessor)
	{
		unimplemented();
	};
};

UCLASS()
class TESTVOXEL_API UVoxelEmptyWorldGenerator : public UVoxelWorldGenerator
{
	GENERATED_BODY()

public:
	virtual void GenerateWorld(UVoxelChunk* Chunk, IVoxelDataAccessor* DataAccessor) override;
};

UCLASS()
class TESTVOXEL_API UVoxelFlatWorldGenerator : public UVoxelWorldGenerator
{
	GENERATED_BODY()

public:
	virtual void GenerateWorld(UVoxelChunk* Chunk, IVoxelDataAccessor* DataAccessor) override;
};
