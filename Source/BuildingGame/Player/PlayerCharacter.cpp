// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Controller.h"
#include "DrawDebugHelpers.h"
#include <buildingGame/MapController.h>
#include <BuildingGame/Chunk.h>
#include <BuildingGame/PartBuilder.h>
#include <BuildingGame/Placeables/PlaceableStorage.h>


// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(20.f, 40.0f);
	//	GetCapsuleComponent()->SetupAttachment(RootComponent);
		/*SpringArmCom = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
		SpringArmCom->SetupAttachment(RootComponent);
		SpringArmCom->TargetArmLength = 0.f;*/

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(GetCapsuleComponent());
	CameraComp->bUsePawnControlRotation = true;
	CameraComp->SetRelativeLocation(FVector(0, 0, 15));

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(GetCapsuleComponent());

	baseTurnRate = 45.f;
	baseLookUpAtRate = 45.f;
	speedFactor = 0.75f;
	baseSpeed = 10.0f;

	mouseRightDown = false;
	mouseLeftDown = false;
	toolSelected = false;
	buildingMode = false;
	movementMethod = 0;
	objectSpawned = false;
	meshNum = 0;
	placeableStorageRef = new PlaceableStorage();
	leftClickCount = 0;
	inventroyOpen = false;
	snapGridDistance = 100;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void APlayerCharacter::MoveForward(float vaule)
{
	if ((Controller) && (vaule != 0.0f))
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), vaule);
	}
}

void APlayerCharacter::MoveRight(float vaule)
{
	if ((Controller) && (vaule != 0.0f))
	{
		AddMovementInput(GetActorRightVector(), vaule);
	}
}

void APlayerCharacter::TurnAtRate(float vaule)
{
	if (inventroyOpen == false && escOpen == false )
	{
		if (controlDown == false)
		{
			APlayerCharacter::AddControllerYawInput(vaule);
		}		
	}	
}

void APlayerCharacter::LookUpRate(float vaule)
{
	if (inventroyOpen == false && escOpen == false)
	{
		if (controlDown == false)
		{
			APlayerCharacter::AddControllerPitchInput(vaule);
		}
	}
}

void APlayerCharacter::RightClickDown()
{
	if (!escOpen && !inventroyOpen && !toolSelected)
	{
		mouseRightDown = true;
	}
}

void APlayerCharacter::RightClickUp()
{
	mouseRightDown = false;
}

void APlayerCharacter::LeftClickDown()
{
	if (!escOpen && !inventroyOpen && !toolSelected)
	{		
		mouseLeftDown = true;
		if (meshNum >= 0)
		{
			if (leftClickCount >= 2) { leftClickCount = 0; }
			else { leftClickCount++; }
		}
		
		GLog->Log(FString::FromInt(leftClickCount));		
	}
}

void APlayerCharacter::LeftClickUp()
{
	mouseLeftDown = false;
}

void APlayerCharacter::LeftArrow()
{
	if (toolSelected)
	{
		APartBuilder* partBuilder = Cast<APartBuilder>(toolRef);
		if (partBuilder != nullptr)
		{
			partBuilder->IncreaseScale(movementMethod);
		}
	}
}

void APlayerCharacter::RightArrow()
{
	if (toolSelected)
	{
		APartBuilder* partBuilder = Cast<APartBuilder>(toolRef);
		if (partBuilder != nullptr)
		{
			partBuilder->DecreaseScale(movementMethod);
		}
	}
}

void APlayerCharacter::CyleMovement()
{
	if (toolSelected)
	{
		if (movementMethod +1> 2)
		{
			movementMethod = 0;
		}
		else
		{
			movementMethod++;
		}
		APartBuilder* partBuilder = Cast<APartBuilder>(toolRef);
		if (partBuilder != nullptr)
		{
			partBuilder->UpdateDirectionArrow(movementMethod);
		}
	}
}

void APlayerCharacter::CycleBuildingMode()
{
	if (buildingMode)
	{
		buildingMode = false;
		GLog->Log("Landscape Mode");
	}
	else
	{
		buildingMode = true;
		GLog->Log("Building Mode");
	}
}

