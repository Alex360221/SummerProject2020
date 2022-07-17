// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkController.h"
#include "Chunk.h"
#include "MapController.h"

// Sets default values
AChunkController::AChunkController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	chunksInColumn = 15;
	chunkSize = (10 - 1) * 50;
	spawnedAllChunks = false;
	chunksAnlaysed = false;
	tickToggle = true;
	lodChunks = 0;
	ThisScene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = ThisScene;
}

// Called when the game starts or when spawned
void AChunkController::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AChunkController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (spawnedAllChunks == false)
	{
		TArray<AChunk*> chunkRefs;
		chunks.GenerateValueArray(chunkRefs);
		bool checkAllChunksSpawned = false;
		for (int i = 0; i < chunkRefs.Num();i++)
		{
			if (chunkRefs.IsValidIndex(i))
			{
				if (chunkRefs[i]->hasChunkLoaded == true) { checkAllChunksSpawned = true; }
				else { checkAllChunksSpawned = false; }
			}
		}
		spawnedAllChunks = checkAllChunksSpawned;
	}
	if (spawnedAllChunks == true && chunksAnlaysed == false)
	{//anlysas chunks and remove ones with nothing in
		//first get all the data to see if the chunk should be kept loaded
		TArray<AChunk*> chunkRefs;
		chunks.GenerateValueArray(chunkRefs);
		for (int i = 0; i < chunks.Num();i++)
		{
			if (chunkRefs.IsValidIndex(i))
			{
				if (chunkRefs[i] != nullptr)
				{
					if (chunksToKeep.IsValidIndex(i))
					{
						chunksToKeep[i] = chunkRefs[i]->KeepChunkLoaded();
					}
				}
			}		
		}
		AnlayseChunks();
	}
}

void AChunkController::SpawnChunks(AMapController* mapRef,TArray<UStaticMesh*> newMeshes,FString gameName)
{
	mapControllerRef = mapRef;
	staticMeshesPoint = newMeshes;
	GameName = gameName;
	for (int i = 0; i < chunksInColumn;i++)
	{
		chunksToKeep.Add(0);
		//spawn chunk
		FVector location = GetActorLocation();
		location.Z = i * chunkSize;
		FRotator rotation{ 0.0f };
		AChunk* newActor = GetWorld()->SpawnActor<AChunk>(location, rotation);
		newActor->mapCont = mapRef;
		newActor->chunkCont = this;
		TArray<UStaticMesh*> meshes;
		/*for (int i = 0; i < instancedMeshes.Num();i++)
		{
			meshes.Add(instancedMeshes[i]->GetStaticMesh());
		}*/
		newActor->SetChunkInstanceMeshes(newMeshes);
		newActor->gameName = gameName;
		newActor->chunkLevel = FString::FromInt(i);
		newActor->chunkHeight = i;
		newActor->LoadChunk();
		//newActor->bSeed = bSeed;
		//newActor->eSeed = eSeed;
		//newActor->euSeed = euSeed;
		//newActor->edSeed = edSeed;

		//newActor->CreateChunk();

		//set seed in chunk
		chunks.Add(location,newActor);
	}
}

void AChunkController::SpawnChunk(FVector chunkPos, int chunkHeight)
{
	FRotator rotation{ 0.0f };
	AChunk* newActor = GetWorld()->SpawnActor<AChunk>(chunkPos, rotation);
	chunks.Add(chunkPos, newActor);
	newActor->mapCont = mapControllerRef;
	newActor->chunkCont = this;
	TArray<UStaticMesh*> meshes;
	/*for (int i = 0; i < instancedMeshes.Num();i++)
	{
		meshes.Add(instancedMeshes[i]->GetStaticMesh());
	}*/
	newActor->SetChunkInstanceMeshes(staticMeshesPoint);
	newActor->gameName = GameName;
	newActor->chunkLevel = FString::FromInt(chunkHeight);
	newActor->chunkHeight = chunkHeight;
	newActor->LoadChunk();
	//newActor->bSeed = bSeed;
	//newActor->eSeed = eSeed;
	//newActor->euSeed = euSeed;
	//newActor->edSeed = edSeed;

	//newActor->CreateChunk();

	//set seed in chunk
	
}

bool AChunkController::RemoveChunks()
{
	if (chunksAnlaysed == true)
	{
		GLog->Log("removing chunks");
		TArray<AChunk*> chunkRefs;
		TArray<FVector> chunkPoses;
		chunks.GenerateValueArray(chunkRefs);
		chunks.GenerateKeyArray(chunkPoses);
		for (int i = 0;i < chunkRefs.Num();i++)
		{
			if (chunkRefs.IsValidIndex(i) == true)
			{
				AChunk* chunk = chunkRefs[i];
				if (chunk != nullptr)
				{
					if (chunk->RemoveChunk() == true)	//this removes all the chunk data
					{
						chunk->Destroy();
						//chunks.Remove(chunkPoses[i]);
					}
					else { return false; }	//return false as not all chunks have been removed
				}
			}
		}	
		GLog->Log("done removing chunks");
		chunks.Empty();
		return true;
	}
	return false;
}

void AChunkController::SaveChunks()
{
	TArray<AChunk*> chunkRefs;
	chunks.GenerateValueArray(chunkRefs);
	for (int i = 0;i < chunkRefs.Num();i++)
	{
		AChunk* chunk = chunkRefs[i];
		chunk->SaveChunk();	//this removes all the chunk data
	}
}

AChunk* AChunkController::FindChunk(FVector chunkPos)
{
	if (chunks.Contains(chunkPos))
	{
		return chunks.FindRef(chunkPos);
	}
	return nullptr;
}

