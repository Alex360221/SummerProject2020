// Fill out your copyright notice in the Description page of Project Settings.


#include "PlaceableStorage.h"
#include <BuildingGame/Chunk.h>
#include <BuildingGame/PartBuilder.h>

PlaceableStorage::PlaceableStorage()
{
}

void PlaceableStorage::SpawnPlaceble(int ID, FVector location, UWorld* worldRef)
{
	switch (ID)
	{
	case 0:
		//spawn part builder
		FRotator rotation{ 0.0f };
		partBuilder = worldRef->SpawnActor<APartBuilder>(location, rotation);
		partBuilder->SetActorEnableCollision(ECollisionEnabled::NoCollision);
		break;

	}
}

void PlaceableStorage::UpdateLocation(int ID, FVector location)
{
	switch (ID)
	{
	case 0:
		//update part builder location
		partBuilder->SetActorLocation(location);
		break;
	}
}

void PlaceableStorage::RemoveSpawnObject(int ID)
{
	switch (ID)
	{
	case 0:
		//update part builder location
		partBuilder->Destroy();
		break;
	}
}

void PlaceableStorage::SpawnPlaceableInChunk(int ID, FVector location, AChunk* chunk)
{
	switch (ID)
	{
	case 0:
		//update part builder location
		partBuilder->SetActorLocation(location);
		partBuilder->SetActorEnableCollision(true);
		chunk->AddPartBuilder(partBuilder);
		break;
	}
}

void PlaceableStorage::SpawnLoadedPlaceableObjects(TMap<FVector, int> info, AChunk* chunk, UWorld* worldRef)
{
	TArray<FVector> keys;
	TArray<int> vaules;
	info.GenerateKeyArray(keys);
	info.GenerateValueArray(vaules);
	for (int i = 0; i < keys.Num();i++)
	{
		SpawnPlaceableStrightInChunk(vaules[i], keys[i], chunk, worldRef);
	}
}

int PlaceableStorage::GetPlaceableCount()
{
	return 1;
}

void PlaceableStorage::SpawnPlaceableStrightInChunk(int ID, FVector location, AChunk* chunk, UWorld* worldRef)
{
	switch (ID)
	{
	case 0:
		//spawn part builder
		FRotator rotation{ 0.0f };
		partBuilder = worldRef->SpawnActor<APartBuilder>(location, rotation);
		partBuilder->SetActorEnableCollision(true);
		chunk->AddPartBuilder(partBuilder);
		break;
	}
}





