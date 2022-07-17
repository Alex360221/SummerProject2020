// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class BUILDINGGAME_API MeshStorage
{
public:
	MeshStorage();
	MeshStorage(TArray<FVector> newVertices, TArray<int> newTriangles);
    TArray<FVector> GetMeshVertices();  //this will return the vertices
    TArray<int> GetMeshTriangles();     //this will return the triangles
private:
    TArray<FVector> Vertices;   //this will store the vertices
    TArray<int> Triangles;      //this will store the triangles

};
