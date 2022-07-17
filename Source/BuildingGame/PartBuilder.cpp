// Fill out your copyright notice in the Description page of Project Settings.


#include "PartBuilder.h"
#include "UObject/ConstructorHelpers.h"
#include "MeshData/DefaultMeshes.h"
#include "MapController.h"
#include "Lib/UVMapping.h"
#include "Lib/DefaultMarchingCubes.h"
#include "KismetProceduralMeshLibrary.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"

// Sets default values
APartBuilder::APartBuilder(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ThisScene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = ThisScene;
	//
	PartBuilderMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, (TEXT("PartBuilderMesh")));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh01(TEXT("StaticMesh'/Game/Models/PartBuilderPlaceHolder.PartBuilderPlaceHolder'"));//StaticMesh'/Game/Models/Trees/Tree2.Tree2'
	UStaticMesh* assset = Mesh01.Object;
	PartBuilderMesh->SetStaticMesh(assset);
	PartBuilderMesh->SetupAttachment(RootComponent);
	//PartBuilderMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	HologramMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("HologramMesh"));
	HologramMesh->SetupAttachment(RootComponent);
	HologramMesh->SetRelativeLocation(FVector(0, 0, 170));
	//Material intitialzer
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>Material1(TEXT("Material'/Game/Mat/HologramMat.HologramMat'"));
	HologramDefaultMat = Material1.Object;
	//Material'/Game/Mat/BasicStone.BasicStone'
	//Material'/Game/Mat/BasicStraw.BasicStraw'
	//Material'/Game/Mat/BasicWood.BasicWood'
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>Material2(TEXT("Material'/Game/Mat/BasicStone.BasicStone'"));
	Materials.Add(Material2.Object);
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>Material3(TEXT("Material'/Game/Mat/BasicStraw.BasicStraw'"));
	Materials.Add(Material3.Object);
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>Material4(TEXT("Material'/Game/Mat/BasicWood.BasicWood'"));
	Materials.Add(Material4.Object);

	Arrow = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, (TEXT("ArrowMesh")));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh02(TEXT("StaticMesh'/Game/Models/Arrow.Arrow'"));
	assset = Mesh02.Object;
	Arrow->SetStaticMesh(assset);

	UInstancedStaticMeshComponent* newInstanceMesh = ObjectInitializer.CreateDefaultSubobject<UInstancedStaticMeshComponent>(this, (TEXT("FullCube")));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh03(TEXT("StaticMesh'/Game/Models/InstanceMeshes/Full_Cube.Full_Cube'"));
	assset = Mesh03.Object;
	newInstanceMesh->SetStaticMesh(assset);
	newInstanceMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	newInstanceMesh->SetCastShadow(false);
	newInstanceMesh->SetMaterial(0, HologramDefaultMat);
	newInstanceMesh->SetupAttachment(RootComponent);
	newInstanceMesh->SetRelativeLocation(FVector(0, 0, 170));
	instancesMeshes.Add(newInstanceMesh);
	//
	PartBuilderDirectionArrow = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, (TEXT("PartBuilderDirectionArrow")));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh04(TEXT("StaticMesh'/Game/Models/ArrowLong.ArrowLong'	"));//StaticMesh'/Game/Models/Trees/Tree2.Tree2'
	assset = Mesh04.Object;
	PartBuilderDirectionArrow->SetStaticMesh(assset);
	PartBuilderDirectionArrow->SetupAttachment(RootComponent);
	PartBuilderDirectionArrow->CastShadow = false;
	PartBuilderDirectionArrow->SetRelativeLocation(FVector(0, 0, 50));
	PartBuilderDirectionArrow->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PartBuilderDirectionArrow1 = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, (TEXT("PartBuilderDirectionArrow1")));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh05(TEXT("StaticMesh'/Game/Models/ArrowLong1.ArrowLong1'"));//StaticMesh'/Game/Models/Trees/Tree2.Tree2'
	assset = Mesh05.Object;
	PartBuilderDirectionArrow1->SetStaticMesh(assset);
	PartBuilderDirectionArrow1->SetupAttachment(RootComponent);
	PartBuilderDirectionArrow1->CastShadow = false;
	PartBuilderDirectionArrow1->SetRelativeLocation(FVector(0, 0, 170));
	PartBuilderDirectionArrow1->SetVisibility(false);
	PartBuilderDirectionArrow1->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PartBuilderDirectionArrow2 = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, (TEXT("PartBuilderDirectionArrow2")));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh06(TEXT("StaticMesh'/Game/Models/ArrowLong1.ArrowLong1'"));//StaticMesh'/Game/Models/Trees/Tree2.Tree2'
	assset = Mesh06.Object;
	PartBuilderDirectionArrow2->SetStaticMesh(assset);
	PartBuilderDirectionArrow2->SetupAttachment(RootComponent);
	PartBuilderDirectionArrow2->CastShadow = false;
	PartBuilderDirectionArrow2->SetRelativeLocation(FVector(0, 0, 170));
	PartBuilderDirectionArrow2->SetRelativeRotation(FRotator(0, 90, 0));
	PartBuilderDirectionArrow2->SetVisibility(false);
	PartBuilderDirectionArrow2->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PartBuilderDirectionArrow3 = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, (TEXT("PartBuilderDirectionArrow3")));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh07(TEXT("StaticMesh'/Game/Models/ArrowLong1.ArrowLong1'"));//StaticMesh'/Game/Models/Trees/Tree2.Tree2'
	assset = Mesh07.Object;
	PartBuilderDirectionArrow3->SetStaticMesh(assset);
	PartBuilderDirectionArrow3->SetupAttachment(RootComponent);
	PartBuilderDirectionArrow3->CastShadow = false;
	PartBuilderDirectionArrow3->SetRelativeLocation(FVector(0, 0, 170));
	PartBuilderDirectionArrow3->SetRelativeRotation(FRotator(90, 0, 0));
	PartBuilderDirectionArrow3->SetVisibility(false);
	PartBuilderDirectionArrow3->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//scene capture intitilser
	SceneCapture = ObjectInitializer.CreateDefaultSubobject<USceneCaptureComponent2D>(this, (TEXT("Scene Capture")));
	SceneCapture->SetupAttachment(RootComponent);
	SceneCapture->SetRelativeLocation(FVector(-200, -200, 200));
	SceneCapture->SetRelativeRotation(FQuat(FRotator(0, 45, 0)));
	meshRenderTarget2D = ObjectInitializer.CreateDefaultSubobject<UTextureRenderTarget2D>(this, (TEXT("My Render Target")));
	meshRenderTarget2D->InitAutoFormat(64, 64);
	SceneCapture->TextureTarget = meshRenderTarget2D;
	//intialize variables
	hologramScale = 2;
	CenterVisible = false;
	Center = FVector::ZeroVector;
	hasBeenCreated = false;
	materialIndex = 0;

	//THREADING
	ShouldCreateMesh = false;
	CreateCells = false;
	CreatingCells = false;
	FilledCells = false;
	FillingCells = false;
	CreatedMesh = false;
	CreatingMesh = false;
}

