// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Core/Public/HAL/Runnable.h"
#include "Runtime/Core/Public/Async/AsyncWork.h"

/**
 * 
 */
class BUILDINGGAME_API ChunkMeshCreationThread
{
public:
	ChunkMeshCreationThread();
};
namespace Threading
{
}

class ChunkMeshCreationThreading : public FNonAbandonableTask
{
    friend class FAutoDeleteAsyncTask <ChunkMeshCreationThreading>;
public:
    ChunkMeshCreationThreading(class AChunk* chunkRef, int cellHeightAmount, int cellAmount, float cellDistance);
    /*This function is executed when we tell our task to execute*/
    void DoWork();

	class AChunk* chunkRef;
	int cellHeightAmount;
	int cellAmount;
	float cellDistance;

	/*This function is needed from the API of the engine.
	My guess is that it provides necessary information
	about the thread that we occupy and the progress of our task*/
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(ChunkMeshCreationThreading, STATGROUP_ThreadPoolAsyncTasks);
	}
};
