// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSelector.h"
#include "SaveGameSelector.h"
#include "Kismet/GameplayStatics.h"
#include <BuildingGame/MapController.h>

// Sets default values
AGameSelector::AGameSelector()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	inGame = false;

}

// Called when the game starts or when spawned
void AGameSelector::BeginPlay()
{
	Super::BeginPlay();
	graphicsIndices.Add(1);
	graphicsIndices.Add(1);
	graphicsIndices.Add(1);
	graphicsIndices.Add(1);
	graphicsIndices.Add(1);
	graphicsIndices.Add(1);
	LoadData();
}

// Called every frame
void AGameSelector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGameSelector::SaveData()
{
	renderDistance = 5 * ((graphicsIndices[5] +1) * 2);
	USaveGameSelector* SaveGameDataInstance = Cast<USaveGameSelector>(UGameplayStatics::CreateSaveGameObject(USaveGameSelector::StaticClass()));
	SaveGameDataInstance->GameName = gameName;
	SaveGameDataInstance->GameNames = gameNames;
	SaveGameDataInstance->GraphicsIndices = graphicsIndices;
	SaveGameDataInstance->RenderDistance = renderDistance;

	GLog->Log("Save data!!");
	GLog->Log(FString::FromInt(SaveGameDataInstance->GameNames.Num()));
	UGameplayStatics::SaveGameToSlot(SaveGameDataInstance, "SaveGameNames", 0);
	if (inGame && mapRef != nullptr) { mapRef->viewDistance = renderDistance; }
}

void AGameSelector::LoadData()
{
	USaveGameSelector* SaveGameDataInstance = Cast<USaveGameSelector>(UGameplayStatics::CreateSaveGameObject(USaveGameSelector::StaticClass()));
	//load the save game data into our variable
	SaveGameDataInstance = Cast<USaveGameSelector>(UGameplayStatics::LoadGameFromSlot("SaveGameNames", 0));
	if (SaveGameDataInstance != nullptr)
	{
		gameName = SaveGameDataInstance->GameName;
		gameNames = SaveGameDataInstance->GameNames;
		graphicsIndices = SaveGameDataInstance->GraphicsIndices;
		renderDistance = SaveGameDataInstance->RenderDistance;

		GLog->Log("Loaded data!!");
		GLog->Log(FString::FromInt(gameNames.Num()));
	}
}

int AGameSelector::AddNewSaveGame(FString name)
{
	return gameNames.Add(name);
}

void AGameSelector::SelectGame(int index)
{
	if (gameNames.IsValidIndex(index))
	{
		gameName = gameNames[index];
	}	
}

FString AGameSelector::GetGameName(int index)
{
	if (gameNames.IsValidIndex(index))
	{
		return gameNames[index];
	}
	return FString();
}

void AGameSelector::SetInGame()
{
	inGame = true;
	mapRef = GetWorld()->SpawnActor<AMapController>();
	if (mapRef != nullptr)
	{
		mapRef->gameName = gameName;
		mapRef->viewDistance = renderDistance;
		mapRef->LoadGame();
	}
}

 bool AGameSelector::ContainName(FString name)
{
	 if (gameNames.Contains(name))
	 {
		 return true;
	 }
	return false;
}

 void AGameSelector::SetGraphicsSetting(int type, int index)
 {
	 if (graphicsIndices.IsValidIndex(type)) { graphicsIndices[type] = index; }
	 else { graphicsIndices.Add(index); }
 }

 int AGameSelector::GetGraphicsSetting(int type)
 {
	 if (graphicsIndices.IsValidIndex(type)) { return graphicsIndices[type]; }
	 else { return 1; }	
 }

 void AGameSelector::SaveGame()
 {
	 if (mapRef != nullptr) { mapRef->SaveGame(); }
 }

