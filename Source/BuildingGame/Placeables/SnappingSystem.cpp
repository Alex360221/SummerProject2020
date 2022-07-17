// Fill out your copyright notice in the Description page of Project Settings.


#include "SnappingSystem.h"
#include "DrawDebugHelpers.h"
#include <BuildingGame/MapController.h>
#include <BuildingGame/Chunk.h>
#include <BuildingGame/MeshData/SnappingPointsData.h>

SnappingSystem::SnappingSystem()
{
}

FVector SnappingSystem::FindSnappingPoint(FVector location, FVector meshLocation, FRotator rotation, int meshIndex, AMapController* mapConRef)
{
	snappedPos = FVector::ZeroVector;
	mapControllerRef = mapConRef;
	meshActorLocation = meshLocation;
	meshRotation = rotation;

	if (mapConRef->CheckMeshIndexIsFaild(meshIndex) == true)
	{
		GLog->Log("mesh index -----" + FString::FromInt(meshIndex));
		//sets the deafult snapping points data
		snapPointsDef = mapControllerRef->GetSnappingPoints(meshIndex);
		snapWidthDef = mapControllerRef->GetSnappingPointsWidth(meshIndex);
		snapDirDef = mapControllerRef->GetSnappingDirection(meshIndex);
		//sets the locaal data which will be chnaged
		TArray<FVector> snapPoints = snapPointsDef;
		TArray<float> snapWidth = snapWidthDef;
		TArray<FVector> snapDir = snapDirDef;
		
		
		//FTransform meshTran;
		//currentMesh->GetInstanceTransform(currentInstanceMeshIndex, meshTran);
		GLog->Log("Current Location X: " + FString::FromInt(location.X) + " ,Y: " + FString::FromInt(location.Y) + " ,Z: " + FString::FromInt(location.Z));
		GLog->Log("Current Location mesh actor X: " + FString::FromInt(meshActorLocation.X) + " ,Y: " + FString::FromInt(meshActorLocation.Y) + " ,Z: " + FString::FromInt(meshActorLocation.Z));
		GLog->Log(FString::FromInt(snapPoints.Num()));
		//now loop through snapping points
		for (int i = 0; i < snapPoints.Num();i++)
		{
			const FRotator rot = rotation;

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
			end += meshActorLocation;
			start= rot.RotateVector(start);
			start+= meshActorLocation;
			world = mapControllerRef->GetWorld();
			GLog->Log("After rotation Snapping Direction!!!! X: " + FString::FromInt(end.X) + " ,Y: " + FString::FromInt(end.Y) + " ,Z: " + FString::FromInt(end.Z));
			if (SnappingPointFound(start, end) == true)
			{
				UInstancedStaticMeshComponent* mesh = Cast<UInstancedStaticMeshComponent>(outHit.GetComponent());
				if (mesh != nullptr)
				{
					FVector test = NewSnappedPos(mesh, snapPoints[i]);
					//GLog->Log("Current Location X: " + FString::FromInt(location.X) + " ,Y: " + FString::FromInt(location.Y) + " ,Z: " + FString::FromInt(location.Z));
					//GLog->Log("Snapped Location: " + FString::FromInt(test.X) + " ,Y: " + FString::FromInt(test.Y) + " ,Z: " + FString::FromInt(test.Z));
					return test;
				}
			}
			else
			{
				//GLog->Log("found no snapping point");
			}
		}
	}

	return location;
}

bool SnappingSystem::SnappingPointFound(FVector start, FVector end)
{
	GLog->Log("Find Snapping each Point");
	//line trace
	if (world != nullptr)
	{
		FCollisionQueryParams collisionsParms;

		bool isHit = world->LineTraceSingleByChannel(outHit, start, end, ECC_Visibility, collisionsParms);
		//DrawDebugLine(world, start, end, FColor::Red, true);
		if (isHit)
		{
			GLog->Log("HitSomething");
			UInstancedStaticMeshComponent* mesh = Cast<UInstancedStaticMeshComponent>(outHit.GetComponent());
			if (mesh != nullptr)
			{
				GLog->Log("Found mesh");
				//is within other Snapping point
				if (WithinSnappingPoint(mesh, outHit.ImpactPoint) == true)
				{
					//find new location of this object
					return true;
				}
			}
		}
	}	
	return false;
}