void APlayerCharacter::MiddleMouseButton()
{
	if (buildingMode)
	{
		if (meshNum == 0)
		{
			//cancel building
			if (objectSpawned && placeableStorageRef != nullptr)
			{
				//destroy building
				placeableStorageRef->RemoveSpawnObject(0);
			}
		}
		else
		{
			if (objectSpawned == true)
			{
				mapControlRef->RemoveICM();
			}
		}				
		objectSpawned = false;
		leftClickCount = 0;
	}
}

void APlayerCharacter::EnterKey()
{
	if (toolSelected)
	{
		APartBuilder* partBuilder = Cast<APartBuilder>(toolRef);
		if (partBuilder != nullptr)
		{
			partBuilder->SaveHologram();
		}
	}
}

void APlayerCharacter::EKey()
{
	if (escOpen == false && toolSelected == false)
	{
		if (inventroyOpen == true) { inventroyOpen = false; }
		else { inventroyOpen = true; }
	}	
}

void APlayerCharacter::IKey()
{
	if (toolSelected)
	{
		APartBuilder* partBuilder = Cast<APartBuilder>(toolRef);
		if (partBuilder != nullptr)
		{
			partBuilder->CenterVisibility();
		}
	}
}

void APlayerCharacter::TKey()
{

}

void APlayerCharacter::ZKey()
{
	//if (toolSelected)
	//{
	//	APartBuilder* partBuilder = Cast<APartBuilder>(toolRef);
	//	if (partBuilder != nullptr)
	//	{
	//		partBuilder->SpawnSnappingPoint();
	//	}
	//}
}

void APlayerCharacter::CKey()
{
	if (toolSelected)
	{
		APartBuilder* partBuilder = Cast<APartBuilder>(toolRef);
		if (partBuilder != nullptr)
		{
			partBuilder->ResetHologram();
		}
	}
}

void APlayerCharacter::MouseWheelUp()
{
	if (toolSelected)
	{
		APartBuilder* partBuilder = Cast<APartBuilder>(toolRef);
		if (partBuilder != nullptr)
		{
			partBuilder->UpdateCurrentSnappingPoint(1);
		}
	}
	if (CurrentSlot < 8) { CurrentSlot++; }
	else { CurrentSlot = 0; }
}

void APlayerCharacter::MouseWheelDown()
{
	if (toolSelected)
	{
		APartBuilder* partBuilder = Cast<APartBuilder>(toolRef);
		if (partBuilder != nullptr)
		{
			partBuilder->UpdateCurrentSnappingPoint(-1);
		}
	}
	if (CurrentSlot > 0) { CurrentSlot--; }
	else { CurrentSlot = 8; }
}

void APlayerCharacter::CtrlLeftArrow()
{
	if (toolSelected)
	{
		APartBuilder* partBuilder = Cast<APartBuilder>(toolRef);
		if (partBuilder != nullptr)
		{
			partBuilder->MoveSnappingPointForward(movementMethod);
		}
	}
}

void APlayerCharacter::CtrlRightArrow()
{
	if (toolSelected)
	{
		APartBuilder* partBuilder = Cast<APartBuilder>(toolRef);
		if (partBuilder != nullptr)
		{
			partBuilder->MoveSnappingPointBackwards(movementMethod);
		}
	}
}

void APlayerCharacter::ShiftLeftArrow()
{
	if(toolSelected)
	{
		APartBuilder* partBuilder = Cast<APartBuilder>(toolRef);
		if (partBuilder != nullptr)
		{
			partBuilder->UpdateSnappingPointRotationP(-1);
		}
	}
}

void APlayerCharacter::ShiftRightArrow()
{
	if(toolSelected)
	{
		APartBuilder* partBuilder = Cast<APartBuilder>(toolRef);
		if (partBuilder != nullptr)
		{
			partBuilder->UpdateSnappingPointRotationP(1);
		}
	}
}

void APlayerCharacter::ShiftUpArrow()
{
	if (toolSelected)
	{
		APartBuilder* partBuilder = Cast<APartBuilder>(toolRef);
		if (partBuilder != nullptr)
		{
			partBuilder->UpdateSnappingPointRotationR(-1);
		}
	}
}

