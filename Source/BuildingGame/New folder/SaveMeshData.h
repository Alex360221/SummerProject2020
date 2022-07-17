// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveMeshData.generated.h"

/**
 * 
 */
UCLASS()
class BUILDINGGAME_API USaveMeshData : public USaveGame
{
	GENERATED_BODY()
public:
    USaveMeshData();
    UPROPERTY(VisibleAnywhere, Category = "Basics")
        TArray<FVector> Locations;
    UPROPERTY(VisibleAnywhere, Category = "Basics")
        TArray<int> InstanceType;
    UPROPERTY(VisibleAnywhere, Category = "Basics")
        int ICMID;
    UPROPERTY(VisibleAnywhere, Category = "Basics")
        TArray<FVector> SnappingPoints;
    UPROPERTY(VisibleAnywhere, Category = "Basics")
        TArray<float> SnappingPointsWidth;
    UPROPERTY(VisibleAnywhere, Category = "Basics")
        TArray<FVector> SnappingPointsDirection;
    UPROPERTY(VisibleAnywhere, Category = "Basics")
        int MaterialIndex;
    UPROPERTY(VisibleAnywhere, Category = "Basics")
        FString ObjectName;
};
