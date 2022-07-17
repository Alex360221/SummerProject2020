// Fill out your copyright notice in the Description page of Project Settings.


#include "InstanceCustomMeshData.h"
#include <BuildingGame/MapController.h>
#include <BuildingGame/PartBuilder.h>
#include <BuildingGame/Meshes/ICMType.h>
#include <BuildingGame/Meshes/ICMData.h>
#include <BuildingGame/Meshes/ICMKeyStorage.h>

InstanceCustomMeshData::InstanceCustomMeshData()
{
}

InstanceCustomMeshData::InstanceCustomMeshData(TArray<FVector> newPositions, TArray<int> newInstanceType, int newID, int newMatIndex)
{
	positions = newPositions;
	instanceType = newInstanceType;
	ICMID = newID;
	matIndex = newMatIndex;
}

void InstanceCustomMeshData::SetInstanceCustomMeshData(TArray<FVector> newPositions, TArray<int> newInstanceType, int newID)
{
	positions = newPositions;
	instanceType = newInstanceType;
	ICMID = newID;
}

TArray<FVector> InstanceCustomMeshData::GetPositions()
{
	return positions;
}

TArray<int> InstanceCustomMeshData::GetInstancesType()
{
	return instanceType;
}

void InstanceCustomMeshData::CreateICM(FVector pos, AMapController* map)
{
	//ONLY USE IF ONE ICM OF THIS TYPE IS BEING SPAWNED
	relatedInstances.Empty();
	//loop through all the positions
	centerPos = pos;
	for (int i = 0; i < positions.Num();i++)
	{
		FVector iPos = positions[i] + pos;
		int index = map->AddIM(instanceType[i], iPos);
		relatedInstances.Add(index);
	}
}

void InstanceCustomMeshData::CreateICM(FVector pos, APartBuilder* partBuilder)
{
	//ONLY USE IF ONE ICM OF THIS TYPE IS BEING SPAWNED
	relatedInstances.Empty();
	//loop through all the positions
	centerPos = pos;
	for (int i = 0; i < positions.Num();i++)
	{
		FVector iPos = positions[i] + pos;
		int index = partBuilder->AddIM(instanceType[i], iPos);
		relatedInstances.Add(index);
	}
}

void InstanceCustomMeshData::CreateICM(FTransform pos, AChunk* chunk)
{
	////USED WHEN MORE THAN ONE ICM IS BEING SPAWNED
	GLog->Log("testy");
	ICMType* newBlock = new ICMType();
	//loop through all the positions
	FTransform ta;
	ta = pos;
	if (Centerpositions.Contains(pos.GetLocation()) == false)
	{
		centerPositions.Add(ta);
		Centerpositions.Add(pos.GetLocation());
		int blockID = blockStruct.meshType.Add(newBlock);
		GLog->Log("struckt Added index" + FString::FromInt(blockID));
		for (int i = 0; i < positions.Num();i++)
		{
			FTransform rt = pos;
			FVector iPos = pos.GetRotation().RotateVector(positions[i]) + pos.GetLocation();
			rt.SetLocation(iPos);
			/*FVector iPos = positions[i] + pos;*/
			int instance = FindInstanceType(instanceType[i], matIndex);
			int index = chunk->AddIM(instance, rt, this, blockID);

			newBlock->insatnceType.Add(instance);
			newBlock->relatedInstanceIndex.Add(index);
			newBlock->center = ta;
		//	float key = instance + (index * 0.00001);
		//	blocks.Add(key, meshTypeIndex);
		}
	}

	GLog->Log("struckt " + FString::FromInt(blockStruct.meshType.Num()));
}

void InstanceCustomMeshData::UpdateICM(FTransform pos, AMapController* map)
{
	centerPos = pos.GetLocation();
	//ONLY USE IF ONE ICM OF THIS TYPE IS BEING SPAWNED
	for (int i = 0; i < positions.Num();i++)
	{
		FTransform rt = pos;
		FVector iPos = pos.GetRotation().RotateVector(positions[i]) + pos.GetLocation();	
		rt.SetLocation(iPos);
		map->UpdateIm(instanceType[i], relatedInstances[i], rt);
	}
}

void InstanceCustomMeshData::RemoveICM(AMapController* map)
{
	//ONLY USE IF ONE ICM OF THIS TYPE IS BEING SPAWNED
	for (int i = 0; i < relatedInstances.Num();i++)
	{		
		map->RemoveIm(instanceType[i], relatedInstances[i]-i);
	}
	relatedInstances.Empty();
}

void InstanceCustomMeshData::RemoveICM(APartBuilder* partBuilder)
{
	//ONLY USE IF ONE ICM OF THIS TYPE IS BEING SPAWNED
	for (int i = 0; i < relatedInstances.Num();i++)
	{
		partBuilder->RemoveIm(instanceType[i], relatedInstances[i] - i);
	}
	relatedInstances.Empty();
}

void InstanceCustomMeshData::RemoveICM(AChunk* chunk, int blockID)
{
	//USED WHEN MORE THAN ONE ICM IS BEING SPAWNED
	//find the block ID

	GLog->Log("Vaild Key");
	//int structIndex = blocks.FindRef(key);
	GLog->Log(FString::FromInt(blockID));
	if (blockStruct.meshType.IsValidIndex(blockID))
	{
		GLog->Log(FString::FromInt(blockID));
		ICMType* currentBlock = blockStruct.meshType[blockID];
		if (currentBlock != nullptr)
		{
			TArray<int> changedInstances;
			//GLog->Log("Amount of instances " + FString::FromInt(currentBlock->insatnceType.Num()));
			for (int i = 0; i < currentBlock->insatnceType.Num();i++)
			{
				//GLog->Log("Looping");
				int ICMInstanceType = currentBlock->insatnceType[i];;
				chunk->RemoveIM(ICMInstanceType,chunk->ICMKeyTable[ICMInstanceType]->GetInstance(currentBlock->relatedInstanceIndex[i], blockID,this));	//won't be able to take i when more than one mesh type
				if(!changedInstances.Contains(ICMInstanceType)){ changedInstances.Add(ICMInstanceType); }	//if this instances type isn't in the array add it

			}
			GLog->Log("Done Loop");			
			//blocks.Remove(key);	//remove after getting the pos to avoid error				
			int cpi = Centerpositions.Find(currentBlock->center.GetLocation());
			GLog->Log("CPI" + FString::FromInt(cpi));
			centerPositions.RemoveAt(cpi/*,1,false*/);
			Centerpositions.RemoveAt(cpi/*, 1, false*/);
			currentBlock = nullptr;
			blockStruct.meshType.RemoveAt(blockID);
			for (int i = 0; i < changedInstances.Num();i++)
			{
				chunk->ICMKeyTable[changedInstances[i]]->UpdateBlockIDs(blockID,this);
			}
				
		}
	}
	GLog->Log("Not Vaild Key");
}

FTransform InstanceCustomMeshData::FindCenterPos(int blockID)
{
	//int structIndex = blocks.FindRef(key);
	if (blockStruct.meshType.IsValidIndex(blockID))
	{
		ICMType* currentBlock = blockStruct.meshType[blockID];
		if (currentBlock != nullptr)
		{
			return currentBlock->center;
		}
	}
	return FTransform();
}

int InstanceCustomMeshData::FindInstanceType(int meshType, int matType)
{
	//mesh type times amount of mats + mat type
	return ((meshType*3) + matType);
}


