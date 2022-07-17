// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunk.h"
#include "ChunkController.h"
#include "MapController.h"
#include "ChunkCreationThread.h"
#include "ChunkMeshCreationThread.h"
#include "ChunkWaterMeshCreationThreading.h"
#include "Lib/TriTable.h"
#include "Lib/ChunkColour.h"
#include "PartBuilder.h"
#include "Save/SaveChunkData.h"
#include "Placeables/PlaceableStorage.h"
#include "KismetProceduralMeshLibrary.h"
#include "MeshData/InstanceCustomMeshData.h"
#include "Meshes/CustomInstanceMesh.h"
#include "Meshes/ICMData.h"
#include "Meshes/ICMKeyStorage.h"


// Sets default values
AChunk::AChunk(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SurfaceHeight = 10;

	//
	cellDistance = 50.0f;
	cellAmount = 10;
	cellHeightAmount = 10;
	targetVertexCount = 10;
	targetCellDistance = cellDistance;
	chunkSize = (cellAmount - 1) * cellDistance;
	ThisScene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = ThisScene;
	ThisMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	ThisMesh->SetupAttachment(RootComponent);
	waterMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("WaterMesh"));
	waterMesh->SetupAttachment(RootComponent);

	//brushes
	brushCeneter = FVector::ZeroVector;
	brushSize = 50;
	mapCont = nullptr;
	//threading
	CreatedCells = false;
	CellsCreating = false;
	MeshCreating = false;
	CreatedWaterMesh = false;
	waterMeshCreating = false;
	CreatedMesh = false;
	MeshDrawn = false;
	waterMeshDrawn = false;
	hasChunkBeenEdited = false;
	LoadData = false;
	LoadedData = false;
	loadedChunk = false;
	hasChunkLoaded = false;
	meshLODUpdating = false;
	//Material intitialzer
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>Material1(TEXT("Material'/Game/Mat/TerrainMat.TerrainMat'"));
	terrainMat = Material1.Object;
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>Material5(TEXT("Material'/Game/Mat/Water.Water'"));
	waterMat = Material5.Object;
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>Material2(TEXT("Material'/Game/Mat/BasicStone.BasicStone'"));
	materials.Add(Material2.Object);
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>Material3(TEXT("Material'/Game/Mat/BasicStraw.BasicStraw'"));
	materials.Add(Material3.Object);
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>Material4(TEXT("Material'/Game/Mat/BasicWood.BasicWood'"));
	materials.Add(Material4.Object);
	placeableStorageRef = new PlaceableStorage();
	bSeed = 11123323;
}

// Called when the game starts or when spawned
void AChunk::BeginPlay()
{
	Super::BeginPlay();

	chunkX = GetActorLocation().X / chunkSize;
	chunkY = GetActorLocation().Y / chunkSize;
}

// Called every frame
void AChunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (loadedChunk == true)
	{
		if (hasChunkLoaded == false)
		{

			if (!CreatedCells)
			{
				if (!CellsCreating)
				{
					CellsCreating = true;
					CreateCells();
				}
			}
			if (CreatedCells)
			{
				if (!CreatedMesh)
				{
					if (LoadData == true)
					{
						LoadData = false;
						MeshCreating = true;
						MeshDrawn = true;
						CreatedMesh = true;
						//update cells
						LoadEditedCells();
						UpdateMesh();
						hasChunkLoaded = true;
					}
					if (!MeshCreating /*&& LoadedData == true*/)
					{
						MeshCreating = true;
						Colour.Empty();
						CreateMesh();
					}
				}
				if (CreatedMesh/* && CreatedWaterMesh*/)
				{
					if (!CreatedWaterMesh)
					{
						if (!waterMeshCreating)
						{
							waterMeshCreating = true;
							(new FAutoDeleteAsyncTask<ChunkWaterMeshCreationThread>(this, cellHeightAmount, cellAmount, cellDistance))->StartBackgroundTask();
						}
					}
					else
					{
						if (!MeshDrawn)
						{
							//CreateColour();
							MeshDrawn = true;
							TArray<FVector> Normals;
							TArray<FVector2D> UV;
							TArray<FColor> colour;
							TArray<FProcMeshTangent> Tan;
							UV = GenerateUVs();
							/*float dis = 1 / chunkSize;
							for (int i = 0; i < Vertices.Num();i++)
							{
								UV.Add(FVector2D(Vertices[i].X * dis, Vertices[i].Y * dis));

							}*/
							if (Vertices.Num() > 0)
							{
								if (chunkCont != nullptr)
								{
									chunkCont->AddChunkToKeep(chunkHeight, 1);
								}
							}
							//else{ chunkCont->AddChunkToKeep(chunkHeight,0); }
							UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UV, Normals, Tan);
							ThisMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UV, Colour, Tan, true);
							ThisMesh->CastShadow = false;
							ThisMesh->SetMaterial(0, terrainMat);
						}
						else
						{
							if (!waterMeshDrawn)
							{
								waterMeshDrawn = true;
								TArray<FVector> vert;
								TArray<int> tri;
								TArray<FVector> waterNormals;
								TArray<FVector2D> waterUV;
								TArray<FColor> watercolour;
								TArray<FProcMeshTangent> waterTan;
								float dis = 1 / chunkSize;
								for (int i = 0; i < waterVertices.Num();i++)
								{
									waterUV.Add(FVector2D(waterVertices[i].X * dis, waterVertices[i].Y * dis));

								}
								if (Vertices.Num() > 0)
								{
									if (chunkCont != nullptr)
									{
										chunkCont->AddChunkToKeep(chunkHeight, 1);
									}
								}
								UKismetProceduralMeshLibrary::CalculateTangentsForMesh(waterVertices, waterTriangles, waterUV, waterNormals, waterTan);
								waterMesh->CreateMeshSection(0, waterVertices, waterTriangles, waterNormals, waterUV, watercolour, waterTan, false);
								waterMesh->CastShadow = false;
								waterMesh->SetMaterial(0, waterMat);
								hasChunkLoaded = true;
								meshLODUpdating = false;
								safeToRemove = true;
							}
						}

					}
				}
			}
		}
	}	
}
bool AChunk::RemoveChunk()
{
	//if (safeToRemove == true)
	//{
		SaveChunk();
		//loop through part builders and remove them
		for (int i = 0; i < partBuilderRef.Num();i++)
		{
			if (partBuilderRef[i] != nullptr)
			{
				partBuilderRef[i]->Destroy();
			}		
		}
		partBuilderRef.Empty();
		placedObjects.Empty();
		return true;
	//}
	return false;
}
void AChunk::SaveChunk()
{
	if (hasChunkBeenEdited == true)	//only save if chunk has been edited
	{
		//create an instance of our save game class
		USaveChunkData* SaveGameDataInstance = Cast<USaveChunkData>(UGameplayStatics::CreateSaveGameObject(USaveChunkData::StaticClass()));
		//set the data for saving inside save game instance
		//TMultiMap <float, FTransform> rm = SavePlacedMeshes();
		TArray<int> fa;
		TArray<FTransform> ta;
		GetAllICMPositions(&ta, &fa);
		//rm.GenerateKeyArray(fa);
		//rm.GenerateValueArray(ta);
		SaveGameDataInstance->EditedCells = editedCells;
		SaveGameDataInstance->CellsInMesh = CellsInMesh;
		SaveGameDataInstance->PlacedMeshesType = fa;
		SaveGameDataInstance->PlacedMeshesPos = ta;
		//save placed objects
		SaveGameDataInstance->PlacedObjects = placedObjects;
		//GLog->Log(FString::FromInt(SaveGameDataInstance->PlacedMeshes.Num()));
		//create save game name
		FString chunkName = gameName + "_" + FString::FromInt(chunkX) + "_" + FString::FromInt(chunkY) + "_" + chunkLevel;
		//save the the data 
		UGameplayStatics::SaveGameToSlot(SaveGameDataInstance, chunkName, 0);
	}
}
void AChunk::LoadChunk()
{
	chunkX = GetActorLocation().X / chunkSize;
	chunkY = GetActorLocation().Y / chunkSize;
	//create an instance of our save game class
	USaveChunkData* SaveGameDataInstance = Cast<USaveChunkData>(UGameplayStatics::CreateSaveGameObject(USaveChunkData::StaticClass()));
	//create save game name
	FString chunkName = gameName + "_" + FString::FromInt(chunkX) + "_" + FString::FromInt(chunkY) + "_" + chunkLevel;
	//load the save game data into our variable
	SaveGameDataInstance = Cast<USaveChunkData>(UGameplayStatics::LoadGameFromSlot(chunkName, 0));
	if (SaveGameDataInstance != nullptr)
	{
		GLog->Log("Load chunk!!");
		LoadData = true;
		editedCells = SaveGameDataInstance->EditedCells;
		CellsInMesh = SaveGameDataInstance->CellsInMesh;
		GLog->Log("Cells In Mesh!!!" + FString::FromInt(CellsInMesh.Num()));
		LoadICM(SaveGameDataInstance->PlacedMeshesType, SaveGameDataInstance->PlacedMeshesPos);
		placeableStorageRef->SpawnLoadedPlaceableObjects(SaveGameDataInstance->PlacedObjects, this, GetWorld());	

		if (chunkCont != nullptr)
		{
			chunkCont->AddChunkToKeep(chunkHeight, 1);
		}
		loadedChunk = true;
	}
	else
	{
		//not laoding
		LoadedData = true;
		loadedChunk = true;
	}
}
void AChunk::SetChunkInstanceMeshes(TArray<UStaticMesh*> meshes)
{
	for (int i = 0; i < meshes.Num();i++)
	{
		for (int m = 0; m < materials.Num();m++)
		{
			UInstancedStaticMeshComponent* InstanceMesh = NewObject<UInstancedStaticMeshComponent>(this);
			InstanceMesh->RegisterComponent();
			InstanceMesh->SetStaticMesh(meshes[i]);
			InstanceMesh->SetMobility(EComponentMobility::Movable);
			InstanceMesh->SetMaterial(0, materials[m]);
			InstanceMesh->CastShadow = false;
			this->AddInstanceComponent(InstanceMesh);
			instancedMeshes.Add(InstanceMesh);
			ICMData* newICMData = new ICMData();/*NewObject<ICMData>(this)*/;
			ICMKeyStorage* newICMKey = new ICMKeyStorage();
			//ICMTable.Add(newICMData);
			ICMKeyTable.Add(newICMKey);
		}
	}
}

