// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveMapData.generated.h"

/**
 * 
 */
UCLASS()
class BUILDINGGAME_API USaveMapData : public USaveGame
{
	GENERATED_BODY()
public:
	USaveMapData();

	UPROPERTY(BlueprintReadOnly)
		int StaticMeshesCount;
	UPROPERTY(BlueprintReadOnly)
		FVector PlayerPos;
	UPROPERTY(BlueprintReadOnly)
		FRotator PlayerRot;

};
