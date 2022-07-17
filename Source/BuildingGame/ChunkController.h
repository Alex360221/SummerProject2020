// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChunkController.generated.h"

UCLASS()
class BUILDINGGAME_API AChunkController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChunkController();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USceneComponent* ThisScene;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SpawnChunks(class AMapController* mapRef, TArray<UStaticMesh*> newMeshes, FString gameName);	//this will spawn all the chunks in this coulmn
	void SpawnChunk(FVector chunkPos, int chunkHeight);	//this will spawn only one chunk at pos
	bool RemoveChunks();	//this will remove all chunks in this coloumn
	void SaveChunks();	//this will save all the chunks 
	class AChunk* FindChunk(FVector chunkPos);	//this will return the chunk at this pos
	bool spawnedAllChunks;
	void AddChunkToKeep(int chunk,int chunkType) { chunksToKeep[chunk] = chunkType; }	//this adds a chunk to chunks to keep
	void CheckChunks(FVector chunkPos);	//this will check to see if there is a chunk at this pos if not it will spawn one
	bool CheckOtherChunkTerrain(FVector cellPos, FVector chunkPos);	//this will call the correct chunk to see if the cell is part of the terrain
	void UpdateChunksLOD(int LOD);
	void UpdateChunksTargetVertiecs(int target);	//this will set all the chunks target vertices
	int GetChunkTargetVertices() { return chunksTargetVertiecs; }	//this will return amount of vertices
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<int> chunksToKeep;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<int>aChunks;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		int lodChunks;
	void ToggleTick(bool toggle);	//this will toggle the tick for this and sub chunks
private:
	TMap<FVector, class AChunk*> chunks;
	class AMapController* mapControllerRef;
	TArray<UStaticMesh*> staticMeshesPoint;
	FString GameName;
	int chunksInColumn;
	float chunkSize;
	bool chunksAnlaysed;
	int chunksTargetVertiecs;
	void AnlayseChunks();	
	bool tickToggle;

	

};