void AChunk::LoadICM(TArray<int> meshesType, TArray<FTransform> meshesPos)
{
	for (int i = 0; i < meshesType.Num();i++)
	{
		AddICM(meshesPos[i], meshesType[i]);
	}
}
bool AChunk::ChunkLoaded()
{
	return hasChunkLoaded;
}
int AChunk::KeepChunkLoaded()
{
	int rt = 0;
	if (hasChunkBeenEdited == true) { rt = 1; }
	if (Vertices.Num() > 0) { rt = 1; }
	if (waterVertices.Num() > 0) { rt = 1; }
	if (meshLODUpdating == true) { rt = 1; }
	return rt;
}
//public functions
void AChunk::CreateCells()
{	
	(new FAutoDeleteAsyncTask<ChunkCreationThreading>(this,cellHeightAmount,cellAmount, cellDistance,SurfaceHeight,bSeed))->StartBackgroundTask();
}
void AChunk::CreateMesh()
{
	(new FAutoDeleteAsyncTask<ChunkMeshCreationThreading>(this, cellHeightAmount, cellAmount, cellDistance))->StartBackgroundTask();
	//(new FAutoDeleteAsyncTask<ChunkWaterMeshCreationThread>(this, cellHeightAmount, cellAmount, cellDistance))->StartBackgroundTask();
}

void AChunk::UpdateMesh()
{
	Vertices.Empty();
	Triangles.Empty();	
	Colour.Empty();
	//VerticesPlusCellPos.Empty();
	//GLog->Log("Cells In Mesh!!!" + FString::FromInt(CellsInMesh.Num()));
	////create the mesh
	TArray<FVector> oldCellsInMesh = CellsInMesh;

	CellsInMesh.Empty();
	for (int i = 0; i < oldCellsInMesh.Num(); i++)
	{
		//update all the cells which are part of the existing mesh or which have been updated
		TriTable Tri;
		Tri.Triangle(0,oldCellsInMesh[i], this, FVector(cellDistance, cellDistance, cellDistance), cellAmount, cellHeightAmount);
	}
	//GLog->Log("Cells In Mesh!!!" + FString::FromInt(CellsInMesh.Num()));
	//CreateColour();
	ThisMesh->ClearMeshSection(0);
	TArray<FVector> Normals;
	TArray<FVector2D> UV;
	UV = GenerateUVs();
	/*float dis = 1 / chunkSize;
	for (int i = 0; i < Vertices.Num();i++)
	{
		UV.Add(FVector2D(Vertices[i].X * dis, Vertices[i].Y * dis));
	}*/
	TArray<FProcMeshTangent> Tan;
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UV, Normals, Tan);
	ThisMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UV, Colour, Tan, true);
	ThisMesh->CastShadow = false;
	ThisMesh->SetMaterial(0, terrainMat);
}

void AChunk::LoadMesh()
{
	Vertices.Empty();
	Triangles.Empty();
	//VerticesPlusCellPos.Empty();
	Colour.Empty();

	////create the mesh
	TArray<FVector> oldCellsInMesh;
	editedCells.GenerateKeyArray(oldCellsInMesh);
	GLog->Log(FString::FromInt(oldCellsInMesh.Num()));
	for (int i = 0; i < CellsInMesh.Num(); i++)
	{
		if (oldCellsInMesh.Contains(CellsInMesh[i]) != true)
		{
			oldCellsInMesh.Add(CellsInMesh[i]);
		}
	}
	GLog->Log(FString::FromInt(oldCellsInMesh.Num()));
	CellsInMesh.Empty();
	for (int i = 0; i < oldCellsInMesh.Num(); i++)
	{
		//update all the cells which are part of the existing mesh or which have been updated
		TriTable Tri;
		Tri.Triangle(0,oldCellsInMesh[i], this, FVector(cellDistance,cellDistance,cellDistance), cellAmount, cellHeightAmount);	//NOTE cell distance will be deault vaule as chunk will be at full vertex amount
	}
	//CreateColour();
	ThisMesh->ClearMeshSection(0);
	TArray<FVector> Normals;
	TArray<FVector2D> UV;
	TArray<FProcMeshTangent> Tan;
	ThisMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UV, Colour, Tan, true);
	ThisMesh->CastShadow = false;
	//ThisMesh->SetMaterial(0, terrainMat);
}

//void AChunk::CreateColour()
//{
//	ChunkColour CC;
//	Colour.Empty();
//	ColourForVertex.Empty();
////	Colour = CC.CreateChunkColour(this, cellDistance, cellAmount);
//}

void AChunk::UpdateMeshLOD()
{
	Vertices.Empty();
	Triangles.Empty();
	Colour.Empty();
	//VerticesPlusCellPos.Empty();
	waterVertices.Empty();
	waterTriangles.Empty();
	hasChunkLoaded = false;
	CreatedMesh = false;
	MeshCreating = false;
	MeshDrawn = false;
	CreatedWaterMesh = false;
	waterMeshCreating = false;
	waterMeshDrawn = false;	
}

void AChunk::AddCell(FVector pos, float density)
{
	Cells.Add(pos, density);
}

void AChunk::AddWaterCell(FVector pos, float density)
{
	waterCells.Add(pos, density);
}

FVector4 AChunk::CellData(int meshType, float x, float y, float z)
{//x,y,z would fit into a cell so find the cloestes cell	
	FVector4 rv;
	rv = FVector(x, y, z);
	if (meshType == 0)
	{						
			rv.W = Cells.FindRef(FVector(x, y, z));
	}
	else if (meshType == 1)
	{
		rv.W = waterCells.FindRef(FVector(x, y, z));
	}
	return rv;
}

int AChunk::AddVertex(int meshType, FVector vertex, FVector cellPos)
{
	if (meshType == 0)
	{
		//if (Vertices.Contains(vertex))
		//{
		//	return Vertices.Find(vertex);
		//}
		if (CellsColour.Contains(cellPos))
		{
			Colour.Add(CellsColour.FindRef(cellPos));	//this finds the colour for this cell adding it to the colour array
		}
		else { Colour.Add(FColor::White); }
		return Vertices.Add(vertex);
	}
	else
	{
		if (waterVertices.Contains(vertex))
		{
			return waterVertices.Find(vertex);
		}
		//if (CellsColour.Contains(cellPos))
		//{
		//	Colour.Add(CellsColour.FindRef(cellPos));	//this finds the colour for this cell adding it to the colour array
		//}
		//else { Colour.Add(FColor::White); }
		return waterVertices.Add(vertex);
	}
}

//void AChunk::AddVertexCellPos(int meshType, FVector vertex, FVector cellPos)
//{
//	if (meshType == 0)
//	{
//		if (!VerticesPlusCellPos.Contains(vertex))
//		{
//			VerticesPlusCellPos.Add(vertex, cellPos);
//		}
//	}	
//}

void AChunk::AddTriangle(int meshType, int index)
{
	if (meshType == 0)
	{
		Triangles.Add(index);
	}
	else 
	{
		waterTriangles.Add(index);
	}
}

void AChunk::AddCellInMesh(int meshType, FVector cellPos)
{
	if (meshType == 0)
	{
		if (!CellsInMesh.Contains(cellPos)) { CellsInMesh.Add(cellPos); }
	}
	else if (meshType == 1)
	{
		if (!CellsInWaterMesh.Contains(cellPos)) { CellsInWaterMesh.Add(cellPos); }
	}
}

void AChunk::UpdateCell(FVector cellPos, float newCellVaule)
{
	if (newCellVaule > 1.5) { newCellVaule = 1.5; }
	if (Cells.Contains(cellPos))
	{
		//GLog->Log("Emplacing cell!!!");
		Cells.Emplace(cellPos, newCellVaule);
		if (!editedCells.Contains(cellPos)) { editedCells.Add(cellPos,Cells.FindRef(cellPos)); }
		if (!CellsInMesh.Contains(cellPos) && WithinChunkBounds(cellPos)) { CellsInMesh.Add(cellPos); }
	}
}

bool AChunk::DrawWaterCell(int meshType, FVector cellPos)
{
	if (meshType == 0) { return true; }
	if (meshType == 1)
	{		
		if ((cellPos.Z + GetActorLocation().Z) == 550) { return true; }
		float waterCellVaule = waterCells.FindRef(cellPos);
		float terrainMeshVaule = Cells.FindRef(cellPos);
		if (terrainMeshVaule >= 1)//cell is in terrain mesh and the cell is water
		{//this cell has water and terrain in it
			return false;			
		}
		FVector newCellPos = FVector(cellPos.X, cellPos.Y, cellPos.Z - cellDistance);
		
		if(newCellPos.Z < 0)
		{//find other chunk cellData
			FVector otherChunkPos = GetActorLocation();
			otherChunkPos.Z -= chunkSize;		
			return chunkCont->CheckOtherChunkTerrain(newCellPos, otherChunkPos);
		}
		else
		{
			terrainMeshVaule = Cells.FindRef(newCellPos);
			if (terrainMeshVaule >= 1)//cell is in terrain mesh and the cell is water
			{//this cell has water and terrain in it
				return false;
			}
			else
			{
				newCellPos = FVector(cellPos.X, cellPos.Y, cellPos.Z - (cellDistance * 2));
				

//				FVector newCellPos = FVector(cellPos.X, cellPos.Y, cellPos.Z - cellDistance);

				if (newCellPos.Z < 0)
				{//find other chunk cellData
					FVector otherChunkPos = GetActorLocation();
					otherChunkPos.Z -= chunkSize;
					return chunkCont->CheckOtherChunkTerrain(newCellPos, otherChunkPos);
				}
				else 
				{
					terrainMeshVaule = Cells.FindRef(FVector(cellPos.X, cellPos.Y, cellPos.Z - (cellDistance * 2)));
					if (terrainMeshVaule >= 1)//cell is in terrain mesh and the cell is water
					{//this cell has water and terrain in it
						return false;
					}
				}
				
			}
		}	
	}
	return true;
}

