// Fill out your copyright notice in the Description page of Project Settings.


#include "MapController.h"
#include "UObject/ConstructorHelpers.h"
#include <BuildingGame/New folder/SaveMeshData.h>
#include "Kismet/GameplayStatics.h"
#include "ProceduralMeshConversion.h"
#include "Player/PlayerCharacter.h"
#include "MeshData/SnappingPointsData.h"
#include "DrawDebugHelpers.h"
#include "Placeables/SnappingSystem.h"
#include "Save/SaveMapData.h"
#include "KismetProceduralMeshLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Placeables/PlaceableStorage.h"
#include "HighResScreenshot.h"
#include "ChunkController.h"

// Sets default values
AMapController::AMapController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ThisScene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = ThisScene;

	UInstancedStaticMeshComponent* newInstanceMesh = ObjectInitializer.CreateDefaultSubobject<UInstancedStaticMeshComponent>(this, (TEXT("newMesh")));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh01(TEXT("StaticMesh'/Game/Models/InstanceMeshes/Full_Cube.Full_Cube'"));//StaticMesh'/Game/Models/Trees/Tree2.Tree2'
	UStaticMesh* assset = Mesh01.Object;	
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>Material1(TEXT("Material'/Game/Mat/GhostMat.GhostMat'"));
	ghostMat = Material1.Object;
	newInstanceMesh->SetStaticMesh(assset);
	newInstanceMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	newInstanceMesh->SetCastShadow(false);
	newInstanceMesh->SetMaterial(0, ghostMat);
	
	instancedMeshes.Add(newInstanceMesh);
	
	ICMarrows = ObjectInitializer.CreateDefaultSubobject<UInstancedStaticMeshComponent>(this, (TEXT("Arrow Mesh")));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh02(TEXT("StaticMesh'/Game/Models/Arrow.Arrow'"));//StaticMesh'/Game/Models/Trees/Tree2.Tree2'
	assset = Mesh02.Object;
	ICMarrows->SetStaticMesh(assset);
	ICMarrows->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ICMarrows->SetCastShadow(false);

	viewDistance = 10;
	chunkSize = 9 * 50;
	meshCount = 0;
	currentInstanceMeshIndex = 0;
	loadedAllMeshes = false;
	loadingGame = false;
	hasLoaded = false;
	loadedPlayerStart = FVector(0, 0, 10000);
	placeableStorageRef = new PlaceableStorage();
}

// Called when the game starts or when spawned
void AMapController::BeginPlay()
{
	Super::BeginPlay();

	pController = GetWorld()->GetFirstPlayerController();
	ACharacter* BasicCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	APlayerCharacter* myCharacter = Cast<APlayerCharacter>(BasicCharacter);
	if (myCharacter != nullptr)
	{
		myCharacter->mapControlRef = this;
	}
	ObjectNames.Add(FString("Part Builder"));
	//LoadGame();
}

// Called every frame
void AMapController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	pController = GetWorld()->GetFirstPlayerController();
	if (pController != NULL)
	{

		FVector pLocation = pController->GetPawn()->GetActorLocation();	///INGORE ERROR ON THIS LINE

		playerLocationChunk.X = FMath::RoundToInt((pLocation.X + chunkSize / 2) / chunkSize);//float(x);
		playerLocationChunk.Y = FMath::RoundToInt((pLocation.Y + chunkSize / 2) / chunkSize);//float(y);	
	}
	if (loadedAllMeshes == true)
	{
		//only start spawning chunks once meshes have been loaded
		SpawnChunks();
		RemoveChunks();
		UpdateChunksLOD();
	}
	if (hasLoaded == false)
	{
		HasChunkUnderPlayerLoaded();
		if (loadingGame == false)
		{
			//check if the chunk under the player has loaded		
			if (pController != NULL)
			{
				pController->GetPawn()->SetActorLocation(loadedPlayerStart);
			}
		}
		else
		{
			hasLoaded = true;
		}
	}
	
}

AChunk* AMapController::GetChunk(FVector chunkPos)
{
	if (chunkArray.Contains(FVector2D(chunkPos.X,chunkPos.Y)))
	{
		return chunkArray.FindRef(FVector2D(chunkPos.X, chunkPos.Y))->FindChunk(chunkPos);
	}
	return nullptr;
}

