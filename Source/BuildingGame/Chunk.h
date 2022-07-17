// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Lib/SimplexNoise.h"
#include "Chunk.generated.h"


UCLASS()
class BUILDINGGAME_API AChunk : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChunk(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(VisibleAnywhere)
		class UMaterialInterface* terrainMat;
	UPROPERTY(VisibleAnywhere)
		class UMaterialInterface* waterMat;
	UPROPERTY(VisibleAnywhere)
		int chunkX;
	UPROPERTY(VisibleAnywhere)
		int chunkY;
	FString gameName;
	FString chunkLevel;
	float chunkSize;
	bool LoadData;
	bool LoadedData;
	bool hasChunkLoaded;
	UPROPERTY(VisibleAnywhere)
		int chunkHeight;
	int bSeed;
	int eSeed;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	float SurfaceHeight;
	UPROPERTY(EditAnywhere)
		bool hasChunkBeenEdited;
	bool RemoveChunk();	//this will remove all the data in the chunk
	void SaveChunk(); //this will save the chunk data
	void LoadChunk();	//this will load the chunk if it can
	void SetChunkInstanceMeshes(TArray<class UStaticMesh*> meshes);	//this will add the static meshes
	void LoadICM(TArray <int> meshesType, TArray<FTransform> meshesPos);	//this will load all the icms into the world
	bool ChunkLoaded();
	int KeepChunkLoaded();	//this will check if this chunk should be loaded
	void CreateCells();
	void CreateMesh();
	void UpdateMesh();
	void LoadMesh();
	//void CreateColour();	//this will set the vertex colours
	void UpdateMeshLOD();	//this will redraw the mesh to the correct LOD

	void AddCell(FVector pos, float density);	//this adds cells to the cell array
	void AddWaterCell(FVector pos, float density);	//this adds cells to the cell array
	bool CreatedCells;
	FVector4 CellData(int meshType, float x, float y, float z);	//returns the data to do with a cell
	int AddVertex(int meshType, FVector vertex, FVector cellPos);	//adds the vertex to the vertices array returning the index at which it was added (and adds colour)
	//void AddVertexCellPos(int meshType, FVector vertex, FVector cellPos);	//this adds the vertex and cell pos to an array
	void AddTriangle(int meshType, int index);	//adds the trinalge index
	void AddCellInMesh(int meshType, FVector cellPos);	//adds the cell to mesh 
	void UpdateCell(FVector cellPos, float newCellVaule);	//this will update this cell
	bool DrawWaterCell(int meshType, FVector cellPos);	//this will check to see if the mesh should be drawn at this pos
	bool OtherCellTerrain(FVector cellPos);	//this check if this cell is part of the terrain
	bool CreatedMesh;
	bool CreatedWaterMesh;
	void SetCellColour(FVector cellPos, FColor cellColour);	//this will set the cell colour for this cell pos
	void SetTargetVertices(int newTarget);	//this will set the chunks target amount of vertices
	//int GetXAmountOfVertices(int x, float* newcellDistance); //this will return how many vertices should be in this line
	//int GetYAmountOfVertices(int y, float* newcellDistance); //this will return how many vertices should be in this line
	//float GetXCellDistance(float y, float* newcellDistance);	//this will return the cell distance
	//float GetYCellDistance(float x, float* newcellDistance);	//this will return the cell distance
	//void GetTargetCellDisatnce(float* newCellDistance);	//this will return the cell distance

	//colour
	//TMap<FVector, FVector> GetCellAndVertexData();	//this will return all the data related to the cell and vertex 

	void LoadEditedCells();	//this will update tghe edited cells
	void AddBrush(FVector location, float strength, FVector playerLocation, bool centerSelf);	//adds to terrain
	void TakeBrush(FVector location, float strength, FVector playerLocation, bool centerSelf);	//takes to terrain

	class AMapController* mapCont;
	class AChunkController* chunkCont;
	//testing things
	TArray<class APartBuilder*> partBuilderRef;
	TMap<FVector, int> placedObjects;
	void AddPartBuilder(class APartBuilder* partBulder);
	void RemovePlacedObject(AActor* object);	//this will work out what actor is being hit then remove it

	UPROPERTY(EditAnywhere)
		TArray<UInstancedStaticMeshComponent*> instancedMeshes;
	UPROPERTY(EditAnywhere)
		TArray<UMaterialInterface*> materials;
	int AddIM(int instanceType, FTransform pos, class InstanceCustomMeshData* ICM, int blockId);	//this will add an instance of type at pos called from ICM
	void AddICM(FTransform transform, int ICMID);	//this will add all the relative instances of the mesh
	void RemoveICM(UInstancedStaticMeshComponent* instanceType, int index);	//this will remove all the ICM related to one instance
	void RemoveIM(int instanceType, int index);	//this will remove this index from instance type
	int ICMD_ContainID(int id);	//this will check if any of the ICMs match this ID
	void GetAllICMPositions(TArray<FTransform>* outPositions, TArray<int>* outTypes);	//this will return an array containing all the icms positions
	FTransform ICM_GetCenterPos(UInstancedStaticMeshComponent* instanceType, int index);	//this will return the center position of the ICM
	int ICM_GetID(UInstancedStaticMeshComponent* instanceType, int index); //this will return the id to this type
	//int ICM_FindICMType(int index, int instanceType);		//this will find what type this mesh is
	TArray<class InstanceCustomMeshData*> instanceMeshData;
	TArray<class ICMData*> ICMTable;	//this needs to be the same length as the amount of instances
	TArray<class ICMKeyStorage*> ICMKeyTable;	//this needs to be the same length as the amount of instances

	UPROPERTY(VisibleAnywhere)
		TArray<FVector> Vertices;	//array to store the vertices
	UPROPERTY(VisibleAnywhere)
		TArray<int> Triangles;	//array to store the triangles
	UPROPERTY(VisibleAnywhere)
		TArray<FVector> waterVertices;	//array to store the water vertices
	UPROPERTY(VisibleAnywhere)
		TArray<int> waterTriangles;	//array to store the water triangles
	UPROPERTY(VisibleAnywhere)
		TArray<FColor> Colour;
	UPROPERTY(VisibleAnywhere)
		TArray<FVector> CellsInMesh;	//array of cells which are part of the mesh
	UPROPERTY(VisibleAnywhere)
		TArray<FVector> CellsInWaterMesh;	//array of cells which are part of the water mesh
	//TMap<FVector, FVector> VerticesPlusCellPos;

	UPROPERTY(VisibleAnywhere)
		TArray<FVector> BrushVertices; //this stores the vertices of the brush
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USceneComponent* ThisScene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UProceduralMeshComponent* ThisMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UProceduralMeshComponent* waterMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TMap<FVector, float> editedCells;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		int targetVertexCount;	//this is how many vertices should be in the mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float targetCellDistance;	//this is how many vertices should be in the mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TMap<FVector, float> Cells;	//map to contain if the cell is soild or air
private:	
	
	float cellDistance;	//how far the cells are apart
	int cellAmount; //how many cells in a line
	int cellHeightAmount; //how many cells in a line
	bool loadedChunk;	//if all the chunk data has been loaded
	
	class PlaceableStorage* placeableStorageRef;
	//mesh

	TMap<FVector, float> waterCells;	//map to contain if the cell is soild or air
	TMap<FVector, FColor> CellsColour;	//map to contain the cell colour

	TMap<FVector, int> ColourVerticesUsed;	//this stores all the vertices which have already been used and how many times
	TMap<FVector, float> ColourForVertex;	//this will store the current vertex colour
	//threading
	bool CellsCreating;
	bool MeshCreating;
	bool waterMeshCreating;
	bool MeshDrawn;
	bool waterMeshDrawn;
	bool meshLODUpdating;
	bool safeToRemove;
	//functions
	//brushes
	FVector brushCeneter;
	float brushSize;
	
	TArray<FVector2D> GenerateUVs();	//this function will create the uv map
	int CheckFlipUV(FVector p1, FVector p2, FVector p3);	//this will check to see if the uvs Need Flipping
	float FindTrianglePerimter(FVector2D p1, FVector2D p2, FVector2D p3);	//this will work out the perimter of the triangle

	TArray<FVector> GetVerticesWithinBrush(FVector center, FVector playerLocation, bool centerSelf, float strength);	//this returns all the vertices which are within the brush
	FVector GetChunkVertex(FVector vertex);	//this returns the chunk postion of where the vertex lies in
	FVector UpdateCenterVertex(FVector center, FVector newChunkPos);	//this returns the new center point 

	void UpdateCells(TArray<FVector> cellsToChange, float changeAmount, FVector playerLocation, bool centerSelf, FVector center);	//this changes the vertices
	void UpdateOtherChunkCells(TArray<AChunk*>& otherChunks, FVector cellPos, float vaule, FVector oppersit, FVector mult, float cellVaule, float changeAmount, FVector center);
	bool WithinEditArea(FVector center, FVector cellToEdit);	//center should be updated for this chunk
	void UpdateSurroundingCells(FVector centerCell, float changeAmount);	//this adds the cells around the cneter cell to to the list
	bool WithinChunkBounds(FVector cellPos);	//this checks the cell is in the chunk
	void UpdateCornerPos(FVector cellPos, TArray<AChunk*>& otherChunks, FVector center);	//if this is a corner vertex it will update the related corners
	void UpdateEdgePos(FVector cellPos, TArray<AChunk*>& otherChunks); //if this is a edge vertex it will then update related edges
	void UpdateFacePos(FVector cellPos, TArray<AChunk*>& otherChunks); //if this cell is a face vertex it will then update the relative face  
	void UpdateOtherChunkCell(TArray<AChunk*>& otherChunks, AChunk* otherChunk, FVector cellPos, FVector newCellPos, FVector center);	//this will update the other chunk cell pos
	void LoadMoreChunks(FVector cellPos);	//this will load more chunks below and above this chunk if needed
	void LoadMoreChunks_ICM();	//used to load chunks arround a built mesh;
};