// Called when the game starts or when spawned
void APartBuilder::BeginPlay()
{
	Super::BeginPlay();
//	DisplayHologram();
	CreateHologramMesh();
	RemoveHologram();
//	SceneCapture->CaptureScene();
	
}

// Called every frame
void APartBuilder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (ShouldCreateMesh == true)
	{
		if (CreateCells == false)
		{
			if (CreatingCells == false)
			{
				CreatingCells = true;//create cells
				(new FAutoDeleteAsyncTask<CreateCellsThreading>(100, 100, 100, 20, this))->StartBackgroundTask();
			}			
		}
		if(CreateCells == true)
		{
			if (FilledCells == false)
			{
				if (FillingCells == false)
				{
					FillingCells = true;//fill cells
					(new FAutoDeleteAsyncTask<FillCells_CubeThreading>(60, 60, 60, 20, this))->StartBackgroundTask();
				}
			}
			if (FilledCells == true)
			{
				if (CreatedMesh == false)
				{
					if (CreatingMesh == false)
					{
						CreatingCells = true; //creates mesh
						vertices.Empty();
						triangles.Empty();
						(new FAutoDeleteAsyncTask<CreateMeshThreading>(100, 100, 100, 20, this))->StartBackgroundTask();
					}
				}
				if (CreatedMesh)
				{
					//TArray<FVector> Normals;
					////TArray<FVector2D> UV;
					//TArray<FColor> Colour;
					//TArray<FProcMeshTangent> Tan;
					//HologramMesh->CreateMeshSection(0, vertices, triangles, Normals, hologramUVs, Colour, Tan, false);
					//HologramMesh->SetCastShadow(false);
					//HologramMesh->SetMaterial(0, HologramDefaultMat);
					GLog->Log("Create!!");
					if (partBuilderICM != nullptr)
					{
						partBuilderICM->RemoveICM(this);
					}
					CustomInstanceMesh cIM;
					InstanceCustomMeshData tim = cIM.CreateMesh(cells, 100, 100, 100, FVector::ZeroVector, 20, 0);
					partBuilderICM = new InstanceCustomMeshData(tim.GetPositions(), tim.GetInstancesType(), 0, 0);
					partBuilderICM->CreateICM((FVector(0, 0, 170)+GetActorLocation()), this);
					GLog->Log("Center!!! " + FString::FromInt(GetActorLocation().X));
					ShouldCreateMesh = false;
					//UpdateHologramMeshScale(1);	//this will update the mesh to scale factor 1
				}
			}
		}
	}	
}

void APartBuilder::SetMapController(AMapController* mapRef)
{
	mapController = mapRef;
}

void APartBuilder::PartBuilderSeclected()
{
	GLog->Log("Part builder seclected!!");
	DisplayHologram();
}

void APartBuilder::PartBuilderDeseclected()
{
	GLog->Log("Part builder deseclected!!");
	RemoveHologram();
}

void APartBuilder::ResetHologram()
{
	GLog->Log("Reset");
	Center = FVector::ZeroVector;	
	if (centerPoint != nullptr)
	{
		centerPoint->DestroyComponent();
	}	
	hasBeenCreated = false;
	currentSnappingPoint = 0;
	for (int i = 0; i < snappingMeshes.Num();i++)
	{
		snappingMeshes[i]->DestroyComponent();
	}
	snappingMeshes.Empty();
	SnappingPonitsDirection.Empty();
	CenterVisible = false;
	objectName = "";
	GLog->Log("Reset done");
	CreateHologramMesh();
}

void APartBuilder::IncreaseScale(int axis)
{
	//this will increase the scale
	switch (axis)
	{
	case 0:
		UpdateHologramScaleX(5, 1);	//x
		break;
	case 1:
		UpdateHologramScaleY(5, 1);	//y
		break;
	case 2:
		UpdateHologramScaleZ(5, 1);	//z
		break;
	}

	//UpdateHologramMesh();	//this wil now upate the mesh
}

