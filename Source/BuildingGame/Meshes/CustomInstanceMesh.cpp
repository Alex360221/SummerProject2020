// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomInstanceMesh.h"
#include <BuildingGame/MeshData/InstanceCustomMeshData.h>

CustomInstanceMesh::CustomInstanceMesh()
{

}

InstanceCustomMeshData CustomInstanceMesh::CreateMesh(TMap<FVector,float> cells,float xSize, float ySize, float zSize, FVector Center, float cellSpacing,int objectCount)
{
	InstanceCustomMeshData rMeshData;
	//check the cells and work out which ones need a instance
	int hX = xSize / cellSpacing;
	int lX = (xSize * -1) / cellSpacing;
	int hY = ySize / cellSpacing;
	int lY = (ySize * -1) / cellSpacing;
	int hZ = zSize / cellSpacing;
	int lZ = (zSize * -1) / cellSpacing;
	float isolevel = 0.9f;
	TArray<FVector> positions;
	TArray<int> instanceTypes;
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
				FVector key = cellPos;
				TArray<FVector4> cellPoints;
				//get all the different posistions around the center
				cellPoints.Add(FVector4(key.X, key.Y, key.Z, cells.FindRef(key)));
				key = FVector(cellPos.X + cellSpacing, cellPos.Y, cellPos.Z);
				cellPoints.Add(FVector4(key.X, key.Y, key.Z, cells.FindRef(key)));
				key = FVector(cellPos.X + cellSpacing, cellPos.Y, cellPos.Z + cellSpacing);
				cellPoints.Add(FVector4(key.X, key.Y, key.Z, cells.FindRef(key)));
				key = FVector(cellPos.X, cellPos.Y, cellPos.Z + cellSpacing);
				cellPoints.Add(FVector4(key.X, key.Y, key.Z, cells.FindRef(key)));
				key = FVector(cellPos.X, cellPos.Y + cellSpacing, cellPos.Z);
				cellPoints.Add(FVector4(key.X, key.Y, key.Z, cells.FindRef(key)));
				key = FVector(cellPos.X + cellSpacing, cellPos.Y + cellSpacing, cellPos.Z);
				cellPoints.Add(FVector4(key.X, key.Y, key.Z, cells.FindRef(key)));
				key = FVector(cellPos.X + cellSpacing, cellPos.Y + cellSpacing, cellPos.Z + cellSpacing);
				cellPoints.Add(FVector4(key.X, key.Y, key.Z, cells.FindRef(key)));
				key = FVector(cellPos.X, cellPos.Y + cellSpacing, cellPos.Z + cellSpacing);
				cellPoints.Add(FVector4(key.X, key.Y, key.Z, cells.FindRef(key)));
				//Negative check
				key = FVector(cellPos.X - cellSpacing, cellPos.Y, cellPos.Z);
				cellPoints.Add(FVector4(key.X, key.Y, key.Z, cells.FindRef(key)));
				key = FVector(cellPos.X - cellSpacing, cellPos.Y, cellPos.Z - cellSpacing);
				cellPoints.Add(FVector4(key.X, key.Y, key.Z, cells.FindRef(key)));
				key = FVector(cellPos.X, cellPos.Y, cellPos.Z - cellSpacing);
				cellPoints.Add(FVector4(key.X, key.Y, key.Z, cells.FindRef(key)));
				key = FVector(cellPos.X, cellPos.Y - cellSpacing, cellPos.Z);
				cellPoints.Add(FVector4(key.X, key.Y, key.Z, cells.FindRef(key)));
				key = FVector(cellPos.X - cellSpacing, cellPos.Y - cellSpacing, cellPos.Z);
				cellPoints.Add(FVector4(key.X, key.Y, key.Z, cells.FindRef(key)));
				key = FVector(cellPos.X - cellSpacing, cellPos.Y - cellSpacing, cellPos.Z - cellSpacing);
				cellPoints.Add(FVector4(key.X, key.Y, key.Z, cells.FindRef(key)));
				key = FVector(cellPos.X, cellPos.Y - cellSpacing, cellPos.Z - cellSpacing);
				cellPoints.Add(FVector4(key.X, key.Y, key.Z, cells.FindRef(key)));

				//works out the type of cube to display for now it doesn't do a good job at it
				int cubeIndex = 0;
				if (cellPoints[0].W >= isolevel) { cubeIndex +=1; }
				if (cellPoints[1].W >= isolevel) { cubeIndex += 1; }
				if (cellPoints[2].W >= isolevel) { cubeIndex += 1; }
				if (cellPoints[3].W >= isolevel) { cubeIndex += 1; }
				if (cellPoints[4].W >= isolevel) { cubeIndex += 1; }
				if (cellPoints[5].W >= isolevel) { cubeIndex += 1; }
				if (cellPoints[6].W >= isolevel) { cubeIndex += 1; }
				if (cellPoints[7].W >= isolevel) { cubeIndex += 1; }
				if (cellPoints[8].W >= isolevel) { cubeIndex += 1; }
				if (cellPoints[9].W >= isolevel) { cubeIndex += 1; }
				if (cellPoints[10].W >= isolevel) { cubeIndex += 1; }
				if (cellPoints[11].W >= isolevel) { cubeIndex += 1; }
				if (cellPoints[12].W >= isolevel) { cubeIndex += 1; }
				if (cellPoints[13].W >= isolevel) { cubeIndex += 1; }
				if (cellPoints[14].W >= isolevel) { cubeIndex += 1; }
			
				if (cellPoints[0].W >= isolevel && cubeIndex < 14)
				{
					//add a basic cube					
					positions.Add(cellPos - Center);
					instanceTypes.Add(0);
				}
			}
		}
	}
	rMeshData.SetInstanceCustomMeshData(positions, instanceTypes, objectCount);
	return rMeshData;
}

TArray<FTransform> CustomInstanceMesh::CreateLODs(TArray<FVector> cubePositions, int LOD)
{
	TArray<FTransform> rt;
	float X,Y,Z;
	X = cubePositions[0].X;
	Y = cubePositions[0].Y;
	Z = cubePositions[0].Z;
	int /*targetX, */targetY/*, targetZ */= LOD;
	int currentX = 0, currentY = 0, currentZ = 0;
	for (int i = 0; i < cubePositions.Num();i++)
	{
		if (cubePositions[i].Z == Z)
		{
			//The Z is the same
		}
		else
		{
			currentZ++;
		}

		if (cubePositions[i].X == X)
		{
			//The X is the same
		}
		else
		{
			currentX++;
			//check we don't have more than one curent y
		}

		if (cubePositions[i].Y == Y)
		{
			//The Y is the same
		}
		else
		{
			currentY++;
			if (currentY == targetY)
			{
				//reached target amount of points
				//find the mind point
				FVector dif = cubePositions[i - 1] - cubePositions[i];
				dif = dif / 2;
				FVector midPoint = cubePositions[i - 1] + dif;
				int scale = currentY;
				FTransform newPos;
				newPos.SetLocation(midPoint);
				newPos.SetScale3D(FVector(1, scale, 1));
				rt.Add(newPos);
			}
		}

		//Worked out calculations for this index 
		X = cubePositions[i].X;
		Y = cubePositions[i].Y;
		Z = cubePositions[i].Z;
	}

	return rt;
}

