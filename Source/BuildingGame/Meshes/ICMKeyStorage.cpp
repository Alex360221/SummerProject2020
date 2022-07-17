// Fill out your copyright notice in the Description page of Project Settings.


#include "ICMKeyStorage.h"

ICMKeyStorage::ICMKeyStorage()
{
}

void ICMKeyStorage::ICMKeyStorageAdd(int newInstance, int newSpawnInstances, int newBlockID, InstanceCustomMeshData* newICMD)
{
	instances.Add(newInstance);
	atSpawnInstances.Add(newSpawnInstances);
	blockIDs.Add(newBlockID);
	ICMData.Add(newICMD);
}

int ICMKeyStorage::GetInstance(int spawnInstance, int blockID, InstanceCustomMeshData* ICM)
{
	for (int i = 0; i < atSpawnInstances.Num();i++)
	{
		if (atSpawnInstances[i] == spawnInstance)
		{
			if (blockIDs[i] == blockID)
			{
				if (ICMData[i] == ICM)
				{
					return instances[i];
				}
			}
		}
	}
	return -1;
}

int ICMKeyStorage::GetBlockID(int instanceIndex)
{
	if (instances.Contains(instanceIndex))
	{
		return blockIDs[instances.Find(instanceIndex)];
	}
	return -1;
}

InstanceCustomMeshData* ICMKeyStorage::GetICMData(int instanceIndex)
{
	if (instances.Contains(instanceIndex))
	{
		return ICMData[instances.Find(instanceIndex)];
	}
	return nullptr;
}

void ICMKeyStorage::Remove(int instanceIndex)
{
	int index = instances.Find(instanceIndex);	//find instance place to remove that row
	instances.RemoveAt(index);
	atSpawnInstances.RemoveAt(index);
	blockIDs.RemoveAt(index);
	ICMData.RemoveAt(index);
	for (int i = index; i < instances.Num();i++)
	{
		//update instances
		instances[i] -= 1;
	}
}

void ICMKeyStorage::UpdateBlockIDs(int blockID, class InstanceCustomMeshData* ICM)
{
	for (int i = 0;i < blockIDs.Num();i++)
	{
		if (ICMData[i] == ICM)	//check this row belongs to this ICM
		{
			if (blockIDs[i] > blockID)
			{
				blockIDs[i] -= 1;	//takes one, updates the block ids
			}
		}		
	}
}


