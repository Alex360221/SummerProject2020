// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class BUILDINGGAME_API DefaultMeshes
{
public:
	DefaultMeshes();
    TArray<FVector> GetVerticesSquareMesh(float size);    //this gets the triangles from the square mesh
    TArray<int> GetTrianglesSquareMesh();       //this gets the triangles from the square mesh
    
private:
    TArray<FVector> VerticesSquareMesh; //this stores the vertices for a square mesh
    TArray<int> TrianglesSquareMesh;    //this stores the trinalges for the square mesh

};
