// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class BUILDINGGAME_API SnappingPointsData
{
public:
	SnappingPointsData();
	SnappingPointsData(TArray<FVector> snappingPoints, TArray<float> snappingPointsWidth, TArray<FVector> snappingDirection);
	void SetSnappingpoints(TArray<FVector> snappingPoints, TArray<float> snappingPointsWidth, TArray<FVector> snappingDirection);
	TArray<FVector> GetSnappingPoints();	//this will return the snapping points
	TArray<float> GetSnappingPointsWidth();	//this will return the snapping point width
	TArray<FVector> GetSnappingDirection();	//this will return the snapping direction
private:
	TArray<FVector> SnappingPoints;	//this will store the snapping points
	TArray<float> SnappingPointsWidth;	//the snapping point width
	TArray<FVector> SnappingDirection;	//this will store the snapping direction
};