void AMapController::SaveGame()
{
	//save all the data
	//save data in this class
	USaveMapData* SaveGameDataInstance = Cast<USaveMapData>(UGameplayStatics::CreateSaveGameObject(USaveMapData::StaticClass()));
	SaveGameDataInstance->StaticMeshesCount = meshCount;
	SaveGameDataInstance->PlayerPos = pController->GetPawn()->GetActorLocation();
	SaveGameDataInstance->PlayerRot = pController->GetPawn()->GetActorRotation();
	//create save game name
	FString chunkName = gameName + "_MapData";
	//save the the data 
	UGameplayStatics::SaveGameToSlot(SaveGameDataInstance, chunkName, 0);
	//save all the chunks
	SaveAllchunks();
}

void AMapController::LoadGame()
{
	GLog->Log("GAME NAME: " + gameName);
	USaveMapData* SaveGameDataInstance = Cast<USaveMapData>(UGameplayStatics::CreateSaveGameObject(USaveMapData::StaticClass()));
	//create save game name
	FString chunkName = gameName + "_MapData";
	//load the save game data into our variable
	SaveGameDataInstance = Cast<USaveMapData>(UGameplayStatics::LoadGameFromSlot(chunkName, 0));
	if (SaveGameDataInstance != nullptr)
	{
		loadingGame = true;
		GLog->Log("Load chunk!!");
		meshCount = SaveGameDataInstance->StaticMeshesCount;
		
		pController->GetPawn()->SetActorLocation(SaveGameDataInstance->PlayerPos);
		pController->GetPawn()->SetActorRotation(SaveGameDataInstance->PlayerRot);
		loadedPlayerStart = SaveGameDataInstance->PlayerPos;
		GLog->Log("Loaded player Pos" + FString::FromInt(loadedPlayerStart.X) + FString::FromInt(loadedPlayerStart.Y) + FString::FromInt(loadedPlayerStart.Z));
		if (meshCount > 0)
		{
			//generate the staticmeshes
			GLog->Log("Create Static Mehses");
			for (int i = 0; i < meshCount; i++)
			{
				GLog->Log("Load Mesh");
				//load the mesh
				USaveMeshData* SaveMeshData = Cast<USaveMeshData>(UGameplayStatics::CreateSaveGameObject(USaveMeshData::StaticClass()));

				//create save game name
				chunkName = gameName + "Mesh_" + FString::FromInt(i);
				//load the save game data into our variable
				SaveMeshData = Cast<USaveMeshData>(UGameplayStatics::LoadGameFromSlot(chunkName, 0));
				if (SaveMeshData != nullptr)
				{
					GLog->Log("Load mesh");
					InstanceCustomMeshData* newInstanceMesh = new InstanceCustomMeshData(SaveMeshData->Locations, SaveMeshData->InstanceType,SaveMeshData->ICMID,SaveMeshData->MaterialIndex);
					int id = instanceMeshData.Add(newInstanceMesh);					
					//load the snapping Points
					SnappingPointsData* newPoint = new SnappingPointsData(SaveMeshData->SnappingPoints, SaveMeshData->SnappingPointsWidth
						, SaveMeshData->SnappingPointsDirection);
					SnappingPoints.Add(newPoint);
					if (SaveMeshData->ObjectName.Len() == 0){ ObjectNames.Add("Mesh_" + FString::FromInt(i)); }
					else { ObjectNames.Add(SaveMeshData->ObjectName); }
				}
			}			
		}
	}
	else
	{
		loadingGame = false;
	}
	loadedAllMeshes = true;
}

void AMapController::SpawnChunks()
{
	//need to work out the current render distance 
	for (int currentView = 1; currentView < viewDistance; currentView++)
	{
		FVector2D chunkLocation{ 0.f };
		for (int x = (currentView * -1) + playerLocationChunk.X; x < currentView + playerLocationChunk.X; x++)
		{
			chunkLocation.X = x * chunkSize/* + chunkSize/2*/;
			for (int y = (currentView * -1) + playerLocationChunk.Y; y < (currentView + playerLocationChunk.Y); y++)
			{
				chunkLocation.Y = y * chunkSize/* + chunkSize/2*/;
				//check location to see if chunk has been created
				if (!chunkArray.Contains(chunkLocation))
				{
					SpawnChunk(FVector(chunkLocation.X, chunkLocation.Y, 0));
				}
			}
		}
	}	
}

