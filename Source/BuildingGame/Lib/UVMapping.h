// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class BUILDINGGAME_API UVMapping
{
public:
	UVMapping();
    TArray<FVector2D> UVUnwrap(TArray<FVector> vertices, TArray<int> triangles, float size);    //this will uv unwrap a mesh
private:
    TArray<FVector2D> UVUnwrapTri(TArray<FVector> vertices, TArray<int> triangles);   //this will uv unwrap the current triangle
    bool AreAllPointsNegative(FVector A, FVector B, FVector C);   //this works out if all the points are negative
    int AreTwoPointsTheSame(FVector A, FVector B, FVector C);  //this will check if two points have the same x and y
};
