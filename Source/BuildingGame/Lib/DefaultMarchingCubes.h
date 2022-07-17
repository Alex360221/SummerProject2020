// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class BUILDINGGAME_API DefaultMarchingCubes
{
public:
	DefaultMarchingCubes();
    
    TMap<FVector, float> CreateCells(float xSize, float ySize, float zSize,float cellSpacing); //this will create the cells
    TMap<FVector, float> FillCells_Cube(TMap<FVector, float> cells,float xSize, float ySize, float zSize, float cellSpacing);  //this will create a cube of this size

};

class CreateCellsThreading : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask < CreateCellsThreading>;
public:
	CreateCellsThreading(float xSize, float ySize, float zSize, float newCellSpacing,AActor* parent);
	/*This function is executed when we tell our task to execute*/
	void DoWork();
	class APartBuilder* parentRef;
	int hX;
	int lX;
	int hY;
	int lY;
	int hZ;
	int lZ;
	float cellSpacing;
	/*This function is needed from the API of the engine.
	My guess is that it provides necessary information
	about the thread that we occupy and the progress of our task*/
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(CreateCellsThreading, STATGROUP_ThreadPoolAsyncTasks);
	}
};

class FillCells_CubeThreading : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask < FillCells_CubeThreading>;
public:
	FillCells_CubeThreading(float xSize, float ySize, float zSize, float newCellSpacing, AActor* parent);
	/*This function is executed when we tell our task to execute*/
	void DoWork();
	class APartBuilder* parentRef;
	int hX;
	int lX;
	int hY;
	int lY;
	int hZ;
	int lZ;
	float cellSpacing;
	/*This function is needed from the API of the engine.
	My guess is that it provides necessary information
	about the thread that we occupy and the progress of our task*/
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FillCells_CubeThreading, STATGROUP_ThreadPoolAsyncTasks);
	}
};

class CreateMeshThreading : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask < CreateMeshThreading>;
public:
	CreateMeshThreading(float xSize, float ySize, float zSize, float newCellSpacing, AActor* parent);
	/*This function is executed when we tell our task to execute*/
	void DoWork();
	class APartBuilder* parentRef;
	int hX;
	int lX;
	int hY;
	int lY;
	int hZ;
	int lZ;
	float cellSpacing;
	/*This function is needed from the API of the engine.
	My guess is that it provides necessary information
	about the thread that we occupy and the progress of our task*/
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(CreateMeshThreading, STATGROUP_ThreadPoolAsyncTasks);
	}
};
