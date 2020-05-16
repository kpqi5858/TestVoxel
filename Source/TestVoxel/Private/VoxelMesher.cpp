#include "VoxelMesher.h"
#include "VoxelDataAccessor.h"
#include "VoxelChunk.h"
#include "VoxelDataStorage.h"
#include "VoxelBlock.h"

FVoxelMesherDefault::FVoxelMesherDefault(UVoxelChunk* Chunk, FVoxelMesherDefaultSettings Settings)
	: FVoxelMesher(Chunk), Settings(Settings)
{

}

void FVoxelMesherDefault::Polygonize(FVoxelPolygonizedData& MeshDataOut)
{
	IVoxelDataAccessor* DataAccessor = CreateDataAccessor();
	
	const FIntVector ChunkPos = Chunk->ChunkIndex;
	const FIntVector ChunkVoxelPos = Chunk->GetMinPos();

	FVoxelBlock* InternalArray = DataAccessor->GetInternalArray(ChunkPos);

	for (int X = 0; X < VOX_CHUNKSIZE; X++)
	for (int Y = 0; Y < VOX_CHUNKSIZE; Y++)
	for (int Z = 0; Z < VOX_CHUNKSIZE; Z++)
	{
		const FIntVector LocalPos = FIntVector(X, Y, Z);
		const FIntVector VoxelPos = ChunkVoxelPos + LocalPos;
		const FVoxelBlock& CurrentBlock = InternalArray[VOX_AI(X, Y, Z)];

		const bool bInChunkBorder = FVoxelUtilities::IsInChunkBorder(LocalPos);

		const UVoxelBlock* VoxelBlock = CurrentBlock.GetVoxelBlock();

		//If material is null, this means it is not visible and should never be polygonized
		if (VoxelBlock->Material == nullptr) continue;

		//If bDebugPolygonize enabled, use SolidDefault(Id always 1) block only
		FVoxelPolygonizedDataSection& ThisSection = MeshDataOut.GetSection(Settings.bDebugPolygonize ? 1 : CurrentBlock.Type);

		for (int FaceNum = 0; FaceNum < VOX_FACENUM; FaceNum++)
		{
			const EBlockFace Face = static_cast<EBlockFace>(FaceNum);
			const FIntVector TargetLocalPos = LocalPos + FVoxelUtilities::GetFaceOffset(Face);
			const FIntVector TargetVoxelPos = ChunkVoxelPos + TargetLocalPos;

			const bool IsRefBlockInsideCunk = VOX_AI_ISVALID(TargetLocalPos.X, TargetLocalPos.Y, TargetLocalPos.Z);

			bool bOccludeThisFace = false;

			if (!IsRefBlockInsideCunk && Settings.bFaceOcclusionBorders)
			{
				unimplemented();
			}
			if (IsRefBlockInsideCunk && Settings.bFaceOcclusionLocal)
			{
				const FVoxelBlock& TargetBlock = InternalArray[VOX_AI(TargetLocalPos.X, TargetLocalPos.Y, TargetLocalPos.Z)];

				bOccludeThisFace = TargetBlock.GetVoxelBlock()->VisiblityType == VoxelBlock->VisiblityType;
			}

			if (!bOccludeThisFace)
			{
				CreateFace(X, Y, Z, Face, CurrentBlock.Color, ThisSection);
			}
			else
			{

			}
		}
	}
}

IVoxelDataAccessor* FVoxelMesherDefault::CreateDataAccessor()
{
	if (Settings.bFaceOcclusionBorders)
	{
		unimplemented();
		return nullptr;
	}
	else
	{
		return Chunk->DataStorage;
	}
}