bool AChunk::OtherCellTerrain(FVector cellPos)
{//THIS FUNCTION SHOULD OLNY BE USED FOR CHECKING CHUNK BELOW FOR WATER MESH
	cellPos.Z = cellPos.Z + chunkSize;
	float terrainMeshVaule = Cells.FindRef(cellPos);
	if (terrainMeshVaule >= 1)//cell is in terrain mesh and the cell is water
	{//this cell has water and terrain in it
		return false;
	}
	return true;
}

void AChunk::SetCellColour(FVector cellPos, FColor cellColour)
{//this adds the colour cell to the cellcolour map
	CellsColour.Add(cellPos, cellColour);
}

void AChunk::SetTargetVertices(int newTarget)
{
	safeToRemove = false;
	targetVertexCount = newTarget;
	//update cell distance
	targetCellDistance = chunkSize / (targetVertexCount - 1);
	meshLODUpdating = true;
	UpdateMeshLOD();
}

//int AChunk::GetXAmountOfVertices(int x, float* newcellDistance)
//{	
//	newcellDistance = &targetCellDistance;
//	return targetVertexCount - 1;
//}

//int AChunk::GetYAmountOfVertices(int y, float* newcellDistance)
//{
//	//float t = chunkSize / (targetVertexCount - 1);
//	newcellDistance = &targetCellDistance;
//	return targetVertexCount - 1;
//
//	//if (y == 0)
//	//{//get vertex count of chunk at -1
//	//	FVector chunkPos = GetActorLocation();
//	//	AChunkController* otherChunkCont = mapCont->GetChunkController(FVector2D(chunkPos.X, chunkPos.Y - chunkSize));
//	//	if (otherChunkCont != nullptr)
//	//	{//vaild chunk so get target vertex count
//	//		int otherChunkTarget = otherChunkCont->GetChunkTargetVertices();
//	//		float newCellDistance = chunkSize / (otherChunkTarget-1);
//	//		newcellDistance = &newCellDistance;
//	//		return otherChunkTarget-1;
//	//	}
//	//	else { return targetVertexCount-1; }	//not vaild so return vertex count
//	//}
//	//else if (y >= targetVertexCount)
//	//{//get vertex count of chunk at +1
//	//	FVector chunkPos = GetActorLocation();
//	//	AChunkController* otherChunkCont = mapCont->GetChunkController(FVector2D(chunkPos.X, chunkPos.Y + chunkSize));
//	//	if (otherChunkCont != nullptr)
//	//	{//vaild chunk so get target vertex count
//	//		int otherChunkTarget = otherChunkCont->GetChunkTargetVertices();
//	//		float newCellDistance = chunkSize / (otherChunkTarget-1);
//	//		newcellDistance = &newCellDistance;
//	//		return otherChunkTarget-1;
//	//	}
//	//	else { return targetVertexCount-1; }	//not vaild so return vertex count
//	//}
//	//else
//	//{//return target count
//	//	newcellDistance = &targetCellDistance;
//	//	return targetVertexCount-1;
//	//}
//}

//float AChunk::GetXCellDistance(float y, float* newcellDistance)
//{
//	newcellDistance = &targetCellDistance;
//	return targetCellDistance;
//	if (y == 0)
//	{	//get vertex count at x - 1
//		FVector chunkPos = GetActorLocation();
//		AChunkController* otherChunkCont = mapCont->GetChunkController(FVector2D(chunkPos.X - chunkSize, chunkPos.Y));
//		if (otherChunkCont != nullptr)
//		{//vaild chunk so get target vertex count
//			int otherChunkTarget = otherChunkCont->GetChunkTargetVertices();
//			float newCellDistance = chunkSize / (otherChunkTarget-1);
//			newcellDistance = &newCellDistance;
//			return newCellDistance;
//		}
//		newcellDistance = &targetCellDistance;
//		return targetCellDistance; 	//not vaild so return vertex count
//	}
//	else if (y == chunkSize)
//	{	//get vertex count at x + 1 
//		FVector chunkPos = GetActorLocation();
//		AChunkController* otherChunkCont = mapCont->GetChunkController(FVector2D(chunkPos.X + chunkSize, chunkPos.Y));
//		if (otherChunkCont != nullptr)
//		{//vaild chunk so get target vertex count
//			int otherChunkTarget = otherChunkCont->GetChunkTargetVertices();
//			float newCellDistance = chunkSize / (otherChunkTarget - 1);
//			newcellDistance = &newCellDistance;
//			return newCellDistance;
//		}
//		newcellDistance = &targetCellDistance;
//		return targetCellDistance;	//not vaild so return vertex count
//	}
//	newcellDistance = &targetCellDistance;
//	return targetCellDistance;	
//}

//float AChunk::GetYCellDistance(float x, float* newcellDistance)
//{
//	newcellDistance = &targetCellDistance;
//	return targetCellDistance;
//	if (x == 0)
//	{	//get vertex count at y - 1
//		FVector chunkPos = GetActorLocation();
//		AChunkController* otherChunkCont = mapCont->GetChunkController(FVector2D(chunkPos.X, chunkPos.Y - chunkSize));
//		if (otherChunkCont != nullptr)
//		{//vaild chunk so get target vertex count
//			int otherChunkTarget = otherChunkCont->GetChunkTargetVertices();
//			float newCellDistance = chunkSize / (otherChunkTarget - 1);
//			newcellDistance = &newCellDistance;
//			return newCellDistance;
//		}
//		newcellDistance = &targetCellDistance;
//		return targetCellDistance; 	//not vaild so return vertex count
//	}
//	else if (x == chunkSize)
//	{	//get vertex count at y + 1 
//		FVector chunkPos = GetActorLocation();
//		AChunkController* otherChunkCont = mapCont->GetChunkController(FVector2D(chunkPos.X, chunkPos.Y + chunkSize));
//		if (otherChunkCont != nullptr)
//		{//vaild chunk so get target vertex count
//			int otherChunkTarget = otherChunkCont->GetChunkTargetVertices();
//			float newCellDistance = chunkSize / (otherChunkTarget - 1);
//			newcellDistance = &newCellDistance;
//			return newCellDistance;
//		}
//		newcellDistance = &targetCellDistance;
//		return targetCellDistance;	//not vaild so return vertex count
//	}
//	newcellDistance = &targetCellDistance;
//	return targetCellDistance;
//}

//void AChunk::GetTargetCellDisatnce(float* newCellDistance)
//{
//	float newcellDistance = chunkSize / (targetVertexCount - 1);
//	newCellDistance = &newcellDistance;
//}

//void AChunk::AddCellColour(FVector cellPos, FVector vertex)
//{//this works out what cell the vertex should use for the colour
//	if (CellsColour.Contains(cellPos))
//	{
//		Colour.Add(CellsColour.FindRef(cellPos));	//this finds the colour for this cell adding it to the colour array
//	}
//	else{ Colour.Add(FColor::White); }	//adds basic non colour
//}

//TMap<FVector, FVector> AChunk::GetCellAndVertexData()
//{
//	return VerticesPlusCellPos;
//}

void AChunk::LoadEditedCells()
{
	//GLog->Log("Load Cells");
	TArray<FVector> keys;
	TArray<float> value;
	editedCells.GenerateKeyArray(keys);
	editedCells.GenerateValueArray(value);
	for (int i = 0; i < editedCells.Num();i++)
	{
		if (Cells.Contains(keys[i]) == true)
		{
			//GLog->Log("Update Cells");
			/*int index = Cells.Find*/
			Cells.Remove(keys[i]);
			Cells.Add(keys[i], value[i]);			
		}		
	}
}

void AChunk::AddBrush(FVector location, float strength, FVector playerLocation, bool centerSelf)
{
	hasChunkBeenEdited = true;
	//GLog->Log("AddBrush");
	TArray<FVector> inBrushVertices = GetVerticesWithinBrush(location,playerLocation,centerSelf,strength);
	UpdateCells(inBrushVertices, strength, playerLocation,centerSelf, location);

}
void AChunk::TakeBrush(FVector location, float strength, FVector playerLocation, bool centerSelf)
{
	hasChunkBeenEdited = true;
	//GLog->Log("takeBrush");	
	TArray<FVector> inBrushVertices = GetVerticesWithinBrush(location, playerLocation, centerSelf, strength);
	UpdateCells(inBrushVertices, strength, playerLocation,centerSelf, location);
}
void AChunk::AddPartBuilder(APartBuilder* partBulder)
{
	hasChunkBeenEdited = true;
	partBuilderRef.Add(partBulder);
	placedObjects.Add(partBulder->GetActorLocation(), 0);
	partBulder->SetMapController(mapCont);
}
void AChunk::RemovePlacedObject(AActor* object)
{
	APartBuilder* partBuilder = Cast<APartBuilder>(object);
	if (partBuilder != nullptr)
	{
		partBuilder->Destroy();
		//remove object
		partBuilderRef.Remove(partBuilder);
		placedObjects.Remove(partBuilder->GetActorLocation());
	}

}

