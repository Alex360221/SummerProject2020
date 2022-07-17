// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class BUILDINGGAME_API ICMKeyStorage
{
public:
	ICMKeyStorage();
  
    TArray<int> instances;  //Should both be the same lengt - shouldn't contain the same vaule twice    //will be adjust to match the instances array
   // TArray<int> instanceType;   //won't be adjustable
   // TArray<double> keys;    //won't be adjustable
    TArray<int> atSpawnInstances;   //this is the instances index of this row   //won't be adjustable
    TArray<int> blockIDs;   //this stores what block this row is used to get the data about block in ICM //will be adjust to match the amoutn of blocks inside ICM
    TArray<class InstanceCustomMeshData*> ICMData;  //this will store the refence to the icm for this block //won't be adjustable
    void ICMKeyStorageAdd(int newInstance, int newSpawnInstances, int newBlockID, class InstanceCustomMeshData* newICMD);   //this adds a new vaule to both arrays
    int GetInstance(int spawnInstance, int blockID, class InstanceCustomMeshData* ICM); //this will return the instance in the same row as the spawned instance
    //double GetKey(int spawnInstance);   //this will return the key in the same row ad the spawned instance
    int GetBlockID(int instanceIndex);  //this returns the block id from the row of instances
    class InstanceCustomMeshData* GetICMData(int instanceIndex);    //this will return the ICM data from the rom of insatnces
    void Remove(int instanceIndex);  //this will remove this index from each of the arrays then re calculate the instances
    void UpdateBlockIDs(int blockID, class InstanceCustomMeshData* ICM);    //this will update all block ids to be called after ICM has been removed
    //void UpdateInstances(int newInstanceType);  //this will update the instances of this type 
};