void APartBuilder::DecreaseScale(int axis)
{
	//this will decrease the scale
	switch (axis)
	{
	case 0:
		UpdateHologramScaleX(5, -1);	//x
		break;
	case 1:
		UpdateHologramScaleY(5, -1);	//y
		break;
	case 2:
		UpdateHologramScaleZ(5, -1);	//z
		break;
	}

	//UpdateHologramMesh();	//this wil now upate the mesh
}

void APartBuilder::SaveHologram()
{
	GLog->Log("Save Mesh");
	AddSnappingPoints();	
	//TArray<FVector> UpdatedVertices = AdjustHologramToCenter();
	TArray<FVector> Normals;
	//TArray<FVector2D> UV;
	TArray<FColor> Colour;
	TArray<FProcMeshTangent> Tan;
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(hologramVertices, hologramTriangles, hologramUVs, Normals, Tan);
	//HologramMesh->UpdateMeshSection(0, UpdatedVertices, Normals,hologramUVs, Colour, Tan);
	HologramMesh->CastShadow = false;
//	newTexture = SceneCapture->TextureTarget->ConstructTexture2D(this, "Photo", EObjectFlags::RF_NoFlags, CTF_DeferCompression);
	mapController->AddMesh(cells,300,300,300,20,Center,SnappingPoints,SnappingPointsWidth, SnappingPonitsDirection,materialIndex,objectName);
	SnappingPoints.Empty();
	SnappingPointsWidth.Empty();	
	HologramMesh->UpdateMeshSection(0, vertices, Normals, hologramUVs, Colour, Tan);
}

void APartBuilder::CenterVisibility()
{
	//toggle visible
	if (CenterVisible == true) 
	{ 
		//hide center
		CenterVisible = false; 
		HideCenter();
	}
	else 
	{ 
		//show center
		CenterVisible = true; 
		ShowCenter();
	}
}

void APartBuilder::SpawnSnappingPoint()
{
	GLog->Log("Spawn snapping point");
	CreateSnappingPoint();
}

void APartBuilder::MoveSnappingPointForward(int axis)
{
	if (CenterVisible != true)
	{
		if (snappingMeshes.Num() != 0)
		{
			//this will move the snapping points forward
			switch (axis)
			{
			case 0:
				MoveSnappingPointX(10, 1);	//x
				break;
			case 1:
				MoveSnappingPointY(10, 1);	//y
				break;
			case 2:
				MoveSnappingPointZ(10, 1);	//z
				break;
			}
		}
	}
	else
	{
		//update the center point location
		//update center point
		GLog->Log("Move center!!");
		switch (axis)
		{
		case 0:
			MoveCenterPointX(10, 1);	//x
			break;
		case 1:
			MoveCenterPointY(10, 1);	//y
			break;
		case 2:
			MoveCenterPointZ(10, 1);	//z
			break;
		}
	}	
}

void APartBuilder::MoveSnappingPointBackwards(int axis)
{
	if (CenterVisible != true)
	{
		if (snappingMeshes.Num() != 0)
		{
			//this will move the snapping points backwards
			switch (axis)
			{
			case 0:
				MoveSnappingPointX(10, -1);	//x
				break;
			case 1:
				MoveSnappingPointY(10, -1);	//y
				break;
			case 2:
				MoveSnappingPointZ(10, -1);	//z
				break;
			}
		}
	}
	else
	{
		//update center point
		GLog->Log("Move center!!");
		switch (axis)
		{
		case 0:
			MoveCenterPointX(10, -1);	//x
			break;
		case 1:
			MoveCenterPointY(10, -1);	//y
			break;
		case 2:
			MoveCenterPointZ(10, -1);	//z
			break;
			break;
		}
	}
}

void APartBuilder::UpdateCurrentSnappingPoint(int change)
{
	GLog->Log("Update Current Snapping point");
	if (currentSnappingPoint + change < snappingMeshes.Num() && currentSnappingPoint + change > 0)
	{
		currentSnappingPoint += change;
	}
	else if (currentSnappingPoint + change < 0)
	{
		if (snappingMeshes.Num() == 0) { currentSnappingPoint = 0; }
		else{ currentSnappingPoint = snappingMeshes.Num() - 1; }		
	}
	else
	{
		currentSnappingPoint = 0;
	}
	if (snappingMeshes.IsValidIndex(currentSnappingPoint))
	{
		FVector pos = snappingMeshes[currentSnappingPoint]->GetRelativeLocation();
		PartBuilderDirectionArrow1->SetRelativeLocation(FVector(0, pos.Y, pos.Z));
		PartBuilderDirectionArrow2->SetRelativeLocation(FVector(pos.X, 0, pos.Z));
		PartBuilderDirectionArrow3->SetRelativeLocation(FVector(pos.X, pos.Y, 170));
		PartBuilderDirectionArrow1->SetVisibility(true);
		PartBuilderDirectionArrow2->SetVisibility(true);
		PartBuilderDirectionArrow3->SetVisibility(true);
	}

	GLog->Log(FString::FromInt(currentSnappingPoint));
}