void AMapController::RemoveChunks()
{
	FVector chunkLocation{ 0.f };
	//updates the chunks which should be loaded
	float minVewDistanceX = (((viewDistance + 1) * -1) + playerLocationChunk.X) * chunkSize;
	float minVewDistanceY = (((viewDistance + 1) * -1) + playerLocationChunk.Y) * chunkSize;
	float maxVewDistanceX = ((viewDistance + 1) + playerLocationChunk.X) * chunkSize;
	float maxVewDistanceY = ((viewDistance + 1) + playerLocationChunk.Y) * chunkSize;
	TArray<FVector2D> chunkLocations;
	chunkArray.GenerateKeyArray(chunkLocations);
	//checks the chunks to see if they are outside this range
	for (int i = 0; i < chunkLocations.Num();i++)
	{
		if (chunkLocations[i].X < minVewDistanceX || chunkLocations[i].X > maxVewDistanceX
			|| chunkLocations[i].Y < minVewDistanceY || chunkLocations[i].Y > maxVewDistanceY)
		{
			//remove this chunk
			RemoveChunk(chunkLocations[i]);
		}
	}
}

void AMapController::SpawnChunk(FVector location)
{
	if (chunkArray.Num() == 0)
	{
		//GLog->Log("Spawn first chunk!!");
		FRotator rotation{ 0.0f };
		//AChunk* newActor = GetWorld()->SpawnActor<AChunk>(location, rotation);
		//newActor->mapCont = this;
		TArray<UStaticMesh*> meshes;
		for (int i = 0; i < instancedMeshes.Num();i++)
		{
			meshes.Add(instancedMeshes[i]->GetStaticMesh());
		}
		AChunkController* newChunkCol = GetWorld()->SpawnActor<AChunkController>(location, rotation);
		newChunkCol->SpawnChunks(this, meshes, gameName);
		currentChunk = newChunkCol;

		chunkArray.Add(FVector2D(location.X,location.Y), newChunkCol);
	}
	else
	{
		if (currentChunk != nullptr)
		{
			if (currentChunk->spawnedAllChunks == true)
			{
				//GLog->Log("Faild Spawn new chunk!!");
				FRotator rotation{ 0.0f };
				//AChunk* newActor = GetWorld()->SpawnActor<AChunk>(location, rotation);
				//newActor->mapCont = this;
				TArray<UStaticMesh*> meshes;
				for (int i = 0; i < instancedMeshes.Num();i++)
				{
					meshes.Add(instancedMeshes[i]->GetStaticMesh());
				}
				AChunkController* newChunkCol = GetWorld()->SpawnActor<AChunkController>(location, rotation);
				newChunkCol->SpawnChunks(this, meshes, gameName);
				currentChunk = newChunkCol;

				chunkArray.Add(FVector2D(location.X, location.Y), newChunkCol);
			}
			//else{ GLog->Log("chunk still spawning"); }
		}
	}
}

void AMapController::RemoveChunk(FVector2D location)
{
	TArray<AChunkController*> chunks;
	chunkArray.GenerateValueArray(chunks);
	AChunkController* chunkCol = chunkArray.FindRef(location);
	if (chunkCol != nullptr)
	{
		if (chunkCol->RemoveChunks() == true)
		{
			chunkCol->Destroy();
			//remove from array
			chunkArray.Remove(location);
		}
	}
}

