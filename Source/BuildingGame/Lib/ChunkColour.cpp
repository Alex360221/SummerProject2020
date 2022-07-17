// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkColour.h"
#include <BuildingGame/Chunk.h>

ChunkColour::ChunkColour()
{
}

//TArray<FColor> ChunkColour::CreateChunkColour(AChunk* chunkRef, float cellDistance, float cellAmount)
//{
//	CellAndVertex = chunkRef->GetCellAndVertexData();
//	this->CellDistance = cellDistance;
//	CellAmount = cellAmount;
//	//now we have got the data we need to work out the average height in each cell
//	FindAverageCellHeight();
//	FindCellGradient();
//	//now we have the gradient we can work out the cell and vertex colour
//	TArray<FVector> vertices;
//	CellAndVertex.GenerateKeyArray(vertices);
//	TArray<FColor> chunkColour;
//	for (int i = 0; i < CellAndVertex.Num();i++)
//	{
//		//this will loop through all the vertices
//		chunkColour.Add(FindVertexColour(FindCellColour(vertices[i])));
//	}
//	return chunkColour;
//}

void ChunkColour::FindAverageCellHeight()
{
	TArray<FVector> Cells;
	CellAndVertex.GenerateValueArray(Cells);
	for (int i = 0; i < CellAndVertex.Num();i++)
	{
		CellAverageVertex.Add(Cells[i],FindAverageCellHeight(FindAllVerticesWithinCell(Cells[i])));	//this works out the average cell height
	}

	return;
}

TArray<FVector> ChunkColour::FindAllVerticesWithinCell(FVector cell)
{
	TArray<FVector> vertices;
	TArray<FVector> keys;
	CellAndVertex.GenerateKeyArray(keys);
	for (int i = 0; i < CellAndVertex.Num();i++)	//loops through all the cells
	{
		if (CellAndVertex.FindRef(keys[i]) == cell)	//checks if it matches the cell
		{
			vertices.Add(keys[i]);	//if so add this vertex to to the returning array
		}
	}


	return vertices;
}

float ChunkColour::FindAverageCellHeight(TArray<FVector> vertices)
{
	float count = 0;
	for (int i = 0;i < vertices.Num();i++)	//loops through the current array
	{
		count += vertices[i].Z;	//adds them all up
	}
	count = count / vertices.Num();	//divides the count by the number in the array to the find the average
	return count;
}

void ChunkColour::FindCellGradient()
{
	//TArray<float> Gradients;
	TArray<FVector> cells;
	
	CellAverageVertex.GenerateKeyArray(cells);

	for (int i = 0; i < CellAverageVertex.Num();i++)
	{
		float TCA = CellAverageVertex.FindRef(cells[i]);	//TCA = this cells average
		CellAndGradient.Add(cells[i],CheckSurroundingCells(TCA, cells[i]));	//this will check the surrounding cells to and rturrn the gradient
	}
	return;
}

