// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameSelector.generated.h"

UCLASS()
class BUILDINGGAME_API AGameSelector : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameSelector();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
		void SaveData();	//this will save the data
	UFUNCTION(BlueprintCallable)
		void LoadData();	//this will load the data
	UFUNCTION(BlueprintCallable)
		int AddNewSaveGame(FString name);	//this will add a new save game to the array
	UFUNCTION(BlueprintCallable)
		void SelectGame(int index);	//this will select the game name of this index
	UFUNCTION(BlueprintCallable)
		int GetAmountOfGames() { return gameNames.Num(); }	//this will return the amount of game names
	UFUNCTION(BlueprintCallable)
		FString GetGameName(int index);	//this will return the game name 
	UFUNCTION(BlueprintCallable)
		void SetInGame();	//this will spawn the map controller 
	UFUNCTION(BlueprintCallable)
		bool ContainName(FString name);	//this will check to see if the name is in the array 
	UFUNCTION(BlueprintCallable)
		void SetGraphicsSetting(int type, int index);	//this will set the type to index
	UFUNCTION(BlueprintCallable)
		int GetGraphicsSetting(int type);	//this will return the type
	UFUNCTION(BlueprintCallable)
		void SaveGame();	//this will save the game

private:
	FString gameName;	//this is the game name
	TArray<FString> gameNames; //this stores all the names of all the games
	TArray<int> graphicsIndices;	//this will store the relative index
	int renderDistance;
	int inGame;
	class AMapController* mapRef;
	
};