void APartBuilder::UpdateSnappingPointRotationP(int direction)
{
	if (snappingMeshes.Num() > 0)
	{
		if (direction < 0)
		{
			//take
			newRotation.Roll = 0;
			newRotation -= FRotator(90, 0, 0);
			if (newRotation.Pitch == -360 || newRotation.Pitch == 360) { newRotation.Pitch = 0; }
			FQuat quat = FQuat(newRotation);
			snappingMeshes[currentSnappingPoint]->SetRelativeRotation(quat);
			SnappingPonitsDirection[currentSnappingPoint] = FindSnappingPointDirection(newRotation);
			GLog->Log("Rotation: Pitch " + FString::FromInt(newRotation.Pitch) + " Yaw " + FString::FromInt(newRotation.Yaw) + " Roll " + FString::FromInt(newRotation.Roll));

		}
		else
		{
			//add
			newRotation.Roll = 0;
			newRotation += FRotator(90, 0, 0);
			if (newRotation.Pitch == -360 || newRotation.Pitch == 360) { newRotation.Pitch = 0; }
			FQuat quat = FQuat(newRotation);
			snappingMeshes[currentSnappingPoint]->SetRelativeRotation(quat);
			SnappingPonitsDirection[currentSnappingPoint] = FindSnappingPointDirection(newRotation);
			GLog->Log("Rotation: Pitch " + FString::FromInt(newRotation.Pitch) + " Yaw " + FString::FromInt(newRotation.Yaw) + " Roll " + FString::FromInt(newRotation.Roll));

		}
	}	
}

void APartBuilder::UpdateSnappingPointRotationR(int direction)
{
	if (snappingMeshes.Num() > 0)
	{
		if (direction < 0)
		{
			//take
			newRotation.Pitch = 0;
			newRotation -= FRotator(0, 0, 90);
			if (newRotation.Roll == -360 || newRotation.Roll == 360) { newRotation.Roll = 0; }
			FQuat quat = FQuat(newRotation);
			snappingMeshes[currentSnappingPoint]->SetRelativeRotation(quat);
			SnappingPonitsDirection[currentSnappingPoint] = FindSnappingPointDirection(newRotation);
			GLog->Log("Rotation: Pitch " + FString::FromInt(newRotation.Pitch) + " Yaw " + FString::FromInt(newRotation.Yaw) + " Roll " + FString::FromInt(newRotation.Roll));
		}
		else
		{
			//add
			newRotation.Pitch = 0;
			newRotation += FRotator(0, 0, 90);
			if (newRotation.Roll == -360 || newRotation.Roll == 360) { newRotation.Roll = 0; }
			FQuat quat = FQuat(newRotation);
			snappingMeshes[currentSnappingPoint]->SetRelativeRotation(quat);
			SnappingPonitsDirection[currentSnappingPoint] = FindSnappingPointDirection(newRotation);
			GLog->Log("Rotation: Pitch " + FString::FromInt(newRotation.Pitch) + " Yaw " + FString::FromInt(newRotation.Yaw) + " Roll " + FString::FromInt(newRotation.Roll));
		}
	}
}

void APartBuilder::AddCell(FVector pos, float den)
{
	cells.Add(pos, den);
}

void APartBuilder::UpdateCell(FVector pos, float den)
{
	cells.Emplace(pos, den);
}

FVector4 APartBuilder::GetCellData(float x, float y, float z)
{
	FVector4 rv = FVector(x, y, z);
	rv.W = cells.FindRef(FVector(x, y, z));
	return rv;
}

int APartBuilder::AddVertex(FVector pos)
{
	if (vertices.Contains(pos))
	{
		return vertices.Find(pos);
	}
	return vertices.Add(pos);
}

void APartBuilder::AddTriangle(int index)
{
	triangles.Add(index);
}

void APartBuilder::SetMat(int index)
{
	if (index >= 0)
	{
		materialIndex = index;
		for (int i = 0; i < instancesMeshes.Num();i++)
		{
			instancesMeshes[i]->SetMaterial(0, Materials[materialIndex]);
		}
	}
	else if (index == -2)
	{
		//HologramMesh->SetMaterial(0, HologramDefaultMat);
		for (int i = 0; i < instancesMeshes.Num();i++)
		{
			instancesMeshes[i]->SetMaterial(0, Materials[materialIndex]);
		}
	}
	else if (index == -1)
	{
		//HologramMesh->SetMaterial(0, Materials[materialIndex]);
		for (int i = 0; i < instancesMeshes.Num();i++)
		{
			instancesMeshes[i]->SetMaterial(0, Materials[materialIndex]);
		}
	}

}

void APartBuilder::SetName(FString name)
{
	objectName = name;
}

int APartBuilder::AddIM(int instanceType, FVector location)
{
	//this just adds and instance of instance type at the lcoation returning the index at which it was added
	FTransform spawnPos;
	spawnPos.SetLocation(location);
	spawnPos.SetScale3D(FVector(1, 1, 1));
	FQuat quat(FVector::UpVector, float(0));
	spawnPos.SetRotation(quat);//rotation will need changing to rotate all the instances related to that vaule
	int index = instancesMeshes[instanceType]->AddInstanceWorldSpace(spawnPos);
//	currentMesh = instancesMeshes[instanceType];
	return index;
}

void APartBuilder::RemoveIm(int instanceType, int index)
{
	if (index != -1)
	{
		instancesMeshes[instanceType]->RemoveInstance(index);
	}
}

