// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Engine/Classes/Components/InstancedStaticMeshComponent.h"
#include "PartBuilder.generated.h"

UCLASS()
class BUILDINGGAME_API APartBuilder : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APartBuilder(const FObjectInitializer& ObjectInitializer);
	//UPROPERTY(VisibleAnywhere)
		TMap<FVector, float> cells;
		//THREADING
		bool ShouldCreateMesh;
		bool CreateCells;
		bool CreatingCells;
		bool FilledCells;
		bool FillingCells;
		bool CreatedMesh;
		bool CreatingMesh;

		UPROPERTY(VisibleAnywhere)
			TArray<FVector> vertices;
		UPROPERTY(VisibleAnywhere)
			TArray<int> triangles;
		UPROPERTY(VisibleAnywhere)
			TArray<class UInstancedStaticMeshComponent*> instancesMeshes;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USceneComponent* ThisScene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UStaticMeshComponent* PartBuilderMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UStaticMeshComponent* PartBuilderDirectionArrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UStaticMeshComponent* PartBuilderDirectionArrow1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UStaticMeshComponent* PartBuilderDirectionArrow2;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UStaticMeshComponent* PartBuilderDirectionArrow3;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UProceduralMeshComponent* HologramMesh;
	UPROPERTY(VisibleAnywhere)
		class UMaterialInterface* HologramDefaultMat;
	UPROPERTY(VisibleAnywhere)
		TArray< UMaterialInterface*> Materials;
	UPROPERTY(VisibleAnywhere)
		UProceduralMeshComponent* centerPoint;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UStaticMeshComponent* Arrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class USceneCaptureComponent2D* SceneCapture;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UTextureRenderTarget2D* meshRenderTarget2D;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UTexture2D* newTexture;
	UPROPERTY(VisibleAnywhere)
		TArray<class UStaticMeshComponent*> snappingMeshes;	//this will store all the snapping meshes
	UPROPERTY(VisibleAnywhere)
		TArray<FVector> hologramVertices;	//this will store the hologram vertices
	UPROPERTY(VisibleAnywhere)
		TArray<int> hologramTriangles;	//this will store the hologram triangle
	UPROPERTY(VisibleAnywhere)
		TArray<FVector2D> hologramUVs;	//this will store the hologram UVs

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SetMapController(class AMapController* mapRef);	//this will set the map ref
	void PartBuilderSeclected();	//this will turn on the part builder as its been sected
	void PartBuilderDeseclected();	//this will turn off the part builder as its been sected
	void ResetHologram();	//this will clear the current hologram and resetit
	void IncreaseScale(int axis);	//this will increase the scale on a axis
	void DecreaseScale(int axis);	//this will increase the scale on a axis
	void SaveHologram();	//this will save the hologram 
	void CenterVisibility();		//this will update the center visibility
	void SpawnSnappingPoint();	//this will spawn a snapping point at the center of the object
	void MoveSnappingPointForward(int axis);	//this will increase the snapping points
	void MoveSnappingPointBackwards(int axis);	//this will decrease the snapping points
	void UpdateCurrentSnappingPoint(int change);

	void UpdateSnappingPointRotationP(int direction);	//this will rotate the arrow on the pitch
	void UpdateSnappingPointRotationR(int direction);	//this will rotate the arrow on the Roll
	
	void AddCell(FVector pos, float den);	//this will add a cell
	void UpdateCell(FVector pos, float den);	//this will update the cell
	FVector4 GetCellData(float x,float y, float z);	//this will return the data
	int AddVertex(FVector pos);
	void AddTriangle(int index);
	void SetMat(int index);		//this will set the material
	void SetName(FString name);	//this will set the name

	int AddIM(int instanceType, FVector location);	//this will add the ICM mesh to the part builder
	void RemoveIm(int instanceType, int index);	//this will remove the ICM mesh for the part builder
	void UpdateDirectionArrow(int direction);	//this will remoate the arrow to show the player which axis they are moving in

private:
	void DisplayHologram();	//this will display the hologram
	void RemoveHologram();	//this will remove the hologram

	void CreateHologramMesh();	//this will create the hologram mesh
	void UpdateHologramScaleX(float scaleAmount, float direction);	//this will update the sclae of the mesh on the x axis by the direction at the sclae amount
	void UpdateHologramScaleY(float scaleAmount, float direction);	//this will update the sclae of the mesh on the y axis by the direction at the sclae amount
	void UpdateHologramScaleZ(float scaleAmount, float direction);	//this will update the sclae of the mesh on the z axis by the direction at the sclae amount
	void UpdateHologramScale(FVector cellPos, int increase, float hP, float lP, int axis, TMap<FVector, float>* newCells);	//this will update the sacle on the axis	(SCALE AMOUNT ALWAYS BE PSOTIVE)
	bool CheckScaler(FVector scaler, FVector pos);	//this will check if new pos is less than scaler on axis which arent zero
//	TArray<FVector> AdjustHologramToCenter();	//this will adjust the vertices to center point

	void ShowCenter();	//this will show the center
	void HideCenter();	//this will hide the center
	void CreateCenter();	//if the mesh hasn't been created yet create it
	void MoveCenterPointX(float moveAmount, float direction);	//this will move the center point along the x axis
	void MoveCenterPointY(float moveAmount, float direction);	//this will move the center point along the y axis
	void MoveCenterPointZ(float moveAmount, float direction);	//this will move the center point along the z axis

	void CreateSnappingPoint();	//this will spawn the snapping point
	void MoveSnappingPointX(float moveAmount, float direction);	//this will move the snapping point along the x axis
	void MoveSnappingPointY(float moveAmount, float direction);	//this will move the snapping point along the y axis
	void MoveSnappingPointZ(float moveAmount, float direction);	//this will move the snapping point along the z axis

	void AddSnappingPoints();	//this will add all the snapping points to an array
	FVector FindSnappingPointDirection(FRotator rotation);	//this will work out what direction the snapping point is facing
	
	//varaibles
	float hologramScale;	//this will store the sclae of the hologram
	bool CenterVisible;		//this will store wether or the center point is visable
	bool hasBeenCreated;	//stores wether the mesh ahs been created
	FVector Center;			//this will store the cneter pos

	TArray<FVector> centerPointVertices;	//this will store the center point vertices
	TArray<int> centerPointTriangles;		//this will store the center point triangles

	class AMapController* mapController;

	class InstanceCustomMeshData* partBuilderICM;
	class UInstancedStaticMeshComponent* currentMesh;

	int currentSnappingPoint;	//this will store the current snapping point
	FRotator newRotation;		//this will store the roation
	TArray<FVector> SnappingPoints;	//this will store the snapping points
	TArray<float> SnappingPointsWidth;
	TArray<FVector> SnappingPonitsDirection;	//this will store the snapping points direction

	int materialIndex;
	FString objectName;
};