int AChunk::AddIM(int instanceType, FTransform pos, InstanceCustomMeshData* ICM, int blockId)
{
	//this just adds and instance of instance type at the lcoation returning the index at which it was added
	int index = instancedMeshes[instanceType]->AddInstanceWorldSpace(pos);
	//float key = instanceType + (index * 0.00001);
	//if (ICMTable.IsValidIndex(instanceType))
	//{
	//	ICMTable[instanceType]->meshType.Add(key, ICM);	//this adds the key and icm to the table		
	//}
	if (ICMKeyTable.IsValidIndex(instanceType))
	{
		ICMKeyTable[instanceType]->ICMKeyStorageAdd(index,index,blockId,ICM);	//adds new element to keyTable
	}
	//need to add the key to a map
	return index;
}
void AChunk::AddICM(FTransform transform, int ICMID)
{
	LoadMoreChunks_ICM();
	hasChunkBeenEdited = true;
	int IMDi = ICMD_ContainID(ICMID);
	if (IMDi >= 0)
	{
		//ICM type has already been added to the array
		instanceMeshData[IMDi]->CreateICM(transform, this);
	}
	else
	{
		//ICM type needs to be created for this chunk
		//(___static meshes should already be in this chunk___)
		//Create new ICM type for this chunk and get the data for it from map
		InstanceCustomMeshData* newICM = new InstanceCustomMeshData(mapCont->GetICM_Positions(ICMID),mapCont->GetICM_InstanceTypes(ICMID),mapCont->GetICM_ID(ICMID),mapCont->GetICM_MatIndex(ICMID));
		newICM->CreateICM(transform, this);
		instanceMeshData.Add(newICM);
		//ICMKeyStorage* newICMKey = new ICMKeyStorage();
		//ICMKeyTable.Add(newICMKey);
	}	
}

void AChunk::RemoveICM(UInstancedStaticMeshComponent* instanceType, int index)
{
	GLog->Log("Remove ICM");
	GLog->Log("Index" + FString::FromInt(index));
	if (instancedMeshes.Contains(instanceType))
	{
		int imType = instancedMeshes.Find(instanceType);
		if (ICMKeyTable.IsValidIndex(imType))
		{

			int blockID = ICMKeyTable[imType]->GetBlockID(index);
			GLog->Log("Block ID " + FString::SanitizeFloat(blockID));
			//int removedAmount = 0;	//stores the amount of vertices which where removed
			InstanceCustomMeshData* ICMD = ICMKeyTable[imType]->GetICMData(index);
			if (ICMD != nullptr) 
			{ 
				GLog->Log("Contain!!");
				ICMD->RemoveICM(this,blockID); 
			}
			else
			{
				GLog->Log("IM not vaild!!!");
			}
		}	
	}
}
void AChunk::RemoveIM(int instanceType, int index)
{
	if(instancedMeshes.IsValidIndex(instanceType))
	{ 
		instancedMeshes[instanceType]->RemoveInstance(index); 
		ICMKeyTable[instanceType]->Remove(index);
	}
}
int AChunk::ICMD_ContainID(int id)
{
	for (int i = 0;i < instanceMeshData.Num();i++)//loop through array
	{
		if (instanceMeshData[i]->GetID() == id)//check each ID
		{
			return i;
		}
	}	
	return -1;
}
void AChunk::GetAllICMPositions(TArray<FTransform>* outPositions, TArray<int>* outTypes)
{
	for (int i = 0; i < instanceMeshData.Num();i++)
	{
		TArray<FTransform> positions = instanceMeshData[i]->GetCenterPositions();
		int types = instanceMeshData[i]->GetID();
		for (int p = 0;p < positions.Num();p++)
		{
			outPositions->Add(positions[p]);
			outTypes->Add(types);
		}				
	}
}
FTransform AChunk::ICM_GetCenterPos(UInstancedStaticMeshComponent* instanceType, int index)
{
	GLog->Log("GetCenter");
	if (instancedMeshes.Contains(instanceType))
	{
		int im = instancedMeshes.Find(instanceType);
		//float key = im + (index * 0.00001);
		//if (ICMTable.IsValidIndex(im))
		if (ICMKeyTable.IsValidIndex(im))
		{
			//InstanceCustomMeshData* ICMD = ICMTable[im]->meshType.FindRef(key);
			InstanceCustomMeshData* ICMD = ICMKeyTable[im]->GetICMData(index);
			if (ICMD != nullptr) { return ICMD->FindCenterPos(ICMKeyTable[im]->GetBlockID(index)); }
		}
	}
	return FTransform();
}
int AChunk::ICM_GetID(UInstancedStaticMeshComponent* instanceType, int index)
{
	if (instancedMeshes.Contains(instanceType))
	{
		int im = instancedMeshes.Find(instanceType);
		//float key = im + (index * 0.00001);
		//if (ICMTable.IsValidIndex(im))
		if (ICMKeyTable.IsValidIndex(im))
		{
			InstanceCustomMeshData* ICMD = ICMKeyTable[im]->GetICMData(index);
			if (ICMD != nullptr) { return ICMD->GetID(); }
		}
	}
	return -1;
}


//privtae fucntions

TArray<FVector2D> AChunk::GenerateUVs()
{
	TArray<FVector2D> Uvs;
	float dis = 1 / (chunkSize * 2);
	for (int i = 0; i < Triangles.Num();)
	{
		//stage one get the vertices for this triangle
		FVector p1 = Vertices[Triangles[i]];
		FVector p2 = Vertices[Triangles[i+1]];
		FVector p3 = Vertices[Triangles[i+2]];
		int uvCheck = CheckFlipUV(p1, p2, p3);
		float halfCell = chunkSize;
		//Uvs.Add(FVector2D(p1.X * dis, p1.Z * dis));
		//Uvs.Add(FVector2D(p2.X * dis, p2.Z * dis));
		//Uvs.Add(FVector2D(p3.X * dis, p3.Z * dis));

		//Uvs.Add(FVector2D(p1.Z * dis, p1.Y * dis));
		//Uvs.Add(FVector2D(p2.Z * dis, p2.Y * dis));
		//Uvs.Add(FVector2D(p3.Z * dis, p3.Y * dis));
		FVector2D startPoint;
		switch (uvCheck)
		{
		case 0:
			startPoint = FVector2D(dis, dis);
			Uvs.Add(FVector2D(p1.X * dis, p1.Y * dis));
			Uvs.Add(FVector2D(p2.X * dis, p2.Y * dis));
			Uvs.Add(FVector2D(p3.X * dis, p3.Y * dis));
			break;
		case 1:
			startPoint = FVector2D(dis, 0);
			Uvs.Add(FVector2D((p1.Z + chunkSize) * dis, (p1.Y + chunkSize) * dis));
			Uvs.Add(FVector2D((p2.Z + chunkSize) * dis, (p2.Y + chunkSize) * dis));
			Uvs.Add(FVector2D((p3.Z + chunkSize) * dis, (p3.Y + chunkSize) * dis));
			break;
		case 2:
			startPoint = FVector2D(0, dis);
			Uvs.Add(FVector2D((p1.X + chunkSize) * dis, p1.Z * dis));
			Uvs.Add(FVector2D((p2.X + chunkSize) * dis, p2.Z * dis));
			Uvs.Add(FVector2D((p3.X + chunkSize) * dis, p3.Z * dis));
			break;
		}

		//if ((p1.X + halfCell) < (p2.X + halfCell) && (p1.X - halfCell) > (p2.X - halfCell))
		//{		
		//	if ((p1.X + halfCell) < (p3.X + halfCell) && (p3.X - halfCell) > (p3.X - halfCell))
		//	{
		//		Uvs.Add(FVector2D(p1.X * dis, p1.Z * dis));
		//		Uvs.Add(FVector2D(p2.X * dis, p2.Z * dis));
		//		Uvs.Add(FVector2D(p3.X * dis, p3.Z * dis));
		//	}
		//	else
		//	{
		//		Uvs.Add(FVector2D(p1.X * dis, p1.Y * dis));
		//		Uvs.Add(FVector2D(p2.X * dis, p2.Y * dis));
		//		Uvs.Add(FVector2D(p3.X * dis, p3.Y * dis));
		//	}
		//}
		//else if ((p1.Y + halfCell) < (p2.Y + halfCell) && (p1.Y - halfCell) > (p2.Y - halfCell))
		//{
		//	if((p1.Y + halfCell) < (p3.Y + halfCell) && (p3.Y - halfCell) > (p3.Y - halfCell))
		//	{
		//		Uvs.Add(FVector2D(p1.Z * dis, p1.Y * dis));
		//		Uvs.Add(FVector2D(p2.Z * dis, p2.Y * dis));
		//		Uvs.Add(FVector2D(p3.Z * dis, p3.Y * dis));
		//	}
		//	else
		//	{
		//		Uvs.Add(FVector2D(p1.X * dis, p1.Y * dis));
		//		Uvs.Add(FVector2D(p2.X * dis, p2.Y * dis));
		//		Uvs.Add(FVector2D(p3.X * dis, p3.Y * dis));
		//	}
		//}
		//else
		//{
		//	Uvs.Add(FVector2D(p1.X * dis, p1.Y * dis));
		//	Uvs.Add(FVector2D(p2.X * dis, p2.Y * dis));
		//	Uvs.Add(FVector2D(p3.X * dis, p3.Y * dis));
		//}
		
		/*if (uvCheck != 0)
		{
			Uvs.Add(FVector2D(p1.X * dis, p1.Y * dis));
			Uvs.Add(FVector2D(p2.X * dis, p2.Y * dis));
			Uvs.Add(FVector2D(p3.X * dis, p3.Y * dis));
		}
		else
		{
			Uvs.Add(FVector2D(p1.X * dis, p1.Y * dis));
			Uvs.Add(FVector2D(p2.X * dis, p2.Y * dis));
			Uvs.Add(FVector2D(p3.X * dis, p3.Y * dis));
		}*/
		i += 3;	//update i
	}
	
	return Uvs;
}