void AMapController::UpdateChunksLOD()
{
	int LODDistance = 3;	//this could be an array when there are more vaules
	//need to work out the current render distance 
	for (int currentView = 1; currentView < viewDistance; currentView++)
	{
		FVector2D chunkLocation{ 0.f };
		for (int x = (currentView * -1) + playerLocationChunk.X; x < currentView + playerLocationChunk.X; x++)
		{
			chunkLocation.X = x * chunkSize/* + chunkSize/2*/;
			for (int y = (currentView * -1) + playerLocationChunk.Y; y < (currentView + playerLocationChunk.Y); y++)
			{
				chunkLocation.Y = y * chunkSize/* + chunkSize/2*/;				
				//check location to see if chunk has been created
				if (chunkArray.Contains(chunkLocation))
				{
					AChunkController* chunkCont = chunkArray.FindRef(chunkLocation);
					if (chunkCont != nullptr)
					{
						//check if chunk is with LOD range
						if ((x > (LODDistance * -1 + playerLocationChunk.X) && x < LODDistance + playerLocationChunk.X)
							&& (y >(LODDistance * -1 + playerLocationChunk.Y) && y < LODDistance + playerLocationChunk.Y))
						{	//within range 
							chunkCont->UpdateChunksLOD(0);		
							chunkCont->ToggleTick(true);
							chunkCont->UpdateChunksTargetVertiecs(10);
						}
						else
						{	//out of range
							chunkCont->UpdateChunksLOD(1);	
							chunkCont->ToggleTick(false);
							int vertexLODDistance = LODDistance + 2;
							if ((x > (vertexLODDistance * -1 + playerLocationChunk.X) && x < vertexLODDistance + playerLocationChunk.X)
								&& (y > (vertexLODDistance * -1 + playerLocationChunk.Y) && y < vertexLODDistance + playerLocationChunk.Y))
							{//set target Vertices to 8
								chunkCont->UpdateChunksTargetVertiecs(9);
							}
							else
							{								
								vertexLODDistance = vertexLODDistance + 2;
								if ((x > (vertexLODDistance * -1 + playerLocationChunk.X) && x < vertexLODDistance + playerLocationChunk.X)
									&& (y > (vertexLODDistance * -1 + playerLocationChunk.Y) && y < vertexLODDistance + playerLocationChunk.Y))
								{//set target Vertices to 7
									chunkCont->UpdateChunksTargetVertiecs(8);
								}
								else
								{
									vertexLODDistance = vertexLODDistance + 2;
									if ((x > (vertexLODDistance * -1 + playerLocationChunk.X) && x < vertexLODDistance + playerLocationChunk.X)
										&& (y > (vertexLODDistance * -1 + playerLocationChunk.Y) && y < vertexLODDistance + playerLocationChunk.Y))
									{//set target Vertices to 6
										chunkCont->UpdateChunksTargetVertiecs(7);
									}
									else
									{
										vertexLODDistance = vertexLODDistance + 2;
										if ((x > (vertexLODDistance * -1 + playerLocationChunk.X) && x < vertexLODDistance + playerLocationChunk.X)
											&& (y > (vertexLODDistance * -1 + playerLocationChunk.Y) && y < vertexLODDistance + playerLocationChunk.Y))
										{//set target Vertices to 5
											chunkCont->UpdateChunksTargetVertiecs(6);
										}
										else
										{
											vertexLODDistance = vertexLODDistance + 2;
											if ((x > (vertexLODDistance * -1 + playerLocationChunk.X) && x < vertexLODDistance + playerLocationChunk.X)
												&& (y > (vertexLODDistance * -1 + playerLocationChunk.Y) && y < vertexLODDistance + playerLocationChunk.Y))
											{//set target Vertices to 4
												chunkCont->UpdateChunksTargetVertiecs(5);
											}
											else
											{
												vertexLODDistance = vertexLODDistance + 2;
												if ((x > (vertexLODDistance * -1 + playerLocationChunk.X) && x < vertexLODDistance + playerLocationChunk.X)
													&& (y > (vertexLODDistance * -1 + playerLocationChunk.Y) && y < vertexLODDistance + playerLocationChunk.Y))
												{//set target Vertices to 3
													chunkCont->UpdateChunksTargetVertiecs(4);
												}
												else
												{//set target Vertices to 3
													chunkCont->UpdateChunksTargetVertiecs(4);
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}


void AMapController::SaveAllchunks()
{
	TArray<AChunkController*> chunks;
	chunkArray.GenerateValueArray(chunks);
	for (int i = 0; i < chunks.Num();i++)
	{
		chunks[i]->SaveChunks();
	}
}

AChunk* AMapController::FindChunk(FVector objLocation)
{
	FVector chunkPos = FVector::ZeroVector;
	if(objLocation.X < 0){ chunkPos.X = float(int((objLocation.X - (chunkSize)) / chunkSize)); }
	else { chunkPos.X = float(int(objLocation.X / chunkSize)); }
	if (objLocation.Y < 0) { chunkPos.Y = float(int((objLocation.Y - (chunkSize)) / chunkSize)); }
	else { chunkPos.Y = float(int(objLocation.Y / chunkSize)); }	
	chunkPos.Z = int(objLocation.Z / chunkSize);
	chunkPos.X *= chunkSize;
	chunkPos.Y *= chunkSize;
	chunkPos.Z *= chunkSize;
	return GetChunk(chunkPos);;
}

void AMapController::CheckChunk(FVector chunkPos)
{
	if (chunkArray.Contains(FVector2D(chunkPos.X, chunkPos.Y)))
	{
		AChunkController* chunkCon = chunkArray.FindRef(FVector2D(chunkPos.X, chunkPos.Y));
		if (chunkCon != nullptr)
		{
			chunkCon->CheckChunks(chunkPos);
		}
	}
}

AChunkController* AMapController::GetChunkController(FVector2D chunkPos)
{
	return chunkArray.FindRef(chunkPos);
}

void AMapController::AddMesh(TMap<FVector, float> newCells, float xSize, float ySize, float zSize, float cellSpacing, FVector Center, TArray<FVector> snapP,
	TArray<float> snapW, TArray<FVector> snapDir, int matIndex, FString name)
{
	GLog->Log("Adding mesh");
	//save mesh to file
	SnappingPointsData* newPoint = new SnappingPointsData(snapP, snapW,snapDir);
	SnappingPoints.Add(newPoint);
	//	//create an instance of our save game class
	USaveMeshData* SaveMeshDataInstance = Cast<USaveMeshData>(UGameplayStatics::CreateSaveGameObject(USaveMeshData::StaticClass()));
	CustomInstanceMesh cIM;		
	InstanceCustomMeshData tim = cIM.CreateMesh(newCells, xSize, ySize, zSize,Center, cellSpacing, meshCount);
	InstanceCustomMeshData* newInstanceMesh = new InstanceCustomMeshData(tim.GetPositions(), tim.GetInstancesType(),meshCount,matIndex);
	int id = instanceMeshData.Add(newInstanceMesh);
	

	GLog->Log("Added mesh data");
	//set the data for saving inside save game instance
	SaveMeshDataInstance->Locations = newInstanceMesh->GetPositions();
	SaveMeshDataInstance->InstanceType = newInstanceMesh->GetInstancesType();
	SaveMeshDataInstance->ICMID = meshCount;
	SaveMeshDataInstance->SnappingPoints = snapP;
	SaveMeshDataInstance->SnappingPointsWidth = snapW;
	SaveMeshDataInstance->SnappingPointsDirection = snapDir;
	SaveMeshDataInstance->MaterialIndex = matIndex;
	SaveMeshDataInstance->ObjectName = name;
	GLog->Log("Material index!!!!" + FString::FromInt(matIndex));
	
	////create save game name
	FString chunkName = gameName + "Mesh_" + FString::FromInt(meshCount);
	////save the the data 
	UGameplayStatics::SaveGameToSlot(SaveMeshDataInstance, chunkName, 0);
	if (name.Len() == 0) { ObjectNames.Add("Mesh_" + FString::FromInt(meshCount)); }
	else { ObjectNames.Add(name); }
	meshCount++;
}

int AMapController::GetInstancesType(int index)
{
	if (instanceMeshData.IsValidIndex(index))
	{
		GLog->Log("fat");
		return index;
	}
	GLog->Log("bom");
	return -1;
}

void AMapController::AddICM(int id, FVector location)
{
	if (instanceMeshData.IsValidIndex(id))
	{
		//GLog->Log("lalalal" + FString::FromInt(instancedMeshes[id]->GetInstanceCount()));
		instanceMeshData[id]->CreateICM(location, this);
		//GLog->Log(FString::FromInt(instancedMeshes[id]->GetInstanceCount()));
		//Add Arrows at snapping points
		ICM_AddArrows(location, id);
		currentInstanceMeshIndex = id;
	}
}

int AMapController::AddIM(int instanceType, FVector location)
{
	//this just adds and instance of instance type at the lcoation returning the index at which it was added
	FTransform spawnPos;
	spawnPos.SetLocation(location);
	spawnPos.SetScale3D(FVector(1, 1, 1));
	FQuat quat(FVector::UpVector, float(0));
	spawnPos.SetRotation(quat);//rotation will need changing to rotate all the instances related to that vaule
	int index = instancedMeshes[instanceType]->AddInstanceWorldSpace(spawnPos);
	currentMesh = instancedMeshes[instanceType];
	return index;
}

void AMapController::UpdateICM(FVector location)
{
	if(currentMesh != nullptr)
	{
		SnappingSystem* ss = new SnappingSystem;
		//check if snapping point is found
		GLog->Log("Updateing the mesh instance pos");
		FTransform spawnPos;
		FTransform meshtran;
		//currentMesh->GetInstanceTransform(currentInstanceMeshIndex, meshtran);
		GLog->Log(" mesh indexinh--- " + FString::FromInt(currentInstanceMeshIndex));
		toPlaceLocation = ss->FindSnappingPoint(location, instanceMeshData[currentInstanceMeshIndex]->GetCenterPos(), meshRotation, currentInstanceMeshIndex, this);
		spawnPos.SetLocation(toPlaceLocation);
		spawnPos.SetScale3D(FVector(1, 1, 1));
		FQuat quat(meshRotation);
		spawnPos.SetRotation(quat);
		//currentMesh->UpdateInstanceTransform(currentInstanceMeshIndex, spawnPos, true, true, true);
		//currentMesh->SetRelativeRotation(meshRotation);
		instanceMeshData[currentInstanceMeshIndex]->UpdateICM(spawnPos, this);
		ICM_UPdateArrows(spawnPos, currentInstanceMeshIndex);
	}
	
}

void AMapController::UpdateIm(int instanceType, int index, FTransform location)
{
	//rotation will need changing to rotate all the instances related to that vaule
	instancedMeshes[instanceType]->UpdateInstanceTransform(index, location, true, true, true);
	currentMesh = instancedMeshes[instanceType];
}

void AMapController::RemoveICM()
{
	if (instanceMeshData.IsValidIndex(currentInstanceMeshIndex))
	{
		instanceMeshData[currentInstanceMeshIndex]->RemoveICM(this);
		ICMarrows->ClearInstances();
	}
}

void AMapController::RemoveIm(int instanceType, int index)
{
	if (index != -1)
	{
		instancedMeshes[instanceType]->RemoveInstance(index);
	}
}

SnappingPointsData* AMapController::ICM_GetSnappingPointsRef(int index)
{
	if (SnappingPoints.IsValidIndex(index))
	{
		return SnappingPoints[index];
	}
	/*if (instancedMeshes.Contains(instanceType))
	{
		int im = instancedMeshes.Find(instanceType);*/
		/*float key = im + (index * 0.0000001);
		InstanceCustomMeshData* ICMD = ICMTable[im]->meshType.FindRef(key);
		if (ICMD != nullptr) { return ICMD->FindCenterPos(key); }
		if (SnappingPoints.IsValidIndex(im))
		{
			GLog->Log("Hello");
			return SnappingPoints[im];
		}*/
	//}
	return nullptr;
}

void AMapController::RotateICM_P(int direction)
{	
	if (currentMesh != nullptr)
	{
		GLog->Log("Rotatemesh!!");
		if (direction < 0)
		{
			//take
			meshRotation.Roll = 0;
			meshRotation -= FRotator(45, 0, 0);
			if (meshRotation.Pitch == -360 || meshRotation.Pitch == 360) { meshRotation.Pitch = 0; }
			FQuat quat = FQuat(meshRotation);
		}
		else
		{
			//add
			meshRotation.Roll = 0;
			meshRotation += FRotator(45, 0, 0);
			if (meshRotation.Pitch == -360 || meshRotation.Pitch == 360) { meshRotation.Pitch = 0; }
			//FQuat quat = FQuat(meshRotation);			
		}
		FTransform spawnPos;
		spawnPos.SetLocation(toPlaceLocation);
		spawnPos.SetScale3D(FVector(1, 1, 1));
		FQuat quat(meshRotation);
		spawnPos.SetRotation(quat);
		instanceMeshData[currentInstanceMeshIndex]->UpdateICM(spawnPos, this);
	}
}

void AMapController::RotateICM_R(int direction)
{
	if (currentMesh != nullptr)
	{
		if (direction < 0)
		{
			//take
			meshRotation.Pitch = 0;
			meshRotation -= FRotator(0, 45, 0);
			if (meshRotation.Yaw == -360 || meshRotation.Yaw == 360) { meshRotation.Yaw = 0; }
			FQuat quat = FQuat(meshRotation);


		}
		else
		{
			//add
			meshRotation.Pitch = 0;
			meshRotation += FRotator(0, 45, 0);
			if (meshRotation.Yaw == -360 || meshRotation.Yaw == 360) { meshRotation.Yaw = 0; }
			//FQuat quat = FQuat(meshRotation);			
		}
		FTransform spawnPos;
		spawnPos.SetLocation(toPlaceLocation);
		spawnPos.SetScale3D(FVector(1, 1, 1));
		FQuat quat(meshRotation);
		spawnPos.SetRotation(quat);
		instanceMeshData[currentInstanceMeshIndex]->UpdateICM(spawnPos, this);
	}
}

void AMapController::BuildICM(FVector lcoation)
{
	if (currentMesh != nullptr)
	{
		//find chunk
		//FVector toPlaceChunk = 
		AChunk* chunk = FindChunk(toPlaceLocation);
		if (chunk != nullptr)
		{
			GLog->Log("FoundChunk!!");
			FTransform spawnPos;
			spawnPos.SetLocation(toPlaceLocation);
			spawnPos.SetScale3D(FVector(1, 1, 1));
			FQuat quat(meshRotation);
			spawnPos.SetRotation(quat);
			chunk->AddICM(spawnPos, currentInstanceMeshIndex);
			RemoveICM();

		}
		else 
		{ 
			RemoveICM(); //remove ICM as chunk doesn't exist
		}
	}
}

void AMapController::RemoveInstanceMesh()
{
	currentMesh->RemoveInstance(currentInstanceMeshIndex);
}

bool AMapController::CheckMeshIndexIsFaild(int meshIndex)
{
	if (SnappingPoints.IsValidIndex(meshIndex))
	{
		//GLog->Log("Vaild Index");
		return true;
	}
	//GLog->Log("Failed");
	return false;
}

TArray<FVector> AMapController::GetSnappingPoints(int meshIndex)
{
	if (SnappingPoints.IsValidIndex(meshIndex))
	{
		//GLog->Log("Got Snapping");
		return SnappingPoints[meshIndex]->GetSnappingPoints();
	}
	//GLog->Log("Failed");
	return TArray<FVector>();
}

TArray<float> AMapController::GetSnappingPointsWidth(int meshIndex)
{
	if (SnappingPoints.IsValidIndex(meshIndex))
	{
		//GLog->Log("Got Snapping");
		return SnappingPoints[meshIndex]->GetSnappingPointsWidth();
	}
	//GLog->Log("Failed");
	return TArray<float>();
}

TArray<FVector> AMapController::GetSnappingDirection(int meshIndex)
{
	if (SnappingPoints.IsValidIndex(meshIndex))
	{
		//GLog->Log("Got Snapping");
		return SnappingPoints[meshIndex]->GetSnappingDirection();
	}
	//GLog->Log("Failed");
	return TArray<FVector>();
}

void AMapController::RemoveHitObject(FHitResult hitResult)
{
	GLog->Log("RemoveHitObject!!");
	AChunk* chunk = Cast<AChunk>(hitResult.Actor);
	if (chunk != nullptr)
	{
		GLog->Log("hitchunk!!");
		UInstancedStaticMeshComponent* instMesh = Cast<UInstancedStaticMeshComponent>(hitResult.GetComponent());
		if (instMesh != nullptr)
		{
			//remove the hit instance
			chunk->RemoveICM(instMesh, hitResult.Item);			
		}
	}
	else
	{
		AActor* actor = Cast<AActor>(hitResult.Actor);
		if (actor != nullptr)
		{
			GLog->Log("hitActor!!");
			FindChunk(actor->GetActorLocation())->RemovePlacedObject(actor);
		}
	}
}

int AMapController::GetObjectCount()
{
	int count = placeableStorageRef->GetPlaceableCount();
	count += meshCount;
	return count;
}

FString AMapController::GetObjectName(int index)
{
	if (ObjectNames.IsValidIndex(index))
	{
		return ObjectNames[index];
	}
	return FString();
}

bool AMapController::CheckObjectNameVaild(FString name)
{
	if (ObjectNames.Contains(name))
	{
		return false; 
	} 
	else 
	{
		return true; 
	} 
}


void AMapController::HasChunkUnderPlayerLoaded()
{
	//find which chunk the player is in
	
	FVector2D chunkPos = FVector2D::ZeroVector;
	if (loadedPlayerStart.X < 0) { chunkPos.X = float(int((loadedPlayerStart.X - (chunkSize)) / chunkSize)); }
	else { chunkPos.X = float(int(loadedPlayerStart.X / chunkSize)); }
	if (loadedPlayerStart.Y < 0) { chunkPos.Y = float(int((loadedPlayerStart.Y - (chunkSize)) / chunkSize)); }
	else { chunkPos.Y = float(int(loadedPlayerStart.Y / chunkSize)); }
	chunkPos.X *= chunkSize;
	chunkPos.Y *= chunkSize;
	AChunkController* chunkCont = GetChunkController(chunkPos);
	if (chunkCont != nullptr)
	{//check if it contains any chunks
		loadingGame = chunkCont->spawnedAllChunks;
	}
}

void AMapController::ICM_AddArrows(FVector spawnPos,int id)
{
	ICMarrows->ClearInstances();
	//loop through snappingPoints
	TArray<FVector> snapPoints = SnappingPoints[id]->GetSnappingPoints();
	TArray<float> snapWidth = SnappingPoints[id]->GetSnappingPointsWidth();
	TArray<FVector> snapDir = SnappingPoints[id]->GetSnappingDirection();
	//UKismetMathLibrary KMS = UKismetMathLibrary();
	for (int i = 0; i < snapPoints.Num();i++)
	{
		FTransform sp;
		const FRotator rot = meshRotation;
		FVector end = /*meshTran.GetLocation() + */snapPoints[i];
		FVector start = snapPoints[i];
		FVector dir = snapDir[i];//rot.RotateVector(snapDir[i]);
		if (dir.X < 0) { end.X -= snapWidth[i]; start.X += snapWidth[i]; }
		else if (dir.X > 0) { end.X += snapWidth[i]; start.X -= snapWidth[i]; }
		if (dir.Y < 0) { end.Y -= snapWidth[i]; start.Y += snapWidth[i]; }
		else if (dir.Y > 0) { end.Y += snapWidth[i]; start.Y -= snapWidth[i]; }
		if (dir.Z < 0) { end.Z -= snapWidth[i]; start.Z += snapWidth[i]; }
		else if (dir.Z > 0) { end.Z += snapWidth[i]; start.Z -= snapWidth[i]; }
		end = rot.RotateVector(end);
		end += spawnPos;
		start = rot.RotateVector(start);
		start += spawnPos;
		FVector test = snapPoints[i];
		test = rot.RotateVector(test);
		test += spawnPos;
		//sp.SetLocation(start);
		FVector spr = rot.RotateVector(snapPoints[i]);
		FRotator newRot = UKismetMathLibrary::FindLookAtRotation(FVector::ZeroVector, spr);

		if (snapPoints[i].X < 0) { sp.SetLocation(end); }
		else if (snapPoints[i].X > 0) { sp.SetLocation(start); }
		//if (snapPoints[i].Y < 0) { sp.SetLocation(end);}
		//else if (snapPoints[i].Y > 0) { sp.SetLocation(start);}
		//if (snapPoints[i].Z < 0) { sp.SetLocation(end);}
		//else if (snapPoints[i].Z > 0) { sp.SetLocation(start);}
		newRot = FRotator((newRot.Pitch + 90), newRot.Yaw + 0, (newRot.Roll + 0));//pyr
		FQuat newPos = FQuat(newRot);
		sp.SetRotation(newPos);
		ICMarrows->AddInstance(sp);
	}
}

void AMapController::ICM_UPdateArrows(FTransform spawnPos, int id)
{
	ICMarrows->ClearInstances();
	//loop through snappingPoints
	TArray<FVector> snapPoints = SnappingPoints[id]->GetSnappingPoints();
	TArray<float> snapWidth = SnappingPoints[id]->GetSnappingPointsWidth();
	TArray<FVector> snapDir = SnappingPoints[id]->GetSnappingDirection();
	//UKismetMathLibrary KMS = UKismetMathLibrary();
	for (int i = 0; i < snapPoints.Num();i++)
	{
		FTransform sp;
		const FRotator rot = meshRotation;
		FVector end = /*meshTran.GetLocation() + */snapPoints[i];
		FVector start = snapPoints[i];
		FVector dir = snapDir[i];//rot.RotateVector(snapDir[i]);
		if (dir.X < 0) { end.X -= snapWidth[i]; start.X += snapWidth[i]; }
		else if (dir.X > 0) { end.X += snapWidth[i]; start.X -= snapWidth[i]; }
		if (dir.Y < 0) { end.Y -= snapWidth[i]; start.Y += snapWidth[i]; }
		else if (dir.Y > 0) { end.Y += snapWidth[i]; start.Y -= snapWidth[i]; }
		if (dir.Z < 0) { end.Z -= snapWidth[i]; start.Z += snapWidth[i]; }
		else if (dir.Z > 0) { end.Z += snapWidth[i]; start.Z -= snapWidth[i]; }
		end = rot.RotateVector(end);
		end += spawnPos.GetLocation();
		start = rot.RotateVector(start);
		start += spawnPos.GetLocation();
		FVector test =  snapPoints[i];
		test = rot.RotateVector(test);
		test += spawnPos.GetLocation();
		//sp.SetLocation(start);
		FVector spr = rot.RotateVector(snapPoints[i]);
		FRotator newRot =  UKismetMathLibrary::FindLookAtRotation(FVector::ZeroVector, spr);
		
		if (snapPoints[i].X < 0) { sp.SetLocation(end); }
		else if (snapPoints[i].X > 0) { sp.SetLocation(start);}
		if (snapPoints[i].Y < 0) { sp.SetLocation(end);}
		else if (snapPoints[i].Y > 0) { sp.SetLocation(start);}
		if (snapPoints[i].Z < 0) { sp.SetLocation(end);}
		else if (snapPoints[i].Z > 0) { sp.SetLocation(start);}
		newRot = FRotator((newRot.Pitch+90), newRot.Yaw + 0,(newRot.Roll + 0));//pyr
		FQuat newPos = FQuat(newRot);
		sp.SetRotation(newPos);
		ICMarrows->AddInstance(sp);
	}
}


