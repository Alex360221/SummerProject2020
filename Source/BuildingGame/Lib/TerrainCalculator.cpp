// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainCalculator.h"

TerrainCalculator::TerrainCalculator()
{
}

float TerrainCalculator::GetEnvironmentHeight(FVector vertex, int bSeed)
{
	FastNoise noise;
	float b;
	float v;
	//float biome;
	//noise.SetNoiseType(FastNoise::Cellular);
	//biome = noise.GetNoise(vertex.X * 0.1, vertex.Y * 0.1) * 100;
	////now the biomes can apply certian noise modifer
	//if (biome >= 0.8)
	//{//Mountains
	//	b = biome;
		noise.SetNoiseType(FastNoise::Perlin);
		noise.SetFractalOctaves(5);
		//noise.SetFrequency(0.006);
		b = noise.GetNoise(vertex.X * 0.01, vertex.Y * 0.01) * 300;
		noise.SetNoiseType(FastNoise::SimplexFractal);
		noise.SetFractalType(FastNoise::RigidMulti);
		noise.SetFractalOctaves(4);
		//	noise.SetFractalLacunarity()
		v = noise.GetNoise(vertex.X * 0.01, vertex.Y * 0.01) * 100;
		b = b + v;
		b += 100;
	//}
	//else if (biome >= -0.9)
	//{//hills anf flat land
	//	//b = biome;
	//	noise.SetNoiseType(FastNoise::Perlin);
	//	noise.SetFractalOctaves(5);
	//	//noise.SetFrequency(0.006);
	//	b = noise.GetNoise(vertex.X * 0.01, vertex.Y * 0.01) * 300;
	//	noise.SetNoiseType(FastNoise::SimplexFractal);
	//	noise.SetFractalType(FastNoise::RigidMulti);
	//	noise.SetFractalOctaves(4);
	//	//	noise.SetFractalLacunarity()
	//	v = noise.GetNoise(vertex.X * 0.01, vertex.Y * 0.01) * 100;
	//	b = b + v;
	//	b += 100;
	//}
	//else
	//{//something else
	//	noise.SetNoiseType(FastNoise::Perlin);
	//	noise.SetFractalOctaves(5);
	//	//noise.SetFrequency(0.006);
	//	b = noise.GetNoise(vertex.X * 0.01, vertex.Y * 0.01) * 10;
	//	noise.SetNoiseType(FastNoise::SimplexFractal);
	//	noise.SetFractalType(FastNoise::RigidMulti);
	//	noise.SetFractalOctaves(4);
	//	//	noise.SetFractalLacunarity()
	//	v = noise.GetNoise(vertex.X * 0.01, vertex.Y * 0.01) * 50;
	//	b = b + v;
	//	//b -= 400;
	//}

	
	//float u = noise.GetNoise(vertex.X * 0.01, vertex.Y * 0.01) * 50;
	return b;
}