void AChunkController::CheckChunks(FVector chunkPos)
{
	int i = chunkPos.Z;
	chunkPos.Z = chunkPos.Z * chunkSize;
	if (chunkPos.Z >= 0 && chunkPos.Z <= chunksInColumn * chunkSize)
	{
		if (!chunks.Contains(chunkPos))
		{//spawn chunk
			FRotator rotation{ 0.0f };
			AChunk* newActor = GetWorld()->SpawnActor<AChunk>(chunkPos, rotation);
			newActor->mapCont = mapControllerRef;
			newActor->chunkCont = this;
			TArray<UStaticMesh*> meshes;

			newActor->SetChunkInstanceMeshes(staticMeshesPoint);
			newActor->gameName = GameName;
		//	int i = int(chunkPos.Z / chunkSize) + (chunkSize/2);
			newActor->chunkLevel = FString::FromInt(int(i));
			newActor->chunkHeight = i;
			newActor->LoadChunk();
			//newActor->bSeed = baseSeed;
			//newActor->eSeed = eSeed;
			//newActor->euSeed = euSeed;
			//newActor->edSeed = edSeed;
			GLog->Log("SPAWN CHUNK UNDER !!!!!!!!!!!!!!!!!!!!!!!!!");
			//newActor->CreateChunk();

			//set seed in chunk
			chunks.Add(chunkPos, newActor);
		}
	}
}

bool AChunkController::CheckOtherChunkTerrain(FVector cellPos, FVector chunkPos)
{
	if (chunks.Contains(chunkPos))
	{
		AChunk* chunk = chunks.FindRef(chunkPos);
		if (chunk !=nullptr)
		{
			return chunk->OtherCellTerrain(cellPos);
		}
	}
	return true;
}

void AChunkController::UpdateChunksLOD(int LOD)
{//this will set the lod for all the chunks
	if (lodChunks != LOD)
	{
		GLog->Log("chunk LOD update");
		lodChunks = LOD;
		chunksAnlaysed = false;
	}
}

void AChunkController::UpdateChunksTargetVertiecs(int target)
{//this will set the target vertex count for each chunk if the target as changed
	if (chunksTargetVertiecs != target)
	{
		chunksTargetVertiecs = target;
		TArray<AChunk*> values;
		chunks.GenerateValueArray(values);
		for (int i = 0; i < values.Num();i++)
		{
			if (values.IsValidIndex(i))
			{
				if (values[i] != nullptr)
				{
					values[i]->SetTargetVertices(target);
				}
			}
		}
	}
}

void AChunkController::ToggleTick(bool toggle)
{
	if (toggle != tickToggle)
	{
		if (toggle == false)
		{
			if (spawnedAllChunks == true && chunksAnlaysed == true)
			{//turn off tick 
				SetActorTickEnabled(false);
				tickToggle = false;
				TArray<AChunk*> value;
				chunks.GenerateValueArray(value);
				for (int i = 0; i < value.Num();i++)
				{
					if (value[i] != nullptr)
					{
						value[i]->SetActorTickEnabled(toggle);
					}
				}
			}
		}
		else
		{
			SetActorTickEnabled(true);
			tickToggle = true;
			TArray<AChunk*> value;
			chunks.GenerateValueArray(value);
			for (int i = 0; i < value.Num();i++)
			{
				if (value[i] != nullptr)
				{
					value[i]->SetActorTickEnabled(toggle);
				}
			}
		}
	}
}

void AChunkController::AnlayseChunks()
{
	aChunks.Empty();
	TArray<int>chunkVaules;
	//chunksToKeep.GenerateValueArray(chunkVaules);
	for (int i = 0; i < chunksToKeep.Num();i++)
	{
		if (chunksToKeep.IsValidIndex(i))
		{
			if(chunksToKeep[i] == 1)
			{
				if (lodChunks == 0)
				{
					aChunks.Add(1);
					if (aChunks.IsValidIndex(i))
					{
						aChunks[i] = 1;
					}
					else { aChunks.Add(1); }
					if (aChunks.IsValidIndex(i - 1))
					{
						aChunks[i - 1] = 1;
					}//don't add as minus -1 should always be vaild unless i = 0 
					if (i + 1 < chunksInColumn)
					{
						if (aChunks.IsValidIndex(i + 1))
						{
							aChunks[i + 1] = 1;
						}
						else { aChunks.Add(1); }
					}
				}
				else
				{
					aChunks.Add(1);
				}

			}
		else
		{
			if (!aChunks.IsValidIndex(i))
			{
				aChunks.Add(0);
			}
		}
		}
		
	}
	TArray<AChunk*> chunkRefs;
	TArray<FVector> keys;
	chunks.GenerateValueArray(chunkRefs);
	chunks.GenerateKeyArray(keys);
	for (int i = 0; i < aChunks.Num();i++)
	{//check if i is vaild index
		if (aChunks.IsValidIndex(i))
		{
			if (aChunks[i] == 0)
			{
				if (chunkRefs.IsValidIndex(i))
				{//if vaild chunk needs removing 
					AChunk* chunk = chunkRefs[i];
					if (chunk != nullptr)
					{
						FVector chunkPos = chunk->GetActorLocation();
						chunk->RemoveChunk();	//this removes all the chunk data
						chunk->Destroy();
						chunks.Remove(chunkPos);
					}
				}
			}
			else if (aChunks[i] == 1)
			{//check chunk is spawned
				FVector newChunkPos = GetActorLocation();
				newChunkPos.Z = i * chunkSize;
				if (!chunks.Contains(newChunkPos))
				{//if its not contained then that chunk isn't spawned so spawn it				
					SpawnChunk(newChunkPos, i);
				}
			}
		}		
	}
	chunksAnlaysed = true;
}