int AChunk::CheckFlipUV(FVector p1, FVector p2, FVector p3)
{
	//work out the perimter of the trianlges
	float m1, m2, m3;
	m1 = FindTrianglePerimter(FVector2D(p1.X, p1.Y), FVector2D(p2.X, p2.Y), FVector2D(p3.X, p3.Y));
	m2 = FindTrianglePerimter(FVector2D(p1.Z, p1.Y), FVector2D(p2.Z, p2.Y), FVector2D(p3.Z, p3.Y));
	m3 = FindTrianglePerimter(FVector2D(p1.X, p1.Z), FVector2D(p2.X, p2.Z), FVector2D(p3.X, p3.Z));

	//find out which methode has the biggest perimter
	if (m1 > m2)
	{//m1 bigger than m2
		if (m1 > m3)
		{//m1 bigger than m3
			return 0;
		}
		else//m3 bigger than m1
		{
			return 2;
		}
	}
	else
	{//m2 bigger than m1
		if (m2 > m3)
		{//m2 bigger than m3
			return 1;
		}
		else//m3 bigger than m2
		{
			return 2;
		}
	}

	//float halfCell = 2.5f/*(cellDistance / 4)*/ /**2*/;

	//if ((p1.X) < (p2.X + halfCell) && (p1.X) > (p2.X - halfCell)
	//	&& (p3.X) < (p2.X + halfCell) && (p3.X) > (p2.X - halfCell))
	//{//all the x's are the sameISh
	//	if ((p1.Y) < (p2.Y + halfCell) && (p1.Y) > (p2.Y - halfCell)
	//		|| (p3.Y) < (p2.Y + halfCell) && (p3.Y) > (p2.Y - halfCell))
	//	{//all the Y's are the sameISh
	//		return 1;
	//	}		
	//}
	//if ((p1.Y) < (p2.Y + halfCell) && (p1.Y) > (p2.Y - halfCell)
	//	&& (p3.Y) < (p2.Y + halfCell) && (p3.Y) > (p2.Y - halfCell))
	//{//all the Y's are the sameISh
	//	if ((p1.X) < (p2.X + halfCell) && (p1.X) > (p2.X - halfCell)
	//		|| (p3.X) < (p2.X + halfCell) && (p3.X) > (p2.X - halfCell))
	//	{//all the x's are the sameISh
	//		return 2;
	//	}
	//}
	//return 0;

	//if (((p1.X + halfCell) < (p2.X + halfCell) && (p1.X - halfCell) > (p2.X - halfCell))
	//	&& ((p1.Y + halfCell) < (p2.Y + halfCell) && (p1.Y - halfCell) > (p2.Y - halfCell)))
	//{
	//	return 1;
	//}
	//if (((p2.X + halfCell) < (p3.X + halfCell) && (p2.X - halfCell) > (p3.X - halfCell))
	//	&& ((p2.Y + halfCell) < (p3.Y + halfCell) && (p2.Y - halfCell) > (p3.Y - halfCell)))
	//{
	//	return 2;
	//}
	//return 0;
}

float AChunk::FindTrianglePerimter(FVector2D p1, FVector2D p2, FVector2D p3)
{
	FVector2D s1, s2, s3;
	s1 = p1 - p2;
	s2 = p2 - p3;
	s3 = p3 - p1;
	float s1S, s2S, s3S;
	s1S = s1.Size();
	s2S = s2.Size();
	s3S = s3.Size();
	if (s1S < 0) { s1S *= -1; }
	if (s2S < 0) { s2S *= -1; }
	if (s3S < 0) { s3S *= -1; }

	return s1S + s2S + s3S;
}

TArray<FVector> AChunk::GetVerticesWithinBrush(FVector center, FVector playerLocation,bool centerSelf, float strength)
{
	TArray<FVector> ra;
	TArray<FVector> cellVertices;
	Cells.GenerateKeyArray(cellVertices);
	FVector relativeCenter;
	if (centerSelf == true) { relativeCenter = center - GetActorLocation(); }
	else { relativeCenter = center; }
	TArray<FVector> otherChunkVertices;	
	for (int i = 0; i < cellVertices.Num();i++)
	{
		//loop through all the cells
		if ((cellVertices[i].X <= (relativeCenter.X + brushSize)) && (cellVertices[i].X >= (relativeCenter.X - brushSize))
			&& (cellVertices[i].Y <= (relativeCenter.Y + brushSize)) && (cellVertices[i].Y >= (relativeCenter.Y - brushSize))
			&& (cellVertices[i].Z <= (relativeCenter.Z + (brushSize*2))) && (cellVertices[i].Z >= (relativeCenter.Z - (brushSize*2))))
		{
			//vertex is within brush 
			ra.Add(cellVertices[i]);	//maybe could return an array of indices
			//WithinEditArea(relativeCenter, cellVertices[i]);
			//GLog->Log("cell in brush x " + FString::FromInt(cellVertices[i].X) + " y " + FString::FromInt(cellVertices[i].Y) + " Z " + FString::FromInt(cellVertices[i].Z));
		}
		else
		{		
			////not in this chunk
			//if (centerSelf == true)
			//{
			//	//GLog->Log("lalala!!");
			//	FVector otherChunk = GetChunkVertex(cellVertices[i]);
			//	if (!otherChunkVertices.Contains(otherChunk) && otherChunk != GetActorLocation())
			//	{
			//		otherChunkVertices.Add(otherChunk);
			//		GLog->Log("Not In chunk");
			//	}
			//}	
		}
	}
	if (centerSelf == true)
	{
		FVector otherChunk = GetChunkVertex(FVector(relativeCenter.X + brushSize, relativeCenter.Y + brushSize, relativeCenter.Z+ (brushSize * 2)));
		if (!otherChunkVertices.Contains(otherChunk) && otherChunk != GetActorLocation()) {	otherChunkVertices.Add(otherChunk);	}

		otherChunk = GetChunkVertex(FVector(relativeCenter.X + brushSize, relativeCenter.Y - brushSize, relativeCenter.Z + (brushSize * 2)));
		if (!otherChunkVertices.Contains(otherChunk) && otherChunk != GetActorLocation()){	otherChunkVertices.Add(otherChunk);}

		otherChunk = GetChunkVertex(FVector(relativeCenter.X - brushSize, relativeCenter.Y - brushSize, relativeCenter.Z + (brushSize * 2)));
		if (!otherChunkVertices.Contains(otherChunk) && otherChunk != GetActorLocation()){	otherChunkVertices.Add(otherChunk);}

		otherChunk = GetChunkVertex(FVector(relativeCenter.X - brushSize, relativeCenter.Y + brushSize, relativeCenter.Z + (brushSize * 2)));
		if (!otherChunkVertices.Contains(otherChunk) && otherChunk != GetActorLocation()){	otherChunkVertices.Add(otherChunk);}
		
		otherChunk = GetChunkVertex(FVector(relativeCenter.X + brushSize, relativeCenter.Y + brushSize, relativeCenter.Z - (brushSize * 2)));
		if (!otherChunkVertices.Contains(otherChunk) && otherChunk != GetActorLocation()) { otherChunkVertices.Add(otherChunk); }

		otherChunk = GetChunkVertex(FVector(relativeCenter.X + brushSize, relativeCenter.Y - brushSize, relativeCenter.Z - (brushSize * 2)));
		if (!otherChunkVertices.Contains(otherChunk) && otherChunk != GetActorLocation()) { otherChunkVertices.Add(otherChunk); }

		otherChunk = GetChunkVertex(FVector(relativeCenter.X - brushSize, relativeCenter.Y - brushSize, relativeCenter.Z - (brushSize * 2)));
		if (!otherChunkVertices.Contains(otherChunk) && otherChunk != GetActorLocation()) { otherChunkVertices.Add(otherChunk); }

		otherChunk = GetChunkVertex(FVector(relativeCenter.X - brushSize, relativeCenter.Y + brushSize, relativeCenter.Z - (brushSize*2)));
		if (!otherChunkVertices.Contains(otherChunk) && otherChunk != GetActorLocation()) { otherChunkVertices.Add(otherChunk); }
	}
	if (centerSelf == true)
	{
		//need to call the other chunks to do likewise
		for (int i = 0; i < otherChunkVertices.Num();i++)
		{
			AChunk* newChunk = mapCont->GetChunk(otherChunkVertices[i]);
			if (newChunk != nullptr && newChunk != this)
			{
				GLog->Log("Update other Chunk");
				if (strength < 0)
				{
					//take
					newChunk->TakeBrush(UpdateCenterVertex(center,newChunk->GetActorLocation()), strength, playerLocation, false);
				}
				else
				{
					//add
					newChunk->AddBrush(UpdateCenterVertex(center, newChunk->GetActorLocation()), strength, playerLocation, false);	
				}
			}
			else{ GLog->Log("failed"); }
		}
	}
	return ra;
}

FVector AChunk::GetChunkVertex(FVector vertex)
{
	FVector newChunk = GetActorLocation();

	if (vertex.X < 0)
	{
		//x is less than
		newChunk.X = newChunk.X - chunkSize;
	}
	else if (vertex.X > chunkSize)
	{
		//x is greater than
		newChunk.X = newChunk.X + chunkSize;
	}
	if (vertex.Y < 0)
	{
		//y is less than
		newChunk.Y = newChunk.Y - chunkSize;
	}
	else if (vertex.Y > chunkSize)
	{
		//y is greater than
		newChunk.Y = newChunk.Y + chunkSize;
	}
	if (vertex.Z < 0)
	{
		//z is less than
		newChunk.Z = newChunk.Z - chunkSize;
	}
	else if (vertex.Z > chunkSize)
	{
		//z is greater than
		newChunk.Z = newChunk.Z + chunkSize;
	}
	return newChunk;
}

FVector AChunk::UpdateCenterVertex(FVector center, FVector newChunkPos)
{	
	return center - newChunkPos;
}

void AChunk::UpdateCells(TArray<FVector> cellsToChange, float changeAmount, FVector playerLocation, bool centerSelf, FVector center)
{
	TArray<AChunk *> otherChunksToUpdate;
	TArray<FVector> edgeCells;
	for (int i = 0; i < cellsToChange.Num(); i++)
	{
		Cells.Emplace(cellsToChange[i], Cells.FindRef(cellsToChange[i]) + (changeAmount/* * 10*/));
		editedCells.Add(cellsToChange[i], Cells.FindRef(cellsToChange[i]));
		UpdateSurroundingCells(cellsToChange[i],changeAmount);

		LoadMoreChunks(cellsToChange[i]);
		if (centerSelf == true)
		{
			FVector newCellPos = cellsToChange[i];		
			
			UpdateOtherChunkCells(otherChunksToUpdate, newCellPos, newCellPos.Z, FVector(1, 1, 0), FVector(0, 0, 1), Cells.FindRef(cellsToChange[i]) + changeAmount, changeAmount,center);
		}		
	}

	//Update other chunks
	for (int i = 0;i < otherChunksToUpdate.Num();i++)
	{
		otherChunksToUpdate[i]->UpdateMesh();
	}
	
	UpdateMesh();
}

