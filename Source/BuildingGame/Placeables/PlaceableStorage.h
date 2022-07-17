// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
/**
 * 
 */
class BUILDINGGAME_API PlaceableStorage
{
public:
	PlaceableStorage();
    void SpawnPlaceble(int ID, FVector location, UWorld* worldRef);   //this will spawn the object id at the location
    void UpdateLocation(int ID, FVector location);    //this will update the location of the spawn object
    void RemoveSpawnObject(int ID); //this will remove the 
    void SpawnPlaceableInChunk(int ID, FVector location, class AChunk* chunk);  //this will transfer this refference to the chunk
    void SpawnLoadedPlaceableObjects(TMap<FVector, int> info, class AChunk* chunk, UWorld* worldRef);  //this will spawn all the loaded objects in to the chunks
    int GetPlaceableCount();    //this will return the current amount of placeables
private:
    void SpawnPlaceableStrightInChunk(int ID, FVector location, class AChunk* chunk, UWorld* worldRef); //this will spawn objects stright into the chunk
    class APartBuilder* partBuilder;    //this could just be an actor with casting -- note
};
