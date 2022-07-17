// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SimplexNoise.h"
#include "FastNoise.h"

/**
 * 
 */
class BUILDINGGAME_API TerrainCalculator
{
public:
	TerrainCalculator();
    float GetEnvironmentHeight(FVector vertex, int bSeed);
private: 

};
