// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkCreationThread.h"
#include "Chunk.h"
#include "Lib/TerrainCalculator.h"


ChunkCreationThread::ChunkCreationThread()
{
}

ChunkCreationThreading::ChunkCreationThreading(class AChunk* chunkRef,int cellHeightAmount,int cellAmount, float cellDistance, float surfaceHeight, int bSeed)
{	
	this->chunkRef = chunkRef;
	this->cellHeightAmount = cellHeightAmount;
	this->cellAmount = cellAmount;
	this->cellDistance = cellDistance;
	this->surfaceHeight = surfaceHeight;
	this->bSeed = bSeed;
	chunkPos = chunkRef->GetActorLocation();
}

void ChunkCreationThreading::DoWork()
{
	FVector cellPos = FVector::ZeroVector;
	for (int z = 0; z < cellAmount; z++) //z - height loop
	{
		cellPos.Z = z * cellDistance;	//set the Z pos of the cell
		//float xCellDistance = cellDistance;
		for (int x = 0; x < cellAmount;/*chunkRef->GetXAmountOfVertices(x, &xCellDistance)*/ x++) // x - x loop
		{
			cellPos.X = x * cellDistance;	//sets the x pos of the cell
			//float yCellDistance = cellDistance;
			for (int y = 0; y < cellAmount/*chunkRef->GetYAmountOfVertices(y, &yCellDistance)*/; y++) //y - y loop
			{
				cellPos.Y = y * cellDistance;	//sets the y pos of the cell				
				//chunkRef->AddCell(cellPos, Threading::CellDensity(cellPos,chunkPos,cellDistance, surfaceHeight,cellAmount,bSeed));
				float rf = 1;
				float surface = Threading::CellHeight(FVector(cellPos.X + chunkPos.X, cellPos.Y + chunkPos.Y, cellPos.Z), cellDistance, bSeed) * 10;
				surface = surface + (surfaceHeight * cellDistance);
				float cellHeight = cellPos.Z + chunkPos.Z;
				if (cellHeight != 0.f)
				{
					rf = surface / cellHeight;
					
					if (rf < 0) { rf *= -1; }
					if (rf > 1.6f) { chunkRef->SetCellColour(cellPos, FColor(87, 84, 84)); }	//stone
					else  if (rf > 1.2f) { chunkRef->SetCellColour(cellPos, FColor(87, 32, 25)); } //dirt
					else  if (rf > 0.9f) 
					{ 
						if(cellHeight > 600) { chunkRef->SetCellColour(cellPos, FColor(34, 139, 34)); }
						else { chunkRef->SetCellColour(cellPos, FColor(199, 182, 70)); } //sand
						
					}	//grass
					else  { chunkRef->SetCellColour(cellPos, FColor(87, 84, 84)); }	//stone
					if (rf > 1.5f) { rf = 1.5f; }					
				}
				else { chunkRef->SetCellColour(cellPos, FColor(87, 84, 84)); }
				if (cellHeight < 600 && rf <= 1.1f)
				{//set water
					chunkRef->AddWaterCell(cellPos, 1.5f);
				}
				else{ chunkRef->AddWaterCell(cellPos, 0.f); }

				chunkRef->AddCell(cellPos, rf);
			}
		}
	}
	chunkRef->CreatedCells = true;
}

float Threading::CellDensity(FVector cellPos, FVector chunkPos, float cellDistance, float surfaceHeight, float cellAmount, int bSeed)
{
	float rf = 1;
	float surface = CellHeight(FVector(cellPos.X + chunkPos.X, cellPos.Y + chunkPos.Y, cellPos.Z),cellDistance,bSeed) * 10;
	surface = surface + (surfaceHeight * cellDistance);
	float cellHeight = cellPos.Z + chunkPos.Z;
	if (cellHeight != 0.f)
	{
		rf = surface / cellHeight;
		
		

		if (rf < 0) { rf *= -1; }
		if (rf > 1.5f) { rf = 1.5f; }
		/*if (cellPos.Z == 0 || cellPos.Z == (cellAmount - 1 * cellDistance))
		{
			if (rf > 1) { rf = 1.1; }
		}*/
	}
	return rf;
}

float Threading::CellHeight(FVector cellPos, float cellDistance, int bSeed)
{
	TerrainCalculator TC;
	return TC.GetEnvironmentHeight(cellPos, bSeed);
}
