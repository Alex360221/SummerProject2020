// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class BUILDINGGAME_API ICMData
{
public:
	ICMData();

    TMap<float, class InstanceCustomMeshData*> meshType;
};