void AChunk::UpdateOtherChunkCells(TArray<AChunk*>& otherChunks, FVector cellPos, float vaule, FVector oppersit, FVector mult,float cellVaule, float changeAmount,FVector center)
{
	if (cellPos.Z == 0)
	{//bottom layer so update top layer in chunk below
		GLog->Log("bottom layer");
		//find chunk below
		FVector chunkPos = GetActorLocation();
		chunkPos.Z -= chunkSize;
		AChunk* otherChunk = mapCont->GetChunk(chunkPos);
		if (otherChunk != nullptr && otherChunk != this)
		{
			FVector newCellPos = FVector(cellPos.X, cellPos.Y, chunkSize);
			newCellPos.Z = chunkSize;
			if (WithinEditArea(UpdateCenterVertex(center, otherChunk->GetActorLocation()), newCellPos))
			{
				otherChunk->UpdateCell(newCellPos, cellVaule);
				otherChunk->UpdateSurroundingCells(newCellPos, 0);
				if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
			}		
		}		
	}
	else if (cellPos.Z == chunkSize)
	{//top layer so update bottom layer in chunk above
		GLog->Log("top layer");
		//find chunk below
		FVector chunkPos = GetActorLocation();
		chunkPos.Z += chunkSize;
		AChunk* otherChunk = mapCont->GetChunk(chunkPos);
		if (otherChunk != nullptr && otherChunk != this)
		{
			FVector newCellPos = FVector(cellPos.X, cellPos.Y, 0);
			newCellPos.Z = 0;
			if (WithinEditArea(UpdateCenterVertex(center, otherChunk->GetActorLocation()), newCellPos))
			{
				otherChunk->UpdateCell(newCellPos, cellVaule);
				otherChunk->UpdateSurroundingCells(newCellPos, 0);
				if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
			}
		}		
	}
	UpdateCornerPos(cellPos,otherChunks,center);
	UpdateEdgePos(cellPos, otherChunks);
	UpdateFacePos(cellPos, otherChunks);	
}

bool AChunk::WithinEditArea(FVector center, FVector cellToEdit)
{//the edit area is double the brush size
	if ((cellToEdit.X <= (center.X + brushSize * 2)) && (cellToEdit.X >= (center.X - brushSize * 2))
		&& (cellToEdit.Y <= (center.Y + brushSize * 2)) && (cellToEdit.Y >= (center.Y - brushSize * 2))
		&& (cellToEdit.Z <= (center.Z + (brushSize * 3))) && (cellToEdit.Z >= (center.Z - (brushSize * 3))))
	{
		GLog->Log("True!!!");
		return true;
	}
	GLog->Log("false");
	return false;
}

void AChunk::UpdateSurroundingCells(FVector centerCell, float changeAmount)
{
	FVector cellPos;
	cellPos = centerCell;
	if (!CellsInMesh.Contains(centerCell) && WithinChunkBounds(centerCell)) { CellsInMesh.Add(centerCell);}
	cellPos.X += cellDistance;
	if (!CellsInMesh.Contains(cellPos) && WithinChunkBounds(cellPos)) { CellsInMesh.Add(cellPos); }
	cellPos.Y += cellDistance;
	if (!CellsInMesh.Contains(cellPos) && WithinChunkBounds(cellPos)) { CellsInMesh.Add(cellPos); }
	cellPos.X = centerCell.X;
	if (!CellsInMesh.Contains(cellPos) && WithinChunkBounds(cellPos)) { CellsInMesh.Add(cellPos); }
	cellPos.X -= cellDistance;
	if (!CellsInMesh.Contains(cellPos) && WithinChunkBounds(cellPos)) { CellsInMesh.Add(cellPos); }
	cellPos.Y = centerCell.Y;
	if (!CellsInMesh.Contains(cellPos) && WithinChunkBounds(cellPos)) { CellsInMesh.Add(cellPos); }
	cellPos.Y -= cellDistance;
	if (!CellsInMesh.Contains(cellPos) && WithinChunkBounds(cellPos)) { CellsInMesh.Add(cellPos); }
	cellPos.X = centerCell.X;
	if (!CellsInMesh.Contains(cellPos) && WithinChunkBounds(cellPos)) { CellsInMesh.Add(cellPos); }
	cellPos.X += cellDistance;
	if (!CellsInMesh.Contains(cellPos) && WithinChunkBounds(cellPos)) { CellsInMesh.Add(cellPos); }

	cellPos = centerCell;
	cellPos.Z += cellDistance;
	if (!CellsInMesh.Contains(centerCell) && WithinChunkBounds(centerCell)) { CellsInMesh.Add(centerCell); }
	cellPos.X += cellDistance;
	if (!CellsInMesh.Contains(cellPos) && WithinChunkBounds(cellPos)) { CellsInMesh.Add(cellPos); }
	cellPos.Y += cellDistance;
	if (!CellsInMesh.Contains(cellPos) && WithinChunkBounds(cellPos)) { CellsInMesh.Add(cellPos); }
	cellPos.X = centerCell.X;
	if (!CellsInMesh.Contains(cellPos) && WithinChunkBounds(cellPos)) { CellsInMesh.Add(cellPos); }
	cellPos.X -= cellDistance;
	if (!CellsInMesh.Contains(cellPos) && WithinChunkBounds(cellPos)) { CellsInMesh.Add(cellPos); }
	cellPos.Y = centerCell.Y;
	if (!CellsInMesh.Contains(cellPos) && WithinChunkBounds(cellPos)) { CellsInMesh.Add(cellPos); }
	cellPos.Y -= cellDistance;
	if (!CellsInMesh.Contains(cellPos) && WithinChunkBounds(cellPos)) { CellsInMesh.Add(cellPos); }
	cellPos.X = centerCell.X;
	if (!CellsInMesh.Contains(cellPos) && WithinChunkBounds(cellPos)) { CellsInMesh.Add(cellPos); }
	cellPos.X += cellDistance;
	if (!CellsInMesh.Contains(cellPos) && WithinChunkBounds(cellPos)) { CellsInMesh.Add(cellPos); }

	cellPos = centerCell;
	cellPos.Z -= cellDistance;
	if (!CellsInMesh.Contains(centerCell) && WithinChunkBounds(centerCell)) { CellsInMesh.Add(centerCell); }
	cellPos.X += cellDistance;
	if (!CellsInMesh.Contains(cellPos) && WithinChunkBounds(cellPos)) { CellsInMesh.Add(cellPos); }
	cellPos.Y += cellDistance;
	if (!CellsInMesh.Contains(cellPos) && WithinChunkBounds(cellPos)) { CellsInMesh.Add(cellPos); }
	cellPos.X = centerCell.X;
	if (!CellsInMesh.Contains(cellPos) && WithinChunkBounds(cellPos)) { CellsInMesh.Add(cellPos); }
	cellPos.X -= cellDistance;
	if (!CellsInMesh.Contains(cellPos) && WithinChunkBounds(cellPos)) { CellsInMesh.Add(cellPos); }
	cellPos.Y = centerCell.Y;
	if (!CellsInMesh.Contains(cellPos) && WithinChunkBounds(cellPos)) { CellsInMesh.Add(cellPos); }
	cellPos.Y -= cellDistance;
	if (!CellsInMesh.Contains(cellPos) && WithinChunkBounds(cellPos)) { CellsInMesh.Add(cellPos); }
	cellPos.X = centerCell.X;
	if (!CellsInMesh.Contains(cellPos) && WithinChunkBounds(cellPos)) { CellsInMesh.Add(cellPos); }
	cellPos.X += cellDistance;
	if (!CellsInMesh.Contains(cellPos) && WithinChunkBounds(cellPos)) { CellsInMesh.Add(cellPos); }
}

bool AChunk::WithinChunkBounds(FVector cellPos)
{
	if ((cellPos.X < 0 * cellDistance) && (cellPos.X > (cellAmount - 1 * cellDistance))
		&& (cellPos.Y < 0 * cellDistance) && (cellPos.Y > (cellAmount - 1 * cellDistance))
		&& (cellPos.Z < 0 * cellDistance) && (cellPos.Z > (cellAmount - 1 * cellDistance)))
	{
		return false;
	}
	return true;
}

