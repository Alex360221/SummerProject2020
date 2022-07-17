// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshStorage.h"

MeshStorage::MeshStorage()
{
}

MeshStorage::MeshStorage(TArray<FVector> newVertices, TArray<int> newTriangles)
{
	Vertices = newVertices;
	Triangles = newTriangles;
}

TArray<FVector> MeshStorage::GetMeshVertices()
{
	return Vertices;
}

TArray<int> MeshStorage::GetMeshTriangles()
{
	return Triangles;
}