void APlayerCharacter::ShiftDownArrow()
{
	if (toolSelected)
	{
		APartBuilder* partBuilder = Cast<APartBuilder>(toolRef);
		if (partBuilder != nullptr)
		{
			partBuilder->UpdateSnappingPointRotationR(1);
		}
	}
}

void APlayerCharacter::ShiftMouseWheelUp()
{
	if (objectSpawned == true)
	{
		mapControlRef->RotateICM_P(1);
	}
}

void APlayerCharacter::CrtlMouseWheelUp()
{
	if (objectSpawned == true)
	{
		mapControlRef->RotateICM_R(1);
	}
}

void APlayerCharacter::Save()
{
	mapControlRef->SaveGame();
}

void APlayerCharacter::ESC()
{
	if (escOpen == true) { escOpen = false; }
	else { escOpen = true; }
}

void APlayerCharacter::Ctrl()
{
	if (toolSelected == true)
	{
		if (controlDown == true) { controlDown = false; }
		else { controlDown = true; }
	}
}

void APlayerCharacter::ShiftDown()
{
	shiftDown = true;
}

void APlayerCharacter::ShiftUp()
{
	shiftDown = false;
}

void APlayerCharacter::LineTraceAdd()
{	
	GLog->Log("Line Trace add");
	FVector start = CameraComp->GetComponentLocation();
	FVector forwardVector = CameraComp->GetForwardVector();
	FVector end = (forwardVector * 400) + start;
	FCollisionQueryParams collisionsParms;

	//DrawDebugLine(GetWorld(), start, end, FColor::Green, false);

	bool isHit = GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECC_Visibility, collisionsParms);

	if (isHit)
	{
		AChunk *chunk = Cast<AChunk>(outHit.GetActor());
		if (chunk != nullptr && !toolSelected)
		{
			chunk->AddBrush(outHit.ImpactPoint, 0.01f, FVector::ZeroVector, true);
		}
		else		
		{
			if (!toolSelected)
			{
				APartBuilder* partBuilder = Cast<APartBuilder>(outHit.GetActor());
				if (partBuilder != nullptr)
				{
					toolSelected = true;
					toolRef = outHit.GetActor();
					partBuilder->PartBuilderSeclected();
				}
			}
		}

	}
}

void APlayerCharacter::LineTraceTake()
{
	GLog->Log("Line Trace take");
	FVector start = CameraComp->GetComponentLocation();
	FVector forwardVector = CameraComp->GetForwardVector();
	FVector end = (forwardVector * 400) + start;
	FCollisionQueryParams collisionsParms;


	bool isHit = GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECC_Visibility, collisionsParms);

	if (isHit)
	{
		AChunk* chunk = Cast<AChunk>(outHit.GetActor());
		if (chunk != nullptr && !toolSelected)
		{
			chunk->TakeBrush(outHit.ImpactPoint, -0.01f, FVector::ZeroVector, true);
		}
		else
		{
			if (toolSelected)
			{
				APartBuilder* partBuilder = Cast<APartBuilder>(outHit.GetActor());
				if (partBuilder != nullptr)
				{
					toolSelected = false;
					toolRef = outHit.GetActor();
					partBuilder->PartBuilderDeseclected();
				}
			}
		}
	}
}