void AChunk::UpdateCornerPos(FVector cellPos, TArray<AChunk*>& otherChunks, FVector center)
{
	if (cellPos.X == 0 && cellPos.Y == 0)
	{//Corner pos 1
		GLog->Log("Corner One");
		//update Chunks
		FVector chunkPos = GetActorLocation();
		AChunk* otherChunk = mapCont->GetChunk(FVector(chunkPos.X - chunkSize,chunkPos.Y,chunkPos.Z));
		UpdateOtherChunkCell(otherChunks, otherChunk, cellPos, FVector(chunkSize, 0, cellPos.Z), center);
		/*if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(chunkSize,0,cellPos.Z), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(chunkSize, 0, cellPos.Z), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}*/
		otherChunk = mapCont->GetChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y - chunkSize, chunkPos.Z));
		UpdateOtherChunkCell(otherChunks, otherChunk, cellPos, FVector(chunkSize, chunkSize, cellPos.Z), center);
		/*if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(chunkSize, chunkSize, cellPos.Z), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(chunkSize, chunkSize, cellPos.Z), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}*/
		otherChunk = mapCont->GetChunk(FVector(chunkPos.X, chunkPos.Y - chunkSize, chunkPos.Z));
		UpdateOtherChunkCell(otherChunks, otherChunk, cellPos, FVector(0, chunkSize, cellPos.Z), center);
		/*if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(0, chunkSize, cellPos.Z), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(0, chunkSize, cellPos.Z), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}*/
	}
	else if (cellPos.X == 0 && cellPos.Y == chunkSize)
	{//corner pos 2
		GLog->Log("Corner Two");
		FVector chunkPos = GetActorLocation();
		AChunk* otherChunk = mapCont->GetChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y, chunkPos.Z));
		UpdateOtherChunkCell(otherChunks, otherChunk, cellPos, FVector(chunkSize, chunkSize, cellPos.Z), center);
		/*if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(chunkSize, chunkSize, cellPos.Z), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(chunkSize, chunkSize, cellPos.Z), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}*/
		otherChunk = mapCont->GetChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y + chunkSize, chunkPos.Z));
		UpdateOtherChunkCell(otherChunks, otherChunk, cellPos, FVector(chunkSize, 0, cellPos.Z), center);
		/*if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(chunkSize, 0, cellPos.Z), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(chunkSize, 0, cellPos.Z), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}*/
		otherChunk = mapCont->GetChunk(FVector(chunkPos.X, chunkPos.Y + chunkSize, chunkPos.Z));
		UpdateOtherChunkCell(otherChunks, otherChunk, cellPos, FVector(0, 0, cellPos.Z), center);
		/*if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(0, 0, cellPos.Z), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(0, 0, cellPos.Z), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}*/
	}
	else if ((cellPos.X == chunkSize && cellPos.Y == 0))
	{//corner pos 3
		GLog->Log("Corner Three");
		FVector chunkPos = GetActorLocation();
		AChunk* otherChunk = mapCont->GetChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y, chunkPos.Z));
		UpdateOtherChunkCell(otherChunks, otherChunk, cellPos, FVector(0, 0, cellPos.Z), center);
		/*if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(0, 0, cellPos.Z), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(0, 0, cellPos.Z), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}*/
		otherChunk = mapCont->GetChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y - chunkSize, chunkPos.Z));
		UpdateOtherChunkCell(otherChunks, otherChunk, cellPos, FVector(0, chunkSize, cellPos.Z), center);
		/*if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(0, chunkSize, cellPos.Z), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(0, chunkSize, cellPos.Z), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}*/
		otherChunk = mapCont->GetChunk(FVector(chunkPos.X, chunkPos.Y - chunkSize, chunkPos.Z));
		UpdateOtherChunkCell(otherChunks, otherChunk, cellPos, FVector(chunkSize, chunkSize, cellPos.Z), center);
		/*if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(chunkSize, chunkSize, cellPos.Z), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(chunkSize, chunkSize, cellPos.Z), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}*/
	}
	else if ((cellPos.X == chunkSize && cellPos.Y == chunkSize))
	{//corner pos 4
		GLog->Log("Corner Four");
		FVector chunkPos = GetActorLocation();
		AChunk* otherChunk = mapCont->GetChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y, chunkPos.Z));
		UpdateOtherChunkCell(otherChunks, otherChunk, cellPos, FVector(0, chunkSize, cellPos.Z), center);
		/*if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(0, chunkSize, cellPos.Z), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(0, chunkSize, cellPos.Z), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}*/
		otherChunk = mapCont->GetChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y + chunkSize, chunkPos.Z));
		UpdateOtherChunkCell(otherChunks, otherChunk, cellPos, FVector(0, 0, cellPos.Z), center);
		/*if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(0, 0, cellPos.Z), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(0, 0, cellPos.Z), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}*/
		otherChunk = mapCont->GetChunk(FVector(chunkPos.X, chunkPos.Y + chunkSize, chunkPos.Z));
		UpdateOtherChunkCell(otherChunks, otherChunk, cellPos, FVector(0, chunkSize, cellPos.Z), center);
		/*if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(0, chunkSize, cellPos.Z), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(0, chunkSize, cellPos.Z), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}*/
	}
}

void AChunk::UpdateEdgePos(FVector cellPos, TArray<AChunk*>& otherChunks)
{
	if (cellPos.X == 0 && cellPos.Z == 0)
	{//edge 1
		GLog->Log("Edge One");
		//update Chunks
		FVector chunkPos = GetActorLocation();
		AChunk* otherChunk = mapCont->GetChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y, chunkPos.Z));
		if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(chunkSize, cellPos.Y, 0), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(chunkSize, cellPos.Y, 0), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}
		otherChunk = mapCont->GetChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y, chunkPos.Z- chunkSize));
		if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(chunkSize, cellPos.Y, chunkSize), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(chunkSize, cellPos.Y, chunkSize), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}
		otherChunk = mapCont->GetChunk(FVector(chunkPos.X, chunkPos.Y, chunkPos.Z - chunkSize));
		if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(0, cellPos.Y, chunkSize), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(0, cellPos.Y, chunkSize), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}
	}
	else if (cellPos.X == chunkSize && cellPos.Z == 0)
	{//edge 2
		GLog->Log("Edge two");
		FVector chunkPos = GetActorLocation();
		AChunk* otherChunk = mapCont->GetChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y, chunkPos.Z));
		if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(0, cellPos.Y, 0), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(0, cellPos.Y, 0), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}
		otherChunk = mapCont->GetChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y, chunkPos.Z - chunkSize));
		if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(0, cellPos.Y, chunkSize), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(0, cellPos.Y, chunkSize), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}
		otherChunk = mapCont->GetChunk(FVector(chunkPos.X, chunkPos.Y, chunkPos.Z - chunkSize));
		if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(chunkSize, cellPos.Y, chunkSize), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(chunkSize, cellPos.Y, chunkSize), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}
	}
	else if (cellPos.Y == 0 && cellPos.Z == 0)
	{//edge 3
		GLog->Log("Edge Three");
		//update Chunks
		FVector chunkPos = GetActorLocation();
		AChunk* otherChunk = mapCont->GetChunk(FVector(chunkPos.X, chunkPos.Y - chunkSize, chunkPos.Z));
		if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(cellPos.X, chunkSize, 0), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(cellPos.X, chunkSize, 0), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}
		otherChunk = mapCont->GetChunk(FVector(chunkPos.X, chunkPos.Y - chunkSize, chunkPos.Z - chunkSize));
		if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(cellPos.X, chunkSize, chunkSize), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(cellPos.X, chunkSize, chunkSize), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}
		otherChunk = mapCont->GetChunk(FVector(chunkPos.X, chunkPos.Y, chunkPos.Z - chunkSize));
		if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(cellPos.X, 0, chunkSize), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(cellPos.X, 0, chunkSize), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}
	}
	else if (cellPos.Y == chunkSize && cellPos.Z == 0)
	{//edge 4
		GLog->Log("Edge Four");
		//update cells
		FVector chunkPos = GetActorLocation();
		AChunk* otherChunk = mapCont->GetChunk(FVector(chunkPos.X, chunkPos.Y + chunkSize, chunkPos.Z));
		if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(cellPos.X, 0, 0), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(cellPos.X, 0, 0), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}
		otherChunk = mapCont->GetChunk(FVector(chunkPos.X, chunkPos.Y + chunkSize, chunkPos.Z - chunkSize));
		if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(cellPos.X, 0, chunkSize), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(cellPos.X, 0, chunkSize), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}
		otherChunk = mapCont->GetChunk(FVector(chunkPos.X, chunkPos.Y, chunkPos.Z - chunkSize));
		if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(cellPos.X, chunkSize, chunkSize), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(cellPos.X, chunkSize, chunkSize), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}
	}
	else if (cellPos.X == 0 && cellPos.Z == chunkSize)
	{//edge 5
		GLog->Log("Edge Five");
		FVector chunkPos = GetActorLocation();
		AChunk* otherChunk = mapCont->GetChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y, chunkPos.Z));
		if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(chunkSize, cellPos.Y, chunkSize), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(chunkSize, cellPos.Y, chunkSize), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}
		otherChunk = mapCont->GetChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y, chunkPos.Z + chunkSize));
		if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(chunkSize, cellPos.Y, 0), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(chunkSize, cellPos.Y, 0), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}
		otherChunk = mapCont->GetChunk(FVector(chunkPos.X, chunkPos.Y, chunkPos.Z + chunkSize));
		if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(0, cellPos.Y, 0), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(0, cellPos.Y, 0), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}
	}
	else if (cellPos.X == chunkSize && cellPos.Z == chunkSize)
	{//edge 6
		GLog->Log("Edge Six");
		FVector chunkPos = GetActorLocation();
		AChunk* otherChunk = mapCont->GetChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y, chunkPos.Z));
		if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(0, cellPos.Y, chunkSize), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(0, cellPos.Y, chunkSize), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}
		otherChunk = mapCont->GetChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y, chunkPos.Z + chunkSize));
		if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(0, cellPos.Y, 0), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(0, cellPos.Y, 0), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}
		otherChunk = mapCont->GetChunk(FVector(chunkPos.X, chunkPos.Y, chunkPos.Z + chunkSize));
		if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(chunkSize, cellPos.Y, 0), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(chunkSize, cellPos.Y, 0), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}
	}
	else if (cellPos.Y == 0 && cellPos.Z == chunkSize)
	{//edge 7
		GLog->Log("Edge Seven");
		//update Chunks
		FVector chunkPos = GetActorLocation();
		AChunk* otherChunk = mapCont->GetChunk(FVector(chunkPos.X, chunkPos.Y - chunkSize, chunkPos.Z));
		if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(cellPos.X, chunkSize, chunkSize), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(cellPos.X, chunkSize, chunkSize), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}
		otherChunk = mapCont->GetChunk(FVector(chunkPos.X, chunkPos.Y - chunkSize, chunkPos.Z + chunkSize));
		if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(cellPos.X, chunkSize, 0), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(cellPos.X, chunkSize, 0), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}
		otherChunk = mapCont->GetChunk(FVector(chunkPos.X, chunkPos.Y, chunkPos.Z + chunkSize));
		if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(cellPos.X, 0, 0), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(cellPos.X, 0, 0), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}
	}
	else if (cellPos.Y == chunkSize && cellPos.Z == chunkSize)
	{//edge 8
		GLog->Log("Edge Eight");
		//update cells
		FVector chunkPos = GetActorLocation();
		AChunk* otherChunk = mapCont->GetChunk(FVector(chunkPos.X, chunkPos.Y + chunkSize, chunkPos.Z));
		if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(cellPos.X, 0, chunkSize), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(cellPos.X, 0, chunkSize), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}
		otherChunk = mapCont->GetChunk(FVector(chunkPos.X, chunkPos.Y + chunkSize, chunkPos.Z + chunkSize));
		if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(cellPos.X, 0, 0), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(cellPos.X, 0, 0), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}
		otherChunk = mapCont->GetChunk(FVector(chunkPos.X, chunkPos.Y, chunkPos.Z + chunkSize));
		if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(cellPos.X, chunkSize, 0), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(cellPos.X, chunkSize, 0), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}
	}
}

