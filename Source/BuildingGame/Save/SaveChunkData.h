// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveChunkData.generated.h"

/**
 * 
 */
UCLASS()
class BUILDINGGAME_API USaveChunkData : public USaveGame
{
	GENERATED_BODY()
public:
    USaveChunkData();
	UPROPERTY(BlueprintReadOnly)
		TMap<FVector, float> EditedCells;
	UPROPERTY(BlueprintReadOnly)
		TArray<FVector> CellsInMesh;
	UPROPERTY(BlueprintReadOnly)
		TArray<FTransform> PlacedMeshesPos;
	UPROPERTY(BlueprintReadOnly)
		TArray<int> PlacedMeshesType;

	//Placed Objects
	UPROPERTY(BlueprintReadOnly)
		TMap<FVector, int> PlacedObjects;
};