float ChunkColour::CheckSurroundingCells(float TCA, FVector cell)
{
	float Grad = 0;
	float cGrad = 0;

	//first check is behide Y
	cGrad = FindGradientBetweenCells(TCA, FVector(cell.X, cell.Y - CellDistance, cell.Z));
	if (cGrad != 0)
	{
		//its return something
		Grad += cGrad;
	}
	else
	{
		cGrad = FindGradientBetweenCells(TCA, FVector(cell.X, cell.Y - CellDistance, cell.Z - CellDistance));
		if (cGrad != 0) { Grad += cGrad; }
		else
		{
			cGrad = FindGradientBetweenCells(TCA, FVector(cell.X, cell.Y, cell.Z - CellDistance));
			if (cGrad != 0) { Grad += cGrad; }
			else
			{
				cGrad = FindGradientBetweenCells(TCA, FVector(cell.X, cell.Y + CellDistance, cell.Z - CellDistance));
				if (cGrad != 0) { Grad += cGrad; }
				else
				{
					if (CellOutsideChunkBounds(FVector(cell.X, cell.Y - CellDistance, cell.Z)))
					{
						//cell is edge
						//Grad += TCA;
					}
					else
					{
						//found nothing so the block is air
						Grad += CellDistance;
					}
				}
			}
		}
	}

	cGrad = FindGradientBetweenCells(TCA, FVector(cell.X, cell.Y + CellDistance, cell.Z));
	if (cGrad != 0)
	{
		//its return something
		Grad += cGrad;
	}
	else
	{
		cGrad = FindGradientBetweenCells(TCA, FVector(cell.X, cell.Y + CellDistance, cell.Z + CellDistance));
		if (cGrad != 0) { Grad += cGrad; }
		else
		{
			cGrad = FindGradientBetweenCells(TCA, FVector(cell.X, cell.Y, cell.Z + CellDistance));
			if (cGrad != 0) { Grad += cGrad; }
			else
			{
				cGrad = FindGradientBetweenCells(TCA, FVector(cell.X, cell.Y - CellDistance, cell.Z + CellDistance));
				if (cGrad != 0) { Grad += cGrad; }
				else
				{					
					if (CellOutsideChunkBounds(FVector(cell.X, cell.Y + CellDistance, cell.Z)))
					{
						//cell is edge
						//Grad += TCA;
					}
					else
					{
						//found nothing so the block is air
						Grad += CellDistance;
					}
				}
			}
		}
	}
	//now check is behide x
	cGrad = FindGradientBetweenCells(TCA, FVector(cell.X - CellDistance, cell.Y, cell.Z));
	if (cGrad != 0)
	{
		//its return something
		Grad += cGrad;
	}
	else
	{
		cGrad = FindGradientBetweenCells(TCA, FVector(cell.X - CellDistance, cell.Y, cell.Z - CellDistance));
		if (cGrad != 0) { Grad += cGrad; }
		else
		{
			cGrad = FindGradientBetweenCells(TCA, FVector(cell.X, cell.Y, cell.Z - CellDistance));
			if (cGrad != 0) { Grad += cGrad; }
			else
			{
				cGrad = FindGradientBetweenCells(TCA, FVector(cell.X + CellDistance, cell.Y, cell.Z - CellDistance));
				if (cGrad != 0) { Grad += cGrad; }
				else
				{
					if (CellOutsideChunkBounds(FVector(cell.X - CellDistance, cell.Y, cell.Z)))
					{
						//cell is edge
						//Grad += TCA;
					}
					else
					{
						//found nothing so the block is air
						Grad += CellDistance;
					}
				}
			}
		}
	}

	cGrad = FindGradientBetweenCells(TCA, FVector(cell.X + CellDistance, cell.Y, cell.Z));
	if (cGrad != 0)
	{
		//its return something
		Grad += cGrad;
	}
	else
	{
		cGrad = FindGradientBetweenCells(TCA, FVector(cell.X + CellDistance, cell.Y, cell.Z + CellDistance));
		if (cGrad != 0) { Grad += cGrad; }
		else
		{
			cGrad = FindGradientBetweenCells(TCA, FVector(cell.X, cell.Y, cell.Z + CellDistance));
			if (cGrad != 0) { Grad += cGrad; }
			else
			{
				cGrad = FindGradientBetweenCells(TCA, FVector(cell.X - CellDistance, cell.Y, cell.Z + CellDistance));
				if (cGrad != 0) { Grad += cGrad; }
				else
				{
					if (CellOutsideChunkBounds(FVector(cell.X + CellDistance, cell.Y, cell.Z)))
					{
						//cell is edge
						//Grad += TCA;
					}
					else
					{
						//found nothing so the block is air
						Grad += CellDistance;
					}
				}
			}
		}
	}
	
	return Grad;
}

float ChunkColour::FindGradientBetweenCells(float TCA, FVector newCell)
{
	float Grad = 0;
	//find average of new cell
	if (CellAverageVertex.Contains(newCell))
	{
		Grad = TCA - CellAverageVertex.FindRef(newCell);	//works out the gradient
	}
	if (Grad < 0) { Grad *= -1; }	//gets rid of negative
	return Grad;
}

FColor ChunkColour::FindVertexColour(float cellColour)
{
	if (cellColour < 140)
	{
		return FColor(0.0f, 0.0f, 0.0f);
	}
	else if (cellColour < 150)
	{
		return FColor(0.0f, 150.0f, 0.0f);
	}
	else
	{
		return FColor(0.0f, 0.0f, 255.f);
	}
	
}

float ChunkColour::FindCellColour(FVector vertex)
{
	//first find the cell that this vertex belongs to
	FVector cellPos = CellAndVertex.FindRef(vertex);
	return CellAndGradient.FindRef(cellPos);	//this returns the cellcolour
}

bool ChunkColour::CellOutsideChunkBounds(FVector cellPos)
{
	float ChunkSize = (CellAmount-1) * CellDistance;
	if (cellPos.X < 0 || cellPos.Y < 0 || cellPos.Z < 0
		|| cellPos.X > ChunkSize || cellPos.Y > ChunkSize)
	{
		return true;	//outside of chunk
	}
	return false;
}