inline void FVoxelMesherDefault::CreateFace(const int& X, const int& Y, const int& Z, const EBlockFace& Face, const FColor& Color, FVoxelPolygonizedDataSection& ThisSection)
{
	const int TriIndex = ThisSection.Vertices.Num();

	const float VoxSize = Settings.VoxelSize;

	const FVector BoxVerts[8] =
	{
		FVector(0, 1, 1),
		FVector(1, 1, 1),
		FVector(1, 0, 1),
		FVector(0, 0, 1),
		FVector(0, 1, 0),
		FVector(1, 1, 0),
		FVector(1, 0, 0),
		FVector(0, 0, 0)
	};

	auto VertexBuilder = [&](const FVector& V, const FVector& N, const FVector2D UV)
	{
		ThisSection.Vertices.Add((V + FVector(X, Y, Z)) * VoxSize);
		if (Settings.bCreateNormals)		ThisSection.Normals.Add(N);
		if (Settings.bCreateUVs)			ThisSection.UVs.Add(UV);
		if (Settings.bCreateVertexColors)	ThisSection.Colors.Add(Color);
	};

	FVector Normal;

	switch (Face)
	{
		//Pos Z
	case EBlockFace::TOP:
	{
		Normal = FVector(0.0f, 0.0f, 1.0f);
		VertexBuilder(BoxVerts[0], Normal, FVector2D(0.0f, 0.0f));
		VertexBuilder(BoxVerts[1], Normal, FVector2D(0.0f, 1.0f));
		VertexBuilder(BoxVerts[2], Normal, FVector2D(1.0f, 1.0f));
		VertexBuilder(BoxVerts[3], Normal, FVector2D(1.0f, 0.0f));
		break;
	}
	//Neg Z
	case EBlockFace::BOTTOM:
	{
		Normal = FVector(0.0f, 0.0f, -1.0f);
		VertexBuilder(BoxVerts[7], Normal, FVector2D(0.0f, 0.0f));
		VertexBuilder(BoxVerts[6], Normal, FVector2D(0.0f, 1.0f));
		VertexBuilder(BoxVerts[5], Normal, FVector2D(1.0f, 1.0f));
		VertexBuilder(BoxVerts[4], Normal, FVector2D(1.0f, 0.0f));
		break;
	}
	//Neg Y
	case EBlockFace::LEFT:
	{
		Normal = FVector(0.0f, -1.0f, 0.0f);
		VertexBuilder(BoxVerts[7], Normal, FVector2D(0.0f, 0.0f));
		VertexBuilder(BoxVerts[3], Normal, FVector2D(0.0f, 1.0f));
		VertexBuilder(BoxVerts[2], Normal, FVector2D(1.0f, 1.0f));
		VertexBuilder(BoxVerts[6], Normal, FVector2D(1.0f, 0.0f));
		break;
	}
	//Pos Y
	case EBlockFace::RIGHT:
	{
		Normal = FVector(0.0f, 1.0f, 0.0f);
		VertexBuilder(BoxVerts[5], Normal, FVector2D(0.0f, 0.0f));
		VertexBuilder(BoxVerts[1], Normal, FVector2D(0.0f, 1.0f));
		VertexBuilder(BoxVerts[0], Normal, FVector2D(1.0f, 1.0f));
		VertexBuilder(BoxVerts[4], Normal, FVector2D(1.0f, 0.0f));
		break;
	}
	//Pos X
	case EBlockFace::FRONT:
	{
		Normal = FVector(1.0f, 0.0f, 0.0f);
		VertexBuilder(BoxVerts[6], Normal, FVector2D(0.0f, 0.0f));
		VertexBuilder(BoxVerts[2], Normal, FVector2D(0.0f, 1.0f));
		VertexBuilder(BoxVerts[1], Normal, FVector2D(1.0f, 1.0f));
		VertexBuilder(BoxVerts[5], Normal, FVector2D(1.0f, 0.0f));
		break;
	}
	//Neg X
	case EBlockFace::BACK:
	{
		Normal = FVector(-1.0f, 0.0f, 0.0f);
		VertexBuilder(BoxVerts[4], Normal, FVector2D(0.0f, 0.0f));
		VertexBuilder(BoxVerts[0], Normal, FVector2D(0.0f, 1.0f));
		VertexBuilder(BoxVerts[3], Normal, FVector2D(1.0f, 1.0f));
		VertexBuilder(BoxVerts[7], Normal, FVector2D(1.0f, 0.0f));
		break;
	}
	default:
		check(false);
	}

	ThisSection.Tris.Add(TriIndex);
	ThisSection.Tris.Add(TriIndex + 1);
	ThisSection.Tris.Add(TriIndex + 3);
	ThisSection.Tris.Add(TriIndex + 1);
	ThisSection.Tris.Add(TriIndex + 2);
	ThisSection.Tris.Add(TriIndex + 3);
}
