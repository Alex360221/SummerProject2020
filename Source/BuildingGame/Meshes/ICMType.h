// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class BUILDINGGAME_API ICMType
{
public:
	ICMType();
    TArray<int> insatnceType;
    TArray<int> relatedInstanceIndex;
    FTransform center;
};
