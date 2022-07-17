// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Player/PlayerCharacter.h"
#include "Chunk.h"
#include "MeshData/MeshStorage.h"
#include "MeshData/InstanceCustomMeshData.h"
#include "Meshes/CustomInstanceMesh.h"
#include "MapController.generated.h"

UCLASS()
class BUILDINGGAME_API AMapController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMapController(const FObjectInitializer& ObjectInitializer);

	int viewDistance;
	FString gameName;
	UPROPERTY(EditAnywhere)
		APlayerController* pController;
	UPROPERTY(EditAnywhere)
		FVector2D playerLocationChunk;
	UPROPERTY(EditAnywhere)
		TMap<FVector2D, class AChunkController*> chunkArray;
	UPROPERTY(EditAnywhere)
		FVector loadedPlayerStart;
	bool loadingGame;
	bool hasLoaded;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USceneComponent* ThisScene;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	AChunk* GetChunk(FVector chunkPos);	//this will return the chunk at a given cord
	void SaveGame();	//this will save all the data
	void LoadGame();	//this will load all the data

private:
	//chunk variables
	TMap<FVector2D, class AChunkController*> activeChunks;
	class AChunkController* currentChunk;
	float chunkSize;
	void SpawnChunks();
	void RemoveChunks();
	void SpawnChunk(FVector location);
	void RemoveChunk(FVector2D location);
	void UpdateChunksLOD();
	void SaveAllchunks();
	AChunk* FindChunk(FVector objLocation);	//this will find the chunk that the obj is in


	//mesh stuff
	int meshCount;
	bool loadedAllMeshes;
	UPROPERTY(EditAnywhere)
		TArray<UInstancedStaticMeshComponent*> instancedMeshes;
	UPROPERTY(EditAnywhere)
		UMaterialInterface* ghostMat;
	UInstancedStaticMeshComponent* currentMesh;
	int currentInstanceMeshIndex;
	
	TArray<class SnappingPointsData*> SnappingPoints;
	FVector toPlaceLocation;
	TArray<class InstanceCustomMeshData*> instanceMeshData;
	
public:
	void CheckChunk(FVector chunkPos);	//this will check the chunk at this location
	AChunkController* GetChunkController(FVector2D chunkPos); //this will return the chunkcontroller ref
	void AddMesh(TMap<FVector, float> newCells, float xSize, float ySize, float zSize, float cellSpacing, FVector Center, TArray<FVector> snapP,
		TArray<float> snapW, TArray<FVector> snapDir, int matIndex, FString name);	//this will add a new triangle 
	TArray<FVector> GetICM_Positions(int id) { return instanceMeshData[id]->GetPositions(); }
	TArray<int> GetICM_InstanceTypes(int id) { return instanceMeshData[id]->GetInstancesType(); }
	int GetICM_ID(int id) { return instanceMeshData[id]->GetID(); }
	int GetICM_MatIndex(int id) { return instanceMeshData[id]->GetMatIndex(); }
	int GetInstancesType(int index);	//this will return the number of instance static meshes
	void AddICM(int id, FVector location);	//this will create the instance static meshes
	int AddIM(int instanceType, FVector location);	//this will add a insatnce of this instance type into the world
	void UpdateICM(FVector location);	//this will update all instances related to that id
	void UpdateIm(int instanceType, int index, FTransform location);	//this will update the location of the instance
	void RemoveICM();	//this will remove all the instances related to the current mesh
	void RemoveIm(int instanceType, int index);	//this will remove this instance
	SnappingPointsData* ICM_GetSnappingPointsRef(int index);	//this will return the snapping points belonging to the instance type
	void RotateICM_P(int direction);	//this will rotate the ICM on pith
	void RotateICM_R(int direction);	//this will rotate the ICM on pith
	void BuildICM(FVector lcoation);	//this will build the icm model in the chunk
	void RemoveInstanceMesh();	//this will remove the current mesh
	bool CheckMeshIndexIsFaild(int meshIndex);	//this will check if the mesh index is failed within the snapping points data array
	TArray<FVector> GetSnappingPoints(int meshIndex);	//this will return an array of snapping points belonging to that mesh index
	TArray<float> GetSnappingPointsWidth(int meshIndex);	//this will return an array of snapping points width belonging to that mesh index
	TArray<FVector> GetSnappingDirection(int meshIndex);	//this will return an array of snapping points direction belonging to that mesh index
	void RemoveHitObject(FHitResult hitResult);	//this will remove the hit object
	UFUNCTION(BlueprintCallable)
		int GetObjectCount();	//this will return the amount of objects
	UFUNCTION(BlueprintCallable)
		FString GetObjectName(int index);	//this will return the object name
	UFUNCTION(BlueprintCallable)
		bool CheckObjectNameVaild(FString name);
private:
	FRotator meshRotation;	//this will store the mesh rotation	
	class PlaceableStorage* placeableStorageRef;
	TArray<FString> ObjectNames;
	void HasChunkUnderPlayerLoaded();	//check if the chunk the player is in has loaded
	UInstancedStaticMeshComponent* ICMarrows;
	void ICM_AddArrows(FVector spawnPos, int id);	//this will spawn the arrows
	void ICM_UPdateArrows(FTransform spawnPos, int id);	//this will update the arrows positions
};