void APlayerCharacter::LineTraceLeft()
{
	GLog->Log("Line Trace left Building mode");
	FVector start = CameraComp->GetComponentLocation();
	FVector forwardVector = CameraComp->GetForwardVector();
	FVector end = (forwardVector * 400) + start;
	FCollisionQueryParams collisionsParms;


	bool isHit = GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECC_Visibility, collisionsParms);

	if (isHit)
	{
		//have building mode section here
		if (leftClickCount == 1)
		{
			if (objectSpawned == false)
			{
				if (meshNum == 0)
				{
					objectSpawned = true;
					GLog->Log("Spawn Object");
					placeableStorageRef->SpawnPlaceble(0, AdjustToSnappingGrid(outHit.ImpactPoint), GetWorld());
				}
				else
				{
					objectSpawned = true;
					GLog->Log("Spawn Mesh");
					mapControlRef->AddICM(mapControlRef->GetInstancesType(meshNum - 1), AdjustToSnappingGrid(outHit.ImpactPoint));
					GLog->Log("mesh muberrr " + FString::FromInt(meshNum - 1));
				}

			}
		}
		else if (leftClickCount == 2)
		{
			//have building mode section here
			if (objectSpawned == true)
			{
				AChunk* chunk = Cast<AChunk>(outHit.GetActor());
				if (chunk != nullptr && !toolSelected)
				{
					if (meshNum == 0)
					{
						objectSpawned = false;
						GLog->Log("build Object");
						placeableStorageRef->SpawnPlaceableInChunk(0, AdjustToSnappingGrid(outHit.ImpactPoint), chunk);
						leftClickCount = 0;
					}
					else
					{
						objectSpawned = false;
						GLog->Log("build Mesh");
						//mapControlRef->BuildStaticMesh(outHit.ImpactPoint);
						mapControlRef->BuildICM(AdjustToSnappingGrid(outHit.ImpactPoint));
						leftClickCount = 0;
					}
				}
				else
				{
					if (meshNum == 0)
					{
						placeableStorageRef->RemoveSpawnObject(0);
					}
					objectSpawned = false;
					GLog->Log("build Mesh");
					mapControlRef->BuildICM(AdjustToSnappingGrid(outHit.ImpactPoint));
					leftClickCount = 0;
				}
			}
		}
		if (toolSelected)
		{
			APartBuilder* partBuilder = Cast<APartBuilder>(outHit.GetActor());
			if (partBuilder != nullptr)
			{
				toolSelected = false;
				toolRef = outHit.GetActor();
				partBuilder->PartBuilderDeseclected();
			}
		}
	}
	else
	{
		if(leftClickCount == 1){ leftClickCount = 0; }
		//hitnothing
		if (leftClickCount == 2)
		{
			objectSpawned = false;
			GLog->Log("build Mesh");
			mapControlRef->BuildICM(AdjustToSnappingGrid(outHit.ImpactPoint));
			leftClickCount = 0;
		}
	}
}

void APlayerCharacter::LineTraceRight()
{
	GLog->Log("Line Trace right Building mode");
	FVector start = CameraComp->GetComponentLocation();
	FVector forwardVector = CameraComp->GetForwardVector();
	FVector end = (forwardVector * 400) + start;
	FCollisionQueryParams collisionsParms;


	bool isHit = GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECC_Visibility, collisionsParms);

	if (isHit)
	{
		mouseRightDown = false; // SO ONLY ONE OBJECT GETS DESTORYED
		//have building mode section here
		if (objectSpawned == true)
		{
			
		}
		else
		{
			//remove object
			mapControlRef->RemoveHitObject(outHit);
		}
		if (toolSelected)
		{
			APartBuilder* partBuilder = Cast<APartBuilder>(outHit.GetActor());
			if (partBuilder != nullptr)
			{
				toolSelected = false;
				toolRef = outHit.GetActor();
				partBuilder->PartBuilderSeclected();
			}
		}
	}
	else
	{
		////build even if click doesn't hit anything!!
		//if (meshNum != 0 && objectSpawned == true)
		//{
		//	if (toolSelected)
		//	{
		//		objectSpawned = false;
		//		GLog->Log("build Mesh");
		//		mapControlRef->BuildStaticMesh(FVector::ZeroVector);
		//	}
		//}
	}
	
}

void APlayerCharacter::LineTraceUpdateLocation()
{
	//GLog->Log("Update object Location");
	FVector start = CameraComp->GetComponentLocation();
	FVector forwardVector = CameraComp->GetForwardVector();
	FVector end = (forwardVector * 400) + start;
	FCollisionQueryParams collisionsParms;


	bool isHit = GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECC_Visibility, collisionsParms);

	if (isHit)
	{
		//have building mode section here
		if (outHit.ImpactPoint != lastLineTrace)
		{
			if (meshNum == 0)
			{
				placeableStorageRef->UpdateLocation(0, AdjustToSnappingGrid(outHit.ImpactPoint));
			}
			else
			{
				GLog->Log("Update Mesh Location");
				//mapControlRef->UpdateStaticMeshLocation(outHit.ImpactPoint);
				mapControlRef->UpdateICM(AdjustToSnappingGrid(outHit.ImpactPoint));
			}
		}		
		lastLineTrace = outHit.ImpactPoint;
	}

}

