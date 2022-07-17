// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkMeshCreationThread.h"
#include "Chunk.h"
#include "Lib/TriTable.h"

ChunkMeshCreationThread::ChunkMeshCreationThread()
{
}

ChunkMeshCreationThreading::ChunkMeshCreationThreading(class AChunk* chunkRef, int cellHeightAmount, int cellAmount, float cellDistance)
{
	this->chunkRef = chunkRef;
	this->cellHeightAmount = cellHeightAmount;
	this->cellAmount = cellAmount;
	this->cellDistance = cellDistance;
}

void ChunkMeshCreationThreading::DoWork()
{
	FVector cellPos = FVector::ZeroVector;
	for (int z = 0; z < cellAmount - 1; z++) //z - height loop
	{
		//GLog->Log("Z");
		cellPos.Z = z * cellDistance;	//set the Z pos of the cell
		//float xCellDistance = cellDistance;
		for (int x = 0; x < cellAmount - 1 /*chunkRef->GetXAmountOfVertices(x, &xCellDistance)*/; x++) // x - x loop
		{
			//GLog->Log("X");
			cellPos.X = x * cellDistance;	//sets the x pos of the cell
			//float yCellDistance = cellDistance;
			for (int y = 0; y < cellAmount -1/*chunkRef->GetYAmountOfVertices(y, &yCellDistance)*/; y++) //y - y loop
			{
				//GLog->Log("Y");
				cellPos.Y = y * cellDistance;	//sets the y pos of the cell
				//chunkRef->GetXCellDistance(y, &xCellDistance);
				//chunkRef->GetYCellDistance(x, &yCellDistance);
				//cellPos.X = x * xCellDistance;
				//cellPos.Y = y * xCellDistance;

				//if (cellPos.X <= 450 && cellPos.Y <= 450)
				//{
					TriTable Tri;
					Tri.Triangle(0, cellPos, chunkRef, FVector(cellDistance, cellDistance, cellDistance), cellAmount, cellHeightAmount);
				//}
			}
		}
	}
	chunkRef->CreatedMesh = true;
}

