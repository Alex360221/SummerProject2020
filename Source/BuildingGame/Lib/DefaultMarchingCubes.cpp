// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultMarchingCubes.h"
#include <BuildingGame/PartBuilder.h>
#include "TriTableCustomMesh.h"

DefaultMarchingCubes::DefaultMarchingCubes()
{
}

TMap<FVector,float> DefaultMarchingCubes::CreateCells(float xSize, float ySize, float zSize, float cellSpacing)
{
	TMap<FVector, float> rCells;
	int hX = xSize / cellSpacing;
	int lX = (xSize * -1) / cellSpacing - 1;
	int hY = ySize / cellSpacing;
	int lY = (ySize * -1) / cellSpacing - 1;
	int hZ = zSize / cellSpacing;
	int lZ = (zSize * -1) / cellSpacing - 1;
	FVector cellPos;
	for (int z = lZ; z <= hZ;z++)
	{
		//z loop
		cellPos.Z = z * cellSpacing;
		for (int x = lX;x <= hX;x++)
		{
			//y loop
			cellPos.X = x * cellSpacing;
			for (int y = lY;y <= hY;y++)
			{
				cellPos.Y = y * cellSpacing;
				rCells.Add(cellPos,0);
			}
		}
	}
	return rCells;
}

TMap<FVector, float> DefaultMarchingCubes::FillCells_Cube(TMap<FVector, float> cells, float xSize, float ySize, float zSize,float cellSpacing)
{
	int hX = xSize;
	int lX = (xSize * -1) - 1;
	int hY = ySize;
	int lY = (ySize * -1) - 1;
	int hZ = zSize;
	int lZ = (zSize * -1) - 1;

	FVector cellPos;
	for (int z = lZ; z < hZ;z++)
	{
		//z loop
		cellPos.Z = z * cellSpacing;
		for (int x = lX;x < hX;x++)
		{
			//y loop
			cellPos.X = x * cellSpacing;
			for (int y = lY;y < hY;y++)
			{
				cellPos.Y = y * cellSpacing;
				cells.Emplace(cellPos, 1);
			}
		}
	}
	return cells;
}

CreateCellsThreading::CreateCellsThreading(float xSize, float ySize, float zSize, float newCellSpacing, AActor* parent)
{
	parentRef = Cast<APartBuilder>(parent);
	cellSpacing = newCellSpacing;
	hX = xSize / cellSpacing;
	lX = (xSize * -1) / cellSpacing - 1;
	hY = ySize / cellSpacing;
	lY = (ySize * -1) / cellSpacing - 1;
	hZ = zSize / cellSpacing;
	lZ = (zSize * -1) / cellSpacing - 1;

}

void CreateCellsThreading::DoWork()
{
	if (parentRef != nullptr)
	{
		FVector cellPos;
		for (int z = lZ; z < hZ;z++)
		{
			//z loop
			cellPos.Z = z * cellSpacing;
			for (int x = lX;x < hX;x++)
			{
				//y loop
				cellPos.X = x * cellSpacing;
				for (int y = lY;y < hY;y++)
				{
					cellPos.Y = y * cellSpacing;
					parentRef->AddCell(cellPos, 0);
				}
			}
		}
		parentRef->CreateCells = true;
	}
	
	//return rCells;
}

FillCells_CubeThreading::FillCells_CubeThreading(float xSize, float ySize, float zSize, float newCellSpacing, AActor* parent)
{
	parentRef = Cast<APartBuilder>(parent);
	cellSpacing = newCellSpacing;
	hX = xSize / cellSpacing;
	lX = (xSize * -1) / cellSpacing;
	hY = ySize / cellSpacing;
	lY = (ySize * -1) / cellSpacing;
	hZ = zSize / cellSpacing;
	lZ = (zSize * -1) / cellSpacing;
}

void FillCells_CubeThreading::DoWork()
{
	FVector cellPos;
	for (int z = lZ; z <= hZ;z++)
	{
		//z loop
		cellPos.Z = z * cellSpacing;
		for (int x = lX;x <= hX;x++)
		{
			//y loop
			cellPos.X = x * cellSpacing;
			for (int y = lY;y <= hY;y++)
			{
				cellPos.Y = y * cellSpacing;
				parentRef->UpdateCell(cellPos, 1);
			}
		}
	}
	parentRef->FilledCells = true;
}

CreateMeshThreading::CreateMeshThreading(float xSize, float ySize, float zSize, float newCellSpacing, AActor* parent)
{
	parentRef = Cast<APartBuilder>(parent);
	cellSpacing = newCellSpacing;
	hX = xSize / cellSpacing;
	lX = (xSize * -1) / cellSpacing - 1;
	hY = ySize / cellSpacing;
	lY = (ySize * -1) / cellSpacing - 1;
	hZ = zSize / cellSpacing;
	lZ = (zSize * -1) / cellSpacing - 1;
}

void CreateMeshThreading::DoWork()
{
	if (parentRef != nullptr)
	{
		FVector cellPos;
		for (int z = lZ; z <= hZ;z++)
		{
			//z loop
			cellPos.Z = z * cellSpacing;
			for (int x = lX;x <= hX;x++)
			{
				//y loop
				cellPos.X = x * cellSpacing;
				for (int y = lY;y <= hY;y++)
				{
					cellPos.Y = y * cellSpacing;
					TriTableCustomMesh tri;
					tri.Triangle(cellPos, parentRef, cellSpacing);
				}
			}
		}
		parentRef->CreatedMesh = true;
	}
}