void APlayerCharacter::CheckDistanceFromTool()
{
	if (toolSelected == true)
	{
		FVector toolPos = toolRef->GetActorLocation();
		FVector playerPos = this->GetActorLocation();
		float dis = (toolPos - playerPos).Size();
		if (dis > 800)
		{
			APartBuilder* partBuilder = Cast<APartBuilder>(toolRef);
			if (partBuilder != nullptr)
			{
				partBuilder->PartBuilderDeseclected();
			}
			toolSelected = false;
		}
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckDistanceFromTool();
	if (inventroyOpen == false)
	{
		if(mouseRightDown == true)
		{
			if (!buildingMode) { LineTraceAdd(); }
			else { LineTraceRight(); }

		}
		if (mouseLeftDown == true)
		{
			if (!buildingMode) { LineTraceTake(); }
			else { LineTraceLeft(); }
		}
		if (buildingMode)
		{
			if (objectSpawned) { LineTraceUpdateLocation(); }
		}
	}
	if (GetActorLocation().Z < -500) { SetActorLocation(mapControlRef->loadedPlayerStart); }
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);//uses the inbuilt jump function
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("RightClick", IE_Pressed, this, &APlayerCharacter::RightClickDown);
	PlayerInputComponent->BindAction("RightClick", IE_Released, this, &APlayerCharacter::RightClickUp);
	PlayerInputComponent->BindAction("LeftClick", IE_Pressed, this, &APlayerCharacter::LeftClickDown);
	PlayerInputComponent->BindAction("LeftClick", IE_Released, this, &APlayerCharacter::LeftClickUp);

	PlayerInputComponent->BindAction("LeftArrow", IE_Pressed, this, &APlayerCharacter::LeftArrow);
	PlayerInputComponent->BindAction("RightArrow", IE_Pressed, this, &APlayerCharacter::RightArrow);
	PlayerInputComponent->BindAction("CycleMovement", IE_Pressed, this, &APlayerCharacter::CyleMovement);
	
	PlayerInputComponent->BindAction("BuildingMode", IE_Pressed, this, &APlayerCharacter::CycleBuildingMode);
	PlayerInputComponent->BindAction("MiddleMouse", IE_Pressed, this, &APlayerCharacter::MiddleMouseButton);
	PlayerInputComponent->BindAction("EKey", IE_Pressed, this, &APlayerCharacter::EKey);
	PlayerInputComponent->BindAction("Enter", IE_Pressed, this, &APlayerCharacter::EnterKey);
	PlayerInputComponent->BindAction("IKey", IE_Pressed, this, &APlayerCharacter::IKey);
	PlayerInputComponent->BindAction("TKey", IE_Pressed, this, &APlayerCharacter::TKey);
	PlayerInputComponent->BindAction("ZKey", IE_Pressed, this, &APlayerCharacter::ZKey);
	PlayerInputComponent->BindAction("CKey", IE_Pressed, this, &APlayerCharacter::CKey);
	PlayerInputComponent->BindAction("MouseWheelUp", IE_Pressed, this, &APlayerCharacter::MouseWheelUp);
	PlayerInputComponent->BindAction("MouseWheelDown", IE_Pressed, this, &APlayerCharacter::MouseWheelDown);
	
	PlayerInputComponent->BindAction("CtrlLeftArrow", IE_Pressed, this, &APlayerCharacter::CtrlLeftArrow);
	PlayerInputComponent->BindAction("CtrlRightArrow", IE_Pressed, this, &APlayerCharacter::CtrlRightArrow);
	PlayerInputComponent->BindAction("ShiftLeftArrow", IE_Pressed, this, &APlayerCharacter::ShiftLeftArrow);
	PlayerInputComponent->BindAction("ShiftRightArrow", IE_Pressed, this, &APlayerCharacter::ShiftRightArrow);
	PlayerInputComponent->BindAction("ShiftUpArrow", IE_Pressed, this, &APlayerCharacter::ShiftUpArrow);
	PlayerInputComponent->BindAction("ShiftDownArrow", IE_Pressed, this, &APlayerCharacter::ShiftDownArrow);
	
	PlayerInputComponent->BindAction("Crt", IE_Pressed, this, &APlayerCharacter::Ctrl);
	PlayerInputComponent->BindAction("Shift", IE_Pressed, this, &APlayerCharacter::ShiftDown);
	PlayerInputComponent->BindAction("Shift", IE_Released, this, &APlayerCharacter::ShiftUp);
	
	PlayerInputComponent->BindAction("ShiftMouseWheelUp", IE_Pressed, this, &APlayerCharacter::ShiftMouseWheelUp);
	PlayerInputComponent->BindAction("CrtlMouseWheelUp", IE_Pressed, this, &APlayerCharacter::CrtlMouseWheelUp);
	PlayerInputComponent->BindAction("Save", IE_Pressed, this, &APlayerCharacter::Save);
	PlayerInputComponent->BindAction("ESC", IE_Pressed, this, &APlayerCharacter::ESC);

	InputComponent->BindAxis("MoveX", this, &APlayerCharacter::MoveForward);
	InputComponent->BindAxis("MoveY", this, &APlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUpRate);
}

