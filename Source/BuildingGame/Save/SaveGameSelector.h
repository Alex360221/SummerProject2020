// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveGameSelector.generated.h"

/**
 * 
 */
UCLASS()
class BUILDINGGAME_API USaveGameSelector : public USaveGame
{
	GENERATED_BODY()
public:
	USaveGameSelector();
	UPROPERTY(BlueprintReadOnly)
		TArray<FString> GameNames;
	UPROPERTY(BlueprintReadOnly)
		FString GameName;
	UPROPERTY(BlueprintReadOnly)
		TArray<int> GraphicsIndices;
	UPROPERTY(BlueprintReadOnly)
		int RenderDistance;
};
