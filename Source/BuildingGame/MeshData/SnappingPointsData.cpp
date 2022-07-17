// Fill out your copyright notice in the Description page of Project Settings.


#include "SnappingPointsData.h"

SnappingPointsData::SnappingPointsData()
{
}

SnappingPointsData::SnappingPointsData(TArray<FVector> snappingPoints, TArray<float> snappingPointsWidth, TArray<FVector> snappingDirection)
{
	SnappingPoints = snappingPoints;
	SnappingPointsWidth = snappingPointsWidth;
	SnappingDirection = snappingDirection;
}

void SnappingPointsData::SetSnappingpoints(TArray<FVector> snappingPoints, TArray<float> snappingPointsWidth, TArray<FVector> snappingDirection)
{
	SnappingPoints = snappingPoints;
	SnappingPointsWidth = snappingPointsWidth;
	SnappingDirection = snappingDirection;
}

TArray<FVector> SnappingPointsData::GetSnappingPoints()
{
	//GLog->Log(FString::FromInt(SnappingPoints.Num()));
	return SnappingPoints;
}

TArray<float> SnappingPointsData::GetSnappingPointsWidth()
{
	return SnappingPointsWidth;
}

TArray<FVector> SnappingPointsData::GetSnappingDirection()
{
	return SnappingDirection;
}
