// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class BUILDINGGAME_API ChunkColour
{
public:
	ChunkColour();
    //TArray<FColor> CreateChunkColour(class AChunk* chunkRef, float cellDistance, float cellAmount);   //this will work out the colour of the chunk returning a colour array
private:
    TMap<FVector, FVector> CellAndVertex;   //this will store the cell and the vert data
    TMap<FVector, float> CellAverageVertex; //this will store the average vertex height
    TMap<FVector, float> CellAndGradient;   //this will store the cell graident which will be used to dertmine vertex colour
    TArray<float> Gradients;    //this will store the gradient betweens cells used to dertimne the colour
    float CellDistance;
    float CellAmount;
    //functions
    void FindAverageCellHeight();   //this will find the average cell height
    TArray<FVector> FindAllVerticesWithinCell(FVector cell);    //this will find all the vertices iwthin this cell and return an array with all the vertices in
    float FindAverageCellHeight(TArray<FVector> vertices);  //this will find an average height in the cell
    void FindCellGradient(); //this will fidn the gradient between the cells and return it
    float CheckSurroundingCells(float TCA, FVector cell);  //this will check the surrounding cells
    float FindGradientBetweenCells(float TCA, FVector newCell);  //this will work out the gradient between two cells 
    FColor FindVertexColour(float cellColour);    //this will convert then gradient to the colour
    float FindCellColour(FVector vertex);   //this will find the cell that this vertex belongs to then returns cell colour vaule
    bool CellOutsideChunkBounds(FVector cellPos);   //this will work out if the cell is outside the chunk
};