void AChunk::UpdateFacePos(FVector cellPos, TArray<AChunk*>& otherChunks)
{
	if (cellPos.X == 0)
	{//face 1
		GLog->Log("Face One");
		//update Chunks
		FVector chunkPos = GetActorLocation();
		AChunk* otherChunk = mapCont->GetChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y, chunkPos.Z));
		if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(chunkSize, cellPos.Y, cellPos.Z), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(chunkSize, cellPos.Y, cellPos.Z), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}
	}
	else if (cellPos.X == chunkSize)
	{//face 2
		GLog->Log("Face Two");
		//update Chunks
		FVector chunkPos = GetActorLocation();
		AChunk* otherChunk = mapCont->GetChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y, chunkPos.Z));
		if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(0, cellPos.Y, cellPos.Z), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(0, cellPos.Y, cellPos.Z), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}
	}
	else if (cellPos.Y == 0)
	{//face 3
		GLog->Log("Face Three");
		//update Chunks
		FVector chunkPos = GetActorLocation();
		AChunk* otherChunk = mapCont->GetChunk(FVector(chunkPos.X, chunkPos.Y - chunkSize, chunkPos.Z));
		if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(cellPos.X, chunkSize, cellPos.Z), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(cellPos.X, chunkSize, cellPos.Z), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}
	}
	else if (cellPos.Y == chunkSize)
	{//face 4
		GLog->Log("Face Four");
		//update Chunks
		FVector chunkPos = GetActorLocation();
		AChunk* otherChunk = mapCont->GetChunk(FVector(chunkPos.X, chunkPos.Y + chunkSize, chunkPos.Z));
		if (otherChunk != nullptr && otherChunk != this)
		{
			otherChunk->UpdateCell(FVector(cellPos.X, 0, cellPos.Z), Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(FVector(cellPos.X, 0, cellPos.Z), 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}
	}
}

void AChunk::UpdateOtherChunkCell(TArray<AChunk*>& otherChunks, AChunk* otherChunk, FVector cellPos, FVector newCellPos,FVector center)
{
	if (otherChunk != nullptr && otherChunk != this)
	{
		if (WithinEditArea(UpdateCenterVertex(center, otherChunk->GetActorLocation()), newCellPos))
		{
			otherChunk->UpdateCell(newCellPos, Cells.FindRef(cellPos));
			otherChunk->UpdateSurroundingCells(newCellPos, 0);
			if (!otherChunks.Contains(otherChunk)) { otherChunks.Add(otherChunk); }
		}
	}
}

void AChunk::LoadMoreChunks(FVector cellPos)
{
	if (cellPos.Z == 0)
	{//check chunk below the chunk below
		FVector chunkPos = GetActorLocation();
		//chunkPos.Z -= (chunkSize * 2);
		float z = chunkHeight - 2;
		chunkCont->CheckChunks(FVector(chunkPos.X, chunkPos.Y, z));
		//check chunks around this pos
		mapCont->CheckChunk(FVector(chunkPos.X + chunkSize,chunkPos.Y, z));
		mapCont->CheckChunk(FVector(chunkPos.X,chunkPos.Y + chunkSize, z));
		mapCont->CheckChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y + chunkSize, z));
		mapCont->CheckChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y - chunkSize, z));
		mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y - chunkSize, z));
		mapCont->CheckChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y, z));
		mapCont->CheckChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y + chunkSize, z));
		mapCont->CheckChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y - chunkSize, z));
	}
	else if (cellPos.Z == chunkSize)
	{//check chunk above the chunk above
		FVector chunkPos = GetActorLocation();
		//chunkPos.Z += (chunkSize * 2);
		float z = chunkHeight + 2;
		chunkCont->CheckChunks(FVector(chunkPos.X, chunkPos.Y, z));
		//check chunks around this pos
		mapCont->CheckChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y, z));
		mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y + chunkSize, z));
		mapCont->CheckChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y + chunkSize, z));
		mapCont->CheckChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y - chunkSize, z));
		mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y - chunkSize, z));
		mapCont->CheckChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y, z));
		mapCont->CheckChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y + chunkSize, z));
		mapCont->CheckChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y - chunkSize, z));
	}

	if (cellPos.X == 0)
	{//load chunk at minus x 
		FVector chunkPos = GetActorLocation();
		chunkPos.X -= (chunkSize * 2);
		float z = chunkHeight;
		mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y, z));		
		//check chunks around this pos
		mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y + chunkSize, z));
		mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y - chunkSize, z));
		mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y - chunkSize, z + 1));
		mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y, chunkPos.Z + z));
		mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y + chunkSize, z + 1));
		mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y - chunkSize, z - 1));
		mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y, z - 1));
		mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y + chunkSize, z - 1));
	}
	else if (cellPos.X == chunkSize)
	{//load chunk at plus x
		FVector chunkPos = GetActorLocation();
		chunkPos.X += (chunkSize * 2);
		float z = chunkHeight;
		mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y, z));
		//check chunks around this pos
		mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y + chunkSize, z));
		mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y - chunkSize, z));
		mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y - chunkSize, z + 1));
		mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y, chunkPos.Z + z));
		mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y + chunkSize, z + 1));
		mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y - chunkSize, z - 1));
		mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y, z - 1));
		mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y + chunkSize, z - 1));
	}

	if (cellPos.Y == 0)
	{//load chunk at minus y
		FVector chunkPos = GetActorLocation();
		chunkPos.Y -= (chunkSize * 2);
		float z = chunkHeight;
		mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y, z));
		//check chunks around this pos
		mapCont->CheckChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y, z));
		mapCont->CheckChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y, z));
		mapCont->CheckChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y, z + 1));
		mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y, z + 1));
		mapCont->CheckChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y, z + 1));
		mapCont->CheckChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y, z - 1));
		mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y, z - 1));
		mapCont->CheckChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y, z - 1));
	}
	else if (cellPos.Y == chunkSize)
	{//load chunk at plus y
		FVector chunkPos = GetActorLocation();
		chunkPos.Y += (chunkSize * 2);
		float z = chunkHeight;
		mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y, z));
		//check chunks around this pos
		mapCont->CheckChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y, z));
		mapCont->CheckChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y, z));
		mapCont->CheckChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y, z + 1));
		mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y, z + 1));
		mapCont->CheckChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y, z + 1));
		mapCont->CheckChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y, z - 1));
		mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y, z - 1));
		mapCont->CheckChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y, z - 1));
	}
	//always make sure there are chunks arround this chunk
	FVector chunkPos = GetActorLocation();
	//chunkPos.Z -= (chunkSize * 2);
	float z = chunkHeight;
	mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y, z));
	//check chunks around this pos
	mapCont->CheckChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y, z));
	mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y + chunkSize, z));
	mapCont->CheckChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y + chunkSize, z));
	mapCont->CheckChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y - chunkSize, z));
	mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y - chunkSize, z));
	mapCont->CheckChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y, z));
	mapCont->CheckChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y + chunkSize, z));
	mapCont->CheckChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y - chunkSize, z));

	mapCont->CheckChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y, z - 1));
	mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y + chunkSize, z - 1));
	mapCont->CheckChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y + chunkSize, z - 1));
	mapCont->CheckChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y - chunkSize, z - 1));
	mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y - chunkSize, z - 1));
	mapCont->CheckChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y, z - 1));
	mapCont->CheckChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y + chunkSize, z - 1));
	mapCont->CheckChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y - chunkSize, z - 1));

	mapCont->CheckChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y, z + chunkSize));
	mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y + chunkSize, z + chunkSize));
	mapCont->CheckChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y + chunkSize, z + 1));
	mapCont->CheckChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y - chunkSize, z + 1));
	mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y - chunkSize, z + 1));
	mapCont->CheckChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y, z + 1));
	mapCont->CheckChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y + chunkSize, z + 1));
	mapCont->CheckChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y - chunkSize, z + 1));
}

void AChunk::LoadMoreChunks_ICM()
{
	//always make sure there are chunks arround this chunk
	FVector chunkPos = GetActorLocation();
	//chunkPos.Z -= (chunkSize * 2);
	float z = chunkHeight;
	mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y, z));
	//check chunks around this pos
	mapCont->CheckChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y, z));
	mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y + chunkSize, z));
	mapCont->CheckChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y + chunkSize, z));
	mapCont->CheckChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y - chunkSize, z));
	mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y - chunkSize, z));
	mapCont->CheckChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y, z));
	mapCont->CheckChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y + chunkSize, z));
	mapCont->CheckChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y - chunkSize, z));

	mapCont->CheckChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y, z - 1));
	mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y + chunkSize, z - 1));
	mapCont->CheckChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y + chunkSize, z - 1));
	mapCont->CheckChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y - chunkSize, z - 1));
	mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y - chunkSize, z - 1));
	mapCont->CheckChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y, z - 1));
	mapCont->CheckChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y + chunkSize, z - 1));
	mapCont->CheckChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y - chunkSize, z - 1));

	mapCont->CheckChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y, z + chunkSize));
	mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y + chunkSize, z + chunkSize));
	mapCont->CheckChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y + chunkSize, z + 1));
	mapCont->CheckChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y - chunkSize, z + 1));
	mapCont->CheckChunk(FVector(chunkPos.X, chunkPos.Y - chunkSize, z + 1));
	mapCont->CheckChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y, z + 1));
	mapCont->CheckChunk(FVector(chunkPos.X - chunkSize, chunkPos.Y + chunkSize, z + 1));
	mapCont->CheckChunk(FVector(chunkPos.X + chunkSize, chunkPos.Y - chunkSize, z + 1));
}