void APlayerCharacter::SetPlayerBuildObject(int index)
{
	lastMeshNum = meshNum;
	int clickCounter = leftClickCount;
	if ((index != lastMeshNum) && objectSpawned == true)
	{
		//remove the spawned object
		//leftClickCount = 2;
		//LineTraceLeft();	//spawn object
		
		//LineTraceRight();	//destory object
		GLog->Log("REMOVE OBJECT");
		if(leftClickCount> 0)
		{ 
			if (lastMeshNum != 0)
			{
				mapControlRef->RemoveICM();
				leftClickCount = 0;
				objectSpawned = false;
			}
			else if (lastMeshNum == 0)
			{
				if(placeableStorageRef != nullptr){ placeableStorageRef->RemoveSpawnObject(0); }				
			}
		}			
	}
	meshNum = index;
	//if (buildingMode == true)
	//{
	//	if (clickCounter == 1 && meshNum >= 0)
	//	{
	//		leftClickCount = 1;
	//		//LineTraceLeft();	//spawn object
	//	}
	//}
	//leftClickCount = 0;
	//GLog->Log(FString::FromInt(meshNum));
}

bool APlayerCharacter::GetBuildingMode()
{
	return buildingMode;
}

bool APlayerCharacter::GetInPartBuilder()
{
	return toolSelected;
}

void APlayerCharacter::SetToolSelected()
{
	if (toolSelected)
	{
		APartBuilder* partBuilder = Cast<APartBuilder>(toolRef);
		if (partBuilder != nullptr)
		{

			partBuilder->PartBuilderDeseclected();
		}
	}
	toolSelected = false;
	controlDown = false;
	
}

void APlayerCharacter::PartBuilderSetMat(int index)
{
	if (toolSelected)
	{
		APartBuilder* partBuilder = Cast<APartBuilder>(toolRef);
		if (partBuilder != nullptr)
		{
			partBuilder->SetMat(index);
		}
	}
}

void APlayerCharacter::PartBuilderSetName(FString name)
{
	if (toolSelected)
	{
		APartBuilder* partBuilder = Cast<APartBuilder>(toolRef);
		if (partBuilder != nullptr)
		{
			partBuilder->SetName(name);
		}
	}
}

void APlayerCharacter::PartBuilderSaveObject()
{
	if (toolSelected)
	{
		APartBuilder* partBuilder = Cast<APartBuilder>(toolRef);
		if (partBuilder != nullptr)
		{
			partBuilder->SaveHologram();
		}
	}
}

void APlayerCharacter::PartBuilderResetObject()
{
	if (toolSelected)
	{
		APartBuilder* partBuilder = Cast<APartBuilder>(toolRef);
		if (partBuilder != nullptr)
		{
			partBuilder->ResetHologram();
		}
	}
}

FVector APlayerCharacter::AdjustToSnappingGrid(FVector pos)
{
	if (snappingGrid == true)
	{
		FVector rv;
		rv.X = int(pos.X / snapGridDistance) * snapGridDistance;
		rv.Y = int(pos.Y / snapGridDistance) * snapGridDistance;
		rv.Z = int(pos.Z / snapGridDistance) * snapGridDistance;
		if (shiftDown == true) { rv.Z += snapGridDistance; }
		return rv;
	}
	return pos;
}

