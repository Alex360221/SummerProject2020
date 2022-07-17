// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class BUILDINGGAME_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		UCameraComponent* CameraComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
		UStaticMeshComponent* MeshComp;
	class AMapController* mapControlRef;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
		bool inventroyOpen;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
		int meshNum;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player")
		bool escOpen;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float vaule);
	void MoveRight(float vaule);
	void TurnAtRate(float vaule);
	void LookUpRate(float vaule);
	
	void RightClickDown();
	void RightClickUp();
	void LeftClickDown();
	void LeftClickUp();

	void LeftArrow();
	void RightArrow();
	void CyleMovement();

	void CycleBuildingMode();
	void MiddleMouseButton();
	void EnterKey();
	void EKey();
	void IKey();
	void TKey();
	void ZKey();
	void CKey();
	void MouseWheelUp();
	void MouseWheelDown();

	void CtrlLeftArrow();
	void CtrlRightArrow();
	void ShiftLeftArrow();
	void ShiftRightArrow();
	void ShiftUpArrow();
	void ShiftDownArrow();

	void ShiftMouseWheelUp();
	void CrtlMouseWheelUp();
	void Save();
	void ESC();
	void Ctrl();
	void ShiftDown();
	void ShiftUp();


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		float baseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		float baseLookUpAtRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
		float speedFactor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
		float baseSpeed;
	bool mouseRightDown;
	bool mouseLeftDown;
	bool toolSelected;	
	bool buildingMode;
	int movementMethod;
	bool objectSpawned;
	


	//line trace
	void LineTraceAdd();
	void LineTraceTake();
	void LineTraceLeft();
	void LineTraceRight();
	void LineTraceUpdateLocation();	//this will update the location of the object that is about to be placed
	void CheckDistanceFromTool();	//this will check the player isn't to far way from the selected tool if so it will descted the tool

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
		void SetPlayerBuildObject(int index);
	UFUNCTION(BlueprintCallable)
		bool GetBuildingMode();	//this will return the building mode
	UFUNCTION(BlueprintCallable)
		bool GetInPartBuilder();	//this will return if the player is in the partbuilder
	UFUNCTION(BlueprintCallable)
		int GetSelectedSlot() { return CurrentSlot; }
	UFUNCTION(BlueprintCallable)
		bool GetControlDown() { return controlDown; }
	UFUNCTION(BlueprintCallable)
		void SetToolSelected();	//this will close the tool
	UFUNCTION(BlueprintCallable)
		void PartBuilderSetMat(int index);	//will set the mat for the part builder
	UFUNCTION(BlueprintCallable)
		void PartBuilderSetName(FString name);	//this will set the name of the object in the part builder
	UFUNCTION(BlueprintCallable)
		void PartBuilderSaveObject();
	UFUNCTION(BlueprintCallable)
		void PartBuilderResetObject();
	FVector AdjustToSnappingGrid(FVector pos);
	UFUNCTION(BlueprintCallable)
		void SetSnappingGrid(bool toggle) { snappingGrid = toggle; }	//this will turn on or off the snapping grid
	UFUNCTION(BlueprintCallable)
		void SetSnappingGridSize(float size) { snapGridDistance = size; }	//this will  set the snapping grid size
	UFUNCTION(BlueprintCallable)
		bool GetObjectPlaced(){return objectSpawned;}	//this returns whever the object is spawned
private:
	FHitResult outHit;
	AActor* toolRef;
	class PlaceableStorage* placeableStorageRef;
	FVector lastLineTrace;
	int leftClickCount;
	int CurrentSlot;
	int lastMeshNum;
	bool controlDown;
	float snapGridDistance;
	bool snappingGrid;
	bool shiftDown;
	

};
