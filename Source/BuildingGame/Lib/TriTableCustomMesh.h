// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class BUILDINGGAME_API TriTableCustomMesh
{
public:
	TriTableCustomMesh();
	void Triangle(FVector cellPos, class APartBuilder* parent, float cellDistance/*, int cellAmount, int cellHeightAmount*/);

private:
	//bool VertexWithinChunkBounds(FVector vertexPos, float cellDistance, int cellAmount, int cellHeightAmount);
	FVector InterpolateVerts(FVector4 v1, FVector4 v2);	//interpolates vertices
};
