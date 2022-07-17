// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Engine/UserDefinedStruct.h"
#include <BuildingGame/Meshes/BlockData.h>
#include "CoreMinimal.h"

/**
 * 
 */

class BUILDINGGAME_API InstanceCustomMeshData
{
public:
	InstanceCustomMeshData();
    InstanceCustomMeshData(TArray<FVector> newPositions, TArray<int> newInstanceType, int newID, int newMatIndex);
    void SetInstanceCustomMeshData(TArray<FVector> newPositions, TArray<int> newInstanceType,int newID);
    TArray<FVector> GetPositions();
    TArray<int> GetInstancesType();
    int GetID() { return ICMID; }
    FVector GetCenterPos() { return centerPos; }
    TArray<FTransform> GetCenterPositions() { return centerPositions; }
    int GetMatIndex() { return matIndex; }

    void CreateICM(FVector pos, class AMapController* map);    //this will spawn all the instances into the world
    void CreateICM(FVector pos, class APartBuilder* partBuilder);    //this will spawn all the instances into the partbuilder
    void CreateICM(FTransform pos, class AChunk* chunk);          //this will spawn all the insatnces into a chunk
    void UpdateICM(FTransform pos, class AMapController* map);    //this will update the location of all the instances
    void RemoveICM(class AMapController* map);                  //this will remove all the instancs
    void RemoveICM(class APartBuilder* partBuilder);            //this will remove all the instancs
    void RemoveICM(class AChunk* chunk, int blockID); //this will remove all the instances related to this key in the map
    FTransform FindCenterPos(int blockID);    //this will return the center pos of the ICM related to this key
private:
    FVector centerPos;
    TArray<FVector> positions;
    TArray<int> instanceType;
    TArray<int> relatedInstances; //this will store all the related instances
    TMap<float, int> blocks;  //this will store all the data about each block related to a key
    int ICMID;
    int matIndex;
    //TArray<FICMBlockData> blockStruct;
    FICMBlockData blockStruct;
    TArray<FTransform> centerPositions;
    TArray<FVector> Centerpositions;
  //  TArray<FblockData> plese;
    int FindInstanceType(int meshType, int matType);
};



