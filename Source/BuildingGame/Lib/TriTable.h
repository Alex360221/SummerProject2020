// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class BUILDINGGAME_API TriTable
{
public:
	TriTable();
	void Triangle(int meshType, FVector cellPos, class AChunk* chunkRef, FVector cellDistance, int cellAmount, int cellHeightAmount);

private:
	bool VertexWithinChunkBounds(FVector vertexPos, FVector cellDistance, int cellAmount, int cellHeightAmount);
	FVector InterpolateVerts(FVector4 v1, FVector4 v2);	//interpolates vertices
	bool VertexDistanceCheck(FVector vert1, FVector vert2, FVector vert3);

};
