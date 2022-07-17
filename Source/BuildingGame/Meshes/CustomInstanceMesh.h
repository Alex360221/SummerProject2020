// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class BUILDINGGAME_API CustomInstanceMesh
{
public:
	CustomInstanceMesh();

    class InstanceCustomMeshData CreateMesh(TMap<FVector, float> cells, float xSize, float ySize, float zSize, FVector Center, float cellSpacing, int objectCount); //this will create a mesh::: TODO ad a return
private:
    TArray<FTransform> CreateLODs(TArray<FVector> cubePositions, int LOD);
};
