#pragma once

#include "VoxelData.h"


class IVoxelDataAccessor;

//Polygonizer
class TESTVOXEL_API FVoxelMesher
{
public:
	FVoxelMesher(UVoxelChunk* Chunk)
		: Chunk(Chunk)
	{ }
	virtual ~FVoxelMesher()
	{ }

	UVoxelChunk* Chunk;

	virtual void Polygonize(FVoxelPolygonizedData& MeshDataOut)
	{
		unimplemented();
	}

};

struct FVoxelMesherDefaultSettings
{
	bool bCreateNormals = true;
	bool bCreateTangents = false;
	bool bCreateUVs = true;
	bool bCreateVertexColors = true;

	bool bFaceOcclusionLocal = true;
	//Reference near chunks to occlude the faces in the chunk's border
	bool bFaceOcclusionBorders = true;

	//Make all solid block to "SolidDefault" block. This will create only 1 section.
	bool bDebugPolygonize = false;

	float VoxelSize;
};

class TESTVOXEL_API FVoxelMesherDefault : public FVoxelMesher
{
public:
	FVoxelMesherDefault(UVoxelChunk* Chunk, FVoxelMesherDefaultSettings Settings);

	const FVoxelMesherDefaultSettings Settings;


	virtual void Polygonize(FVoxelPolygonizedData& MeshDataOut) override;

	IVoxelDataAccessor* CreateDataAccessor();

	inline void CreateFace(const int& X, const int& Y, const int& Z, const EBlockFace& Face, const FColor& Color, FVoxelPolygonizedDataSection& ThisSection);
};
