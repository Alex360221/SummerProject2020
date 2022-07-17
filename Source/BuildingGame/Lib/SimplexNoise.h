// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <cstddef>  // size_t
#include "CoreMinimal.h"

/**
 * 
 */
class BUILDINGGAME_API SimplexNoise
{
public:
	static unsigned char perm[];
	static float  grad(int hash, float x);
	static float  grad(int hash, float x, float y);
	static float  grad(int hash, float x, float y, float z);
	static float  grad(int hash, float x, float y, float z, float t);

	static void setNoiseSeed(const int32& newSeed);
	static float SimplexNoise2D(float x, float y);
	static float SimplexNoise3D(float x, float y, float z, float w);

private:
	// Parameters of Fractional Brownian Motion (fBm) : sum of N "octaves" of noise
	float mFrequency;   ///< Frequency ("width") of the first octave of noise (default to 1.0)
	float mAmplitude;   ///< Amplitude ("height") of the first octave of noise (default to 1.0)
	float mLacunarity;  ///< Lacunarity specifies the frequency multiplier between successive octaves (default to 2.0).
	float mPersistence; ///< Persistence is the loss of amplitude between successive octaves (usually 1/lacunarity)
};
