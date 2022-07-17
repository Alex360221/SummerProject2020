// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedStruct.h"
#include "BlockData.generated.h"

/**
 * 
 */
UCLASS()
class BUILDINGGAME_API UBlockData : public UUserDefinedStruct
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FICMBlockData
{
    GENERATED_USTRUCT_BODY()
        FICMBlockData();
    TArray<class ICMType*> meshType;
};