void APartBuilder::UpdateDirectionArrow(int direction)
{
	switch (direction)
	{
	case 0:
		PartBuilderDirectionArrow->SetRelativeRotation(FRotator(0,0,0));
		break;
	case 1:
		PartBuilderDirectionArrow->SetRelativeRotation(FRotator(0, 90, 0));
		break;
	case 2:
		PartBuilderDirectionArrow->SetRelativeRotation(FRotator(90, 0, 0));
		break;
	}
}

void APartBuilder::DisplayHologram()
{
	//first stage is to get the vertices and triangles of the mesh which will be dispalyed
	//CreateHologramMesh();
	for (int i = 0; i < instancesMeshes.Num();i++)
	{
		instancesMeshes[i]->SetVisibility(true);
	}
	PartBuilderDirectionArrow->SetVisibility(true);
	if (SnappingPoints.Num() > 0 || CenterVisible == true)
	{
		if (CenterVisible == true)
		{
			if (centerPoint != nullptr) { centerPoint->SetVisibility(true); }
		}
		
		PartBuilderDirectionArrow1->SetVisibility(true);
		PartBuilderDirectionArrow2->SetVisibility(true);
		PartBuilderDirectionArrow3->SetVisibility(true);
	}	
}

void APartBuilder::RemoveHologram()
{	
	for (int i = 0; i < instancesMeshes.Num();i++)
	{
		instancesMeshes[i]->SetVisibility(false);
	}
	if(centerPoint != nullptr){ centerPoint->SetVisibility(false); }
	PartBuilderDirectionArrow->SetVisibility(false);
	PartBuilderDirectionArrow1->SetVisibility(false);
	PartBuilderDirectionArrow2->SetVisibility(false);
	PartBuilderDirectionArrow3->SetVisibility(false);
}

void APartBuilder::CreateHologramMesh()
{
	cells.Empty();
	ShouldCreateMesh = true;
	CreateCells = false;
	CreatingCells = false;
	FilledCells = false;
	FillingCells = false;
	CreatedMesh = false;
	CreatingMesh = false;
}

void APartBuilder::UpdateHologramScaleX(float scaleAmount, float direction)
{
	float increase = scaleAmount * direction;
	if (increase > 0) { increase = 1; }
	else { increase = 0; }
	TMap<FVector, float> newCells;
	TArray<FVector> cellsPos;
	cells.GenerateKeyArray(cellsPos);
	//loop through all the cells
	
	int hX = 100 / 20-1;
	int lX = ((100 * -1) / 20) +1;
	int hY = 100 / 20-1;
	int lY = ((100 * -1) / 20) + 1;
	int hZ = 100 / 20-1;
	int lZ = ((100 * -1) / 20) + 1;	
	FVector cellPos;

	for (int z = lZ; z < hZ;z++)
	{
		//z loop
		cellPos.Z = z * 20;
		for (int x = lX;x < hX;x++)
		{
			//y loop
			cellPos.X = x * 20;
			for (int y = lY;y < hY;y++)
			{
				cellPos.Y = y * 20;
				if (cells.FindRef(cellPos) > 0.9)
				{
					UpdateHologramScale(cellPos, increase, hX*20, lX*20, 0,&newCells);			
				}
			}
		}
	}
	cells = newCells;
	CreatedMesh = false;
	CreatingMesh = false;
	ShouldCreateMesh = true;
}

void APartBuilder::UpdateHologramScaleY(float scaleAmount, float direction)
{
	float increase = scaleAmount * direction;
	if (increase > 0) { increase = 1; }
	else { increase = 0; }
	TMap<FVector, float> newCells;
	TArray<FVector> cellsPos;
	cells.GenerateKeyArray(cellsPos);
	//loop through all the cells

	int hX = 100 / 20 - 1;
	int lX = ((100 * -1) / 20) + 1;
	int hY = 100 / 20 - 1;
	int lY = ((100 * -1) / 20) + 1;
	int hZ = 100 / 20 - 1;
	int lZ = ((100 * -1) / 20) + 1;
	FVector cellPos;
	for (int z = lZ; z <= hZ;z++)
	{
		//z loop
		cellPos.Z = z * 20;
		for (int x = lX;x <= hX;x++)
		{
			//y loop
			cellPos.X = x * 20;
			for (int y = lY;y <= hY;y++)
			{
				cellPos.Y = y * 20;
				if (cells.FindRef(cellPos) > 0.9)
				{
					UpdateHologramScale(cellPos, increase, hY * 20, lY * 20, 1, &newCells);
				}
			}
		}
	}
	cells = newCells;
	CreatedMesh = false;
	CreatingMesh = false;
	ShouldCreateMesh = true;
}

void APartBuilder::UpdateHologramScaleZ(float scaleAmount, float direction)
{
	float increase = scaleAmount * direction;
	if (increase > 0) { increase = 1; }
	else { increase = 0; }
	TMap<FVector, float> newCells;
	TArray<FVector> cellsPos;
	cells.GenerateKeyArray(cellsPos);
	//loop through all the cells

	int hX = 100 / 20 - 1;
	int lX = ((100 * -1) / 20) + 1;
	int hY = 100 / 20 - 1;
	int lY = ((100 * -1) / 20) + 1;
	int hZ = 100 / 20 - 1;
	int lZ = ((100 * -1) / 20) + 1;
	FVector cellPos;

	for (int z = lZ; z < hZ;z++)
	{
		//z loop
		cellPos.Z = z * 20;
		for (int x = lX;x < hX;x++)
		{
			//y loop
			cellPos.X = x * 20;
			for (int y = lY;y < hY;y++)
			{
				cellPos.Y = y * 20;
				if (cells.FindRef(cellPos) > 0.9)
				{
					UpdateHologramScale(cellPos, increase, hZ * 20, lZ * 20, 2, &newCells);
				}
			}
		}
	}
	cells = newCells;
	CreatedMesh = false;
	CreatingMesh = false;
	ShouldCreateMesh = true;
}

