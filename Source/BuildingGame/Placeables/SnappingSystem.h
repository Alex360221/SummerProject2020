// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class BUILDINGGAME_API SnappingSystem
{
public:
	SnappingSystem();
    FVector FindSnappingPoint(FVector hitLocation, FVector meshLocation, FRotator rotation, int meshIndex, class AMapController* mapConRef);  //this will find the snapping pos for the mesh

private:
    bool SnappingPointFound(FVector start, FVector end);    //does a line trace to dectect the hitting points
    bool WithinSnappingPoint(UInstancedStaticMeshComponent* otherMesh, FVector hitPos);  //check if the hit location is within the snapping point of the hit mehs
    FVector NewSnappedPos(UInstancedStaticMeshComponent* otherMesh, FVector currentSnapPos);    //this will return thenew position for the object 

    FVector snappedPos;	//this will store which snapping point is being used
    //arrays for storing the deafulat inforamtion abouts the snapping points
    TArray<FVector> snapPointsDef;
    TArray<float> snapWidthDef;
    TArray<FVector> snapDirDef;
    class UWorld* world;
    class AMapController* mapControllerRef;
    FVector meshActorLocation;
    FRotator meshRotation;
    FHitResult outHit;	//this will store the outhot of any line traces
};