bool SnappingSystem::WithinSnappingPoint(UInstancedStaticMeshComponent* otherMesh, FVector hitPos)
{
	//FTransform meshtran;
	//otherMesh->GetInstanceTransform(outHit.Item, meshtran);
	AChunk* chunkRef = Cast<AChunk>(outHit.GetActor());
	if (chunkRef == nullptr) { return false; }
	//got vaild chunk ref
	FTransform meshtran = chunkRef->ICM_GetCenterPos(otherMesh, outHit.Item);
	FVector otherMeshPos = meshtran.GetLocation();
	
	/*GLog->Log("X: " + FString::FromInt(otherMeshPos.X) + " ,Y: " + FString::FromInt(otherMeshPos.Y) + " ,Z: " + FString::FromInt(otherMeshPos.Z));
	GLog->Log("hit point X: " + FString::FromInt(hitPos.X) + " ,Y: " + FString::FromInt(hitPos.Y) + " ,Z: " + FString::FromInt(hitPos.Z));*/
	/*int meshIndex = mapControllerRef->GetStaticMeshIndex(otherMesh->GetStaticMesh());
	if (mapControllerRef->CheckMeshIndexIsFaild(meshIndex) == true)
	{*/
		//found all snapping points to do with that mesh
		TArray<FVector> snapPoints;// = snapPointsDef;//i think this should get the snapping points of the other mesh
		TArray<float> snapWidth;// = snapWidthDef;
		int otherMeshType = chunkRef->ICM_GetID(otherMesh, outHit.Item);
		if (otherMeshType == -1) { return false; }
		SnappingPointsData* snappingRef = mapControllerRef->ICM_GetSnappingPointsRef(otherMeshType);
		if (snappingRef != nullptr)
		{
			//GLog->Log("good!!");
			snapPoints = snappingRef->GetSnappingPoints();
			snapWidth = snappingRef->GetSnappingPointsWidth();
		}
		else
		{
			//GLog->Log("bad!!");
			snapPoints = snapPointsDef;
			snapWidth = snapWidthDef;
		}
		//check if the the impact point hits within any of these areas!
		const FRotator rot = meshtran.GetRotation().Rotator();//meshRotation;
		for (int i = 0; i < snapPoints.Num();i++)
		{
			FVector upperP = rot.RotateVector(snapPoints[i] + snapWidth[i]) + otherMeshPos;
			FVector lowerP = rot.RotateVector(snapPoints[i] - snapWidth[i]) + otherMeshPos;

			//GLog->Log("Before change upperP: " + FString::FromInt(upperP.X) + " ,Y: " + FString::FromInt(upperP.Y) + " ,Z: " + FString::FromInt(upperP.Z));
			//GLog->Log("Before change lowerP: " + FString::FromInt(lowerP.X) + " ,Y: " + FString::FromInt(lowerP.Y) + " ,Z: " + FString::FromInt(lowerP.Z));

			//GLog->Log("HitPoint x:" + FString::FromInt(hitPos.X) + " ,Y: " + FString::FromInt(hitPos.Y) + " ,Z: " + FString::FromInt(hitPos.Z));

			if (lowerP.X > upperP.X)
			{
				//GLog->Log("Xlow");
				float a = upperP.X;
				upperP.X = lowerP.X;
				lowerP.X = a;
			}
			if (lowerP.Y > upperP.Y)
			{
				//GLog->Log("Ylow");
				float a = upperP.Y;
				upperP.Y = lowerP.Y;
				lowerP.Y = a;
			}
			if (lowerP.Z > upperP.Z)
			{
				//GLog->Log("Zlow");
				float a = upperP.Z;
				upperP.Z = lowerP.Z;
				lowerP.Z = a;
			}

			if (int(upperP.X) == int(lowerP.X))
			{
				//GLog->Log("X same");
				upperP.X += snapWidth[i];
				lowerP.X -= snapWidth[i];
			}
			if (int(upperP.Y) == int(lowerP.Y))
			{
				//GLog->Log("Y same");
				upperP.Y += snapWidth[i];
				lowerP.Y -= snapWidth[i];
			}
			if (int(upperP.Z) == int(lowerP.Z))
			{
				//GLog->Log("Z same");
				upperP.Z += snapWidth[i];
				lowerP.Z -= snapWidth[i];
			}
			//GLog->Log("After upperP: " + FString::FromInt(upperP.X) + " ,Y: " + FString::FromInt(upperP.Y) + " ,Z: " + FString::FromInt(upperP.Z));
			//GLog->Log("After lowerP: " + FString::FromInt(lowerP.X) + " ,Y: " + FString::FromInt(lowerP.Y) + " ,Z: " + FString::FromInt(lowerP.Z));

			if ((hitPos.X > lowerP.X&& hitPos.X < upperP.X)
				&& (hitPos.Y > lowerP.Y&& hitPos.Y < upperP.Y)
				&& (hitPos.Z > lowerP.Z&& hitPos.Z < upperP.Z))
			{
				snappedPos = rot.RotateVector(snapPoints[i]);
				GLog->Log("Within Snapping Zone!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
				return true;
			}
			GLog->Log("Not in area");
		//}
	}
	return false;
}

FVector SnappingSystem::NewSnappedPos(UInstancedStaticMeshComponent* otherMesh, FVector currentSnapPos)
{
	//find other mesh pos
	AChunk* chunkRef = Cast<AChunk>(outHit.GetActor());
	if (chunkRef == nullptr) { return currentSnapPos; }
	//got vaild chunk ref
	FTransform meshtran = chunkRef->ICM_GetCenterPos(otherMesh, outHit.Item);
	FVector otherMeshPos = meshtran.GetLocation();
	//FTransform meshtran;
	const FRotator rot = meshRotation;
	//otherMesh->GetInstanceTransform(outHit.Item, meshtran);
	//FVector otherMeshPos = meshtran.GetLocation();
	FVector otherEdge = snappedPos + otherMeshPos;
	//currentMesh->GetInstanceTransform(currentInstanceMeshIndex, meshtran);
	FVector currentMeshPos = meshActorLocation;
	FVector currentEdge = rot.RotateVector(currentSnapPos)/* + currentMeshPos*/;
	FVector SnapPos;
	if (snappedPos.X < 0) { SnapPos = otherEdge + currentEdge; }
	else { SnapPos = otherEdge - currentEdge; }
	if (snappedPos.Y < 0) { SnapPos = otherEdge + currentEdge; }
	else { SnapPos = otherEdge - currentEdge; }
	if (snappedPos.Z < 0) { SnapPos = otherEdge + currentEdge; }
	else { SnapPos = otherEdge - currentEdge; }
	//GLog->Log("new Mesh pos!!");
	//GLog->Log("Active pos X: " + FString::FromInt(currentMeshPos.X) + " ,Y: " + FString::FromInt(currentMeshPos.Y) + " ,Z: " + FString::FromInt(currentMeshPos.Z));
	//GLog->Log("Other Edge X: " + FString::FromInt(otherEdge.X) + " ,Y: " + FString::FromInt(otherEdge.Y) + " ,Z: " + FString::FromInt(otherEdge.Z));
	//GLog->Log("hit point X: " + FString::FromInt(outHit.ImpactPoint.X) + " ,Y: " + FString::FromInt(outHit.ImpactPoint.Y) + " ,Z: " + FString::FromInt(outHit.ImpactPoint.Z));

	return SnapPos;
}