void APartBuilder::UpdateHologramScale(FVector cellPos, int increase,float hP,float lP,int axis, TMap<FVector, float>* newCells)
{
	float cellPosAxis = 0;
	FVector mult = FVector::ZeroVector;
	switch (axis)
	{
	case 0:
		cellPosAxis = cellPos.X;
		mult.X = 1;
		break;
	case 1:
		cellPosAxis = cellPos.Y;
		mult.Y = 1;
		break;
	case 2:
		cellPosAxis = cellPos.Z;
		mult.Z = 1;
		break;
	}
	//TMap<FVector,float> newCells;
	//soild cell so update
	if (increase == 1)
	{
		GLog->Log("Increase!!");
		if (newCells->Contains(cellPos)) { newCells->Emplace(cellPos, 1); }
		else { newCells->Add(cellPos, increase); }
		//add 
		if (cellPosAxis > 0)
		{//greater than 0 so add
			FVector scaledCell = cellPos;
			float newCellPos = cellPosAxis;
			newCellPos += 20;
			//scaledCell.X += 20;
			scaledCell += (20 * mult);	//only add to the axis we are working on
			if (newCellPos < hP)
			{
				if (newCells->Contains(scaledCell)) { newCells->Emplace(scaledCell, 1); }
				else { newCells->Add(scaledCell, increase); }
			}
		}
		else if (cellPosAxis < 0)
		{//less than 0 so take
			FVector scaledCell = cellPos;
			//scaledCell.X -= 20;
			float newCellPos = cellPosAxis;
			newCellPos -= 20;
			scaledCell -= (20 * mult);	//only add to the axis we are working on
			if (newCellPos > lP)
			{
				if (newCells->Contains(scaledCell)) { newCells->Emplace(scaledCell, 1); }
				else { newCells->Add(scaledCell, increase); }
			}
		}
		else
		{// cell pos is 0 so add on each side
			FVector scaledCell = cellPos;
		//	scaledCell.X -= 20;
			float newCellPos = cellPosAxis;
			newCellPos -= 20;
			scaledCell -= (20 * mult);	//only add to the axis we are working on
			if (newCells->Contains(scaledCell)) { newCells->Emplace(scaledCell, 1); }
			else { newCells->Add(scaledCell, increase); }
			scaledCell = cellPos;
			//scaledCell.X += 20;
			scaledCell += 20 * mult;	//only add to the axis we are working on
			if (newCells->Contains(scaledCell)) { newCells->Emplace(scaledCell, 1); }
			else { newCells->Add(scaledCell, increase); }
		}
	}
	else
	{
		GLog->Log("Decrease!!");
		if (!newCells->Contains(cellPos)) { newCells->Add(cellPos, 0); }
		if (cellPosAxis > 0)
		{//greater than 0 so add
			FVector scaledCell = cellPos;
			//scaledCell.X -= 20;
			float newCellPos = cellPosAxis;
			newCellPos -= 20;
			scaledCell -= (20 * mult);	//only add to the axis we are working on
			if (newCellPos != 0)
			{
				if (!newCells->Contains(scaledCell)) { newCells->Add(scaledCell, 1); }
				else { newCells->Emplace(scaledCell, 1); }
			}
			//else { newCells.Emplace(scaledCell, 1); }
		}
		else if (cellPosAxis < 0)
		{//less than 0 so take
			FVector scaledCell = cellPos;
			//scaledCell.X += 20;
			float newCellPos = cellPosAxis;
			newCellPos += 20;
			scaledCell += (20 * mult);	//only add to the axis we are working on
			if (newCellPos != 0)
			{
				if (!newCells->Contains(scaledCell)) { newCells->Add(scaledCell, 1); }
				else { newCells->Emplace(scaledCell, 1); }
			}
			//else { newCells.Emplace(scaledCell, 1); }
		}
		else
		{
			newCells->Emplace(cellPos, 1);
		}
	}
}

bool APartBuilder::CheckScaler(FVector scaler, FVector pos)
{
	if (scaler.X != 0)
	{
		if (pos.X < scaler.X) { return true; }
	}
	if (scaler.Y != 0)
	{
		if (pos.Y < scaler.Y) { return true; }
	}
	if (scaler.Z != 0)
	{
		if (pos.Z < scaler.Z) { return true; }
	}
	return false;
}

//TArray<FVector> APartBuilder::AdjustHologramToCenter()
//{
//	TArray<FVector> Vertices = vertices;
//	for (int i = 0; i < Vertices.Num();i++)
//	{
//		Vertices[i] -= Center;
//	}	
//	return Vertices;
//}

void APartBuilder::ShowCenter()
{
	if (hasBeenCreated == false)
	{
		CreateCenter();
		hasBeenCreated = true;
	}
	else
	{
		centerPoint->SetVisibility(true);
		FVector pos = centerPoint->GetRelativeLocation();
		PartBuilderDirectionArrow1->SetRelativeLocation(FVector(0, pos.Y, pos.Z));
		PartBuilderDirectionArrow2->SetRelativeLocation(FVector(pos.X, 0, pos.Z));
		PartBuilderDirectionArrow3->SetRelativeLocation(FVector(pos.X, pos.Y, 170));
		PartBuilderDirectionArrow1->SetVisibility(true);
		PartBuilderDirectionArrow2->SetVisibility(true);
		PartBuilderDirectionArrow3->SetVisibility(true);

	}
}

