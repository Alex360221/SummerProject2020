// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultMeshes.h"

DefaultMeshes::DefaultMeshes()
{
	//VerticesSquareMesh.Append()
		
}

TArray<FVector> DefaultMeshes::GetVerticesSquareMesh(float size)
{
	//4 VERTICES PER SQUARE NOT OPTIMISED AS SHARING VERICES WON'T WORK FOR UV UNWRAPPING
	float iSize = size * -1;
	TArray<FVector> rv;
	rv.Add(FVector(iSize, iSize, iSize));//bottom
	rv.Add(FVector(size, iSize, iSize));
	rv.Add(FVector(size, size, iSize));
	rv.Add(FVector(iSize, size, iSize));

	rv.Add(FVector(iSize, iSize, size));//top
	rv.Add(FVector(size, iSize, size));
	rv.Add(FVector(size, size, size));
	rv.Add(FVector(iSize, size, size));

	rv.Add(FVector(iSize, iSize, iSize));//front side
	rv.Add(FVector(iSize, iSize, size));
	rv.Add(FVector(size, iSize, size));
	rv.Add(FVector(size, iSize, iSize));

	rv.Add(FVector(size, size, iSize));//front back
	rv.Add(FVector(size, size, size));
	rv.Add(FVector(iSize, size, size));
	rv.Add(FVector(iSize, size, iSize));

	rv.Add(FVector(size, iSize, iSize));//left side
	rv.Add(FVector(size, iSize, size));
	rv.Add(FVector(size, size, size));
	rv.Add(FVector(size, size, iSize));

	rv.Add(FVector(iSize, size, iSize));//right side
	rv.Add(FVector(iSize, size, size));
	rv.Add(FVector(iSize, iSize, size));
	rv.Add(FVector(iSize, iSize, iSize));
	return rv;
}

TArray<int> DefaultMeshes::GetTrianglesSquareMesh()
{
	TArray<int> ri;
	ri.Add(0);ri.Add(1);ri.Add(2);ri.Add(2);ri.Add(3);ri.Add(0);//bottome
	ri.Add(4);ri.Add(7);ri.Add(6);ri.Add(6);ri.Add(5);ri.Add(4);//top

	ri.Add(8);ri.Add(9);ri.Add(10);ri.Add(10);ri.Add(11);ri.Add(8);//front side
	ri.Add(12);ri.Add(13);ri.Add(14);ri.Add(14);ri.Add(15);ri.Add(12);//front back

	ri.Add(16);ri.Add(17);ri.Add(18);ri.Add(18);ri.Add(19);ri.Add(16);//left side
	ri.Add(20);ri.Add(21);ri.Add(22);ri.Add(22);ri.Add(23);ri.Add(20);

	return ri;
}

