#include "VoxelWorldGen.h"
#include "VoxelChunk.h"
#include "VoxelDataAccessor.h"

void UVoxelEmptyWorldGenerator::GenerateWorld(UVoxelChunk* Chunk, IVoxelDataAccessor* DataAccessor)
{
	const FIntVector BasePos = Chunk->GetMinPos();

	for (int X = 0; X < VOX_CHUNKSIZE; X++)
	for (int Y = 0; Y < VOX_CHUNKSIZE; Y++)
	for (int Z = 0; Z < VOX_CHUNKSIZE; Z++)
	{
		const FIntVector Pos = BasePos + FIntVector(X, Y, Z);
		DataAccessor->SetBlock(Pos, FVoxelBlock());
	}
}

void UVoxelFlatWorldGenerator::GenerateWorld(UVoxelChunk* Chunk, IVoxelDataAccessor* DataAccessor)
{
	const FIntVector BasePos = Chunk->GetMinPos();

	const UVoxelBlock* AirBlock = GETBLOCK(TEXT("Air"));
	const UVoxelBlock* BlockToUse = GETBLOCK(BlockName);

	for (int X = 0; X < VOX_CHUNKSIZE; X++)
	for (int Y = 0; Y < VOX_CHUNKSIZE; Y++)
	for (int Z = 0; Z < VOX_CHUNKSIZE; Z++)
	{
		const FIntVector VoxelPos = BasePos + FIntVector(X, Y, Z);
		DataAccessor->SetBlock(VoxelPos, FVoxelBlock(VoxelPos.Z <= 0 ? BlockToUse : AirBlock));
	}
}