void APartBuilder::HideCenter()
{

	if (hasBeenCreated == true)
	{
		centerPoint->SetVisibility(false);
		if (snappingMeshes.IsValidIndex(currentSnappingPoint))
		{
			FVector pos = snappingMeshes[currentSnappingPoint]->GetRelativeLocation();
			PartBuilderDirectionArrow1->SetRelativeLocation(FVector(0, pos.Y, pos.Z));
			PartBuilderDirectionArrow2->SetRelativeLocation(FVector(pos.X, 0, pos.Z));
			PartBuilderDirectionArrow3->SetRelativeLocation(FVector(pos.X, pos.Y, 170));
		}
		else
		{
			PartBuilderDirectionArrow1->SetVisibility(false);
			PartBuilderDirectionArrow2->SetVisibility(false);
			PartBuilderDirectionArrow3->SetVisibility(false);
		}
	}
}

void APartBuilder::CreateCenter()
{	
	GLog->Log("Create center point");
	centerPoint = NewObject<UProceduralMeshComponent>(this);
	centerPoint->RegisterComponent();
	centerPoint->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	DefaultMeshes DM;
	centerPointVertices = DM.GetVerticesSquareMesh(5);
	centerPointTriangles = DM.GetTrianglesSquareMesh();
	TArray<FVector> Normals;
	TArray<FVector2D> UV;
	TArray<FColor> Colour;
	TArray<FProcMeshTangent> Tan;		
	centerPoint->CreateMeshSection(0, centerPointVertices, centerPointTriangles, Normals, UV, Colour, Tan, false);	
	centerPoint->SetRelativeLocation(FVector(0,0,170));
	FVector pos = centerPoint->GetRelativeLocation();
	PartBuilderDirectionArrow1->SetRelativeLocation(FVector(0, pos.Y, pos.Z));
	PartBuilderDirectionArrow2->SetRelativeLocation(FVector(pos.X, 0, pos.Z));
	PartBuilderDirectionArrow3->SetRelativeLocation(FVector(pos.X, pos.Y, 170));
	GLog->Log("Created center point");
	PartBuilderDirectionArrow1->SetVisibility(true);
	PartBuilderDirectionArrow2->SetVisibility(true);
	PartBuilderDirectionArrow3->SetVisibility(true);

}

void APartBuilder::MoveCenterPointX(float moveAmount, float direction)
{
	GLog->Log("Moveing center!!");
	FVector newPos = centerPoint->GetRelativeLocation();
	if (direction < 0) { newPos.X -= moveAmount; Center.X -= moveAmount; }
	else { newPos.X += moveAmount; Center.X += moveAmount;}
	
	centerPoint->SetRelativeLocation(newPos);
	GLog->Log("Moved center!!");
	FVector pos = centerPoint->GetRelativeLocation();
	PartBuilderDirectionArrow1->SetRelativeLocation(FVector(0, pos.Y, pos.Z));
	PartBuilderDirectionArrow2->SetRelativeLocation(FVector(pos.X, 0, pos.Z));
	PartBuilderDirectionArrow3->SetRelativeLocation(FVector(pos.X, pos.Y, 170));
}

void APartBuilder::MoveCenterPointY(float moveAmount, float direction)
{
	GLog->Log("Moveing center!!");
	FVector newPos = centerPoint->GetRelativeLocation();
	if (direction < 0) { newPos.Y -= moveAmount; Center.Y -= moveAmount; }
	else { newPos.Y += moveAmount; Center.Y += moveAmount; }

	centerPoint->SetRelativeLocation(newPos);
	FVector pos = centerPoint->GetRelativeLocation();
	PartBuilderDirectionArrow1->SetRelativeLocation(FVector(0, pos.Y, pos.Z));
	PartBuilderDirectionArrow2->SetRelativeLocation(FVector(pos.X, 0, pos.Z));
	PartBuilderDirectionArrow3->SetRelativeLocation(FVector(pos.X, pos.Y, 170));
}

void APartBuilder::MoveCenterPointZ(float moveAmount, float direction)
{
	GLog->Log("Moveing center!!");
	FVector newPos = centerPoint->GetRelativeLocation();
	if (direction < 0) { newPos.Z -= moveAmount; Center.Z -= moveAmount; }
	else { newPos.Z += moveAmount; Center.Z += moveAmount; }

	centerPoint->SetRelativeLocation(newPos);
	FVector pos = centerPoint->GetRelativeLocation();
	PartBuilderDirectionArrow1->SetRelativeLocation(FVector(0, pos.Y, pos.Z));
	PartBuilderDirectionArrow2->SetRelativeLocation(FVector(pos.X, 0, pos.Z));
	PartBuilderDirectionArrow3->SetRelativeLocation(FVector(pos.X, pos.Y, 170));
}

