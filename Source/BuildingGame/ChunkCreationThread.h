// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Core/Public/HAL/Runnable.h"
#include "Runtime/Core/Public/Async/AsyncWork.h"

/**
 * 
 */
class BUILDINGGAME_API ChunkCreationThread
{
public:
	ChunkCreationThread();
};

namespace Threading
{
	float CellDensity(FVector cellPos, FVector chunkPos, float cellDistance, float surfaceHeight, float cellAmount, int bSeed);	//works out if the cell is soild
	float CellHeight(FVector cellPos, float cellDistance, int bSeed);	//works out of the cell is above or below surface
}

class ChunkCreationThreading : public FNonAbandonableTask
{
    friend class FAutoDeleteAsyncTask < ChunkCreationThreading>;
public:
    ChunkCreationThreading(class AChunk* chunkRef, int cellHeightAmount, int cellAmount, float cellDistance, float surfaceHeight, int bSeed);
	/*This function is executed when we tell our task to execute*/
	void DoWork();

	class AChunk* chunkRef;
	int cellHeightAmount;
	int cellAmount;
	float cellDistance;
	float surfaceHeight;
	FVector chunkPos;
	int bSeed;
	/*This function is needed from the API of the engine.
	My guess is that it provides necessary information
	about the thread that we occupy and the progress of our task*/	
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(ChunkCreationThreading, STATGROUP_ThreadPoolAsyncTasks);
	}
};