void APartBuilder::CreateSnappingPoint()
{
	//first create new proc mesh
	//UProceduralMeshComponent* newProcMesh = NewObject<UProceduralMeshComponent>(this);
	UStaticMeshComponent* newArrow = NewObject<UStaticMeshComponent>(this);
	newArrow->SetStaticMesh(Arrow->GetStaticMesh());
	//newArrow = Mesh01.Object;
	//StaticMesh'/Game/Models/Arrow.Arrow'
	newArrow->RegisterComponent();
	newArrow->AttachToComponent(RootComponent,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	snappingMeshes.Add(newArrow);
	SnappingPonitsDirection.Add(FVector(0,0,-1));
	newArrow->SetRelativeLocation(HologramMesh->GetRelativeLocation());
	if (snappingMeshes.Num() == 1)
	{
		FVector pos = newArrow->GetRelativeLocation();
		PartBuilderDirectionArrow1->SetRelativeLocation(FVector(0, pos.Y, pos.Z));
		PartBuilderDirectionArrow2->SetRelativeLocation(FVector(pos.X, 0, pos.Z));
		PartBuilderDirectionArrow3->SetRelativeLocation(FVector(pos.X, pos.Y, 170));

		PartBuilderDirectionArrow1->SetVisibility(true);
		PartBuilderDirectionArrow2->SetVisibility(true);
		PartBuilderDirectionArrow3->SetVisibility(true);
	}
}

void APartBuilder::MoveSnappingPointX(float moveAmount, float direction)
{
	FVector newPos = snappingMeshes[currentSnappingPoint]->GetRelativeLocation();
	if (direction < 0) { newPos.X -= moveAmount; }
	else { newPos.X += moveAmount; }
	 
	snappingMeshes[currentSnappingPoint]->SetRelativeLocation(newPos);
	FVector pos = snappingMeshes[currentSnappingPoint]->GetRelativeLocation();
	PartBuilderDirectionArrow1->SetRelativeLocation(FVector(0, pos.Y, pos.Z));
	PartBuilderDirectionArrow2->SetRelativeLocation(FVector(pos.X, 0, pos.Z));
	PartBuilderDirectionArrow3->SetRelativeLocation(FVector(pos.X, pos.Y, 170));
}

void APartBuilder::MoveSnappingPointY(float moveAmount, float direction)
{
	FVector newPos = snappingMeshes[currentSnappingPoint]->GetRelativeLocation();
	if (direction < 0) { newPos.Y -= moveAmount; }
	else { newPos.Y += moveAmount; }

	snappingMeshes[currentSnappingPoint]->SetRelativeLocation(newPos);
	FVector pos = snappingMeshes[currentSnappingPoint]->GetRelativeLocation();
	PartBuilderDirectionArrow1->SetRelativeLocation(FVector(0, pos.Y, pos.Z));
	PartBuilderDirectionArrow2->SetRelativeLocation(FVector(pos.X, 0, pos.Z));
	PartBuilderDirectionArrow3->SetRelativeLocation(FVector(pos.X, pos.Y, 170));
}

void APartBuilder::MoveSnappingPointZ(float moveAmount, float direction)
{
	FVector newPos = snappingMeshes[currentSnappingPoint]->GetRelativeLocation();
	if (direction < 0) { newPos.Z -= moveAmount; }
	else { newPos.Z += moveAmount; }

	snappingMeshes[currentSnappingPoint]->SetRelativeLocation(newPos);
	FVector pos = snappingMeshes[currentSnappingPoint]->GetRelativeLocation();
	PartBuilderDirectionArrow1->SetRelativeLocation(FVector(0, pos.Y, pos.Z));
	PartBuilderDirectionArrow2->SetRelativeLocation(FVector(pos.X, 0, pos.Z));
	PartBuilderDirectionArrow3->SetRelativeLocation(FVector(pos.X, pos.Y, 170));
}

void APartBuilder::AddSnappingPoints()
{
	for (int i = 0;i < snappingMeshes.Num();i++)
	{
		FVector newPos = snappingMeshes[i]->GetRelativeLocation() - HologramMesh->GetRelativeLocation();
		newPos -= Center;
		GLog->Log("Snapping ponits: " + FString::FromInt(newPos.X) + " Y " + FString::FromInt(newPos.Y) + " Z " + FString::FromInt(newPos.Z));
		//FRotator rotation = snappingMeshes[i]->GetRelativeRotation();
		GLog->Log("Rotation: Pitch " + FString::FromInt(SnappingPonitsDirection[i].X) + " Yaw " + FString::FromInt(SnappingPonitsDirection[i].Y) + " Roll " + FString::FromInt(SnappingPonitsDirection[i].Z));

		SnappingPoints.Add(newPos);
		SnappingPointsWidth.Add(40);
		//SnappingPonitsDirection.Add(FindSnappingPointDirection(rotation));
	}
}

FVector APartBuilder::FindSnappingPointDirection(FRotator rotation)
{
	float pitch = rotation.Pitch;
	float roll = rotation.Roll;
	if (pitch == 0) 
	{ 		
		if(roll == 0){ return FVector(0, 0, -1); }	//z-
		if(roll == -180 || roll == 180){ return FVector(0, 0, 1); }	//z+
		if(roll == -90 || roll == 90){ return FVector(0, -1, 0); }	//Y+
		if(roll == -270 || roll == 270){ return FVector(0, 1, 0); }	//Y-
	}
	else
	{
		if (pitch == -180 || pitch == 180) { return FVector(0, 0, 1); }	//z+
		if (pitch == -90 || pitch == 90) { return FVector(-1, 0, 0); }	//x-
		if (pitch == -270 || pitch == 270) { return FVector(1, 0, 0); }	//x+
	}
	return FVector();
}


