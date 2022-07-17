// Fill out your copyright notice in the Description page of Project Settings.


#include "UVMapping.h"
#include "Math/UnrealMathUtility.h"

UVMapping::UVMapping()
{
}

TArray<FVector2D> UVMapping::UVUnwrap(TArray<FVector> vertices, TArray<int> triangles, float size)
{
	//adjust the vertices so the smallest vaules is 0,0 (IF THIS WASN'T DONE THE UV MAP WOULD GO OUTSIDE THE UV AREA)
	FVector2D smallestPos = FVector2D::ZeroVector;
	for (int i = 0; i < vertices.Num();i++)
	{
		if (vertices[i].X < smallestPos.X) { smallestPos.X = vertices[i].X; }
		if (vertices[i].Y < smallestPos.Y) { smallestPos.Y = vertices[i].Y; }
	}
	//Now we have the smallest find how far below zero it is then add that vaule to all of the vertices
	smallestPos.X = 0 - smallestPos.X;
	smallestPos.Y = 0 - smallestPos.Y;

	TArray<FVector> UpdatedVertices;
	for (int i = 0; i < vertices.Num();i++)
	{
		FVector rv;
		rv.X = (vertices[i].X + smallestPos.X) / size;
		rv.Y = (vertices[i].Y + smallestPos.Y) / size;		//I don't think i need to divide
		rv.Z = vertices[i].Z / size;
		UpdatedVertices.Add(rv);
	}
	TArray<FVector2D> rva;
	
	/*for (int i = 0; i < vertices.Num();i++)
	{
		FVector2D rv;
		rv.X = (vertices[i].X + smallestPos.X) / size;
		rv.Y = (vertices[i].Y + smallestPos.Y) / size;
		rva.Add(rv);
	}*/
	//for (int i = 0; i < UpdatedVertices.Num();i++)
	//{
	//	FVector2D cf;
	//	cf.X = UpdatedVertices[i].X;
	//	cf.Y = UpdatedVertices[i].Y;
	//	rva.Add(cf);
	//}
	rva = UVUnwrapTri(UpdatedVertices, triangles);
	if (rva.Num() != vertices.Num())
	{
		rva.Add(FVector2D::ZeroVector);
	}
	return rva;
}

TArray<FVector2D> UVMapping::UVUnwrapTri(TArray<FVector> vertices, TArray<int> triangles)
{
	TArray<FVector2D> rv;
	TArray<FVector2D> square;
	bool clearSquare = false;
	FVector2D startPoint = FVector2D::ZeroVector;
	int squareAmount = (vertices.Num() / 4);	//4 vertices per square
	float UvDist = 0.3;
	int squareCount = 0;
	for (int i = 0; i < triangles.Num();)
	{
		
		FVector A = vertices[triangles[i]];
		FVector B = vertices[triangles[i + 1]];
		FVector C = vertices[triangles[i + 2]];

		if (AreAllPointsNegative(A, B, C) == true)
		{
			//all points are negative so adjust the face
			A.X += (A.Z * -2);
			B.X += (B.Z * -2);
			C.X += (C.Z * -2);
		}
		int G = AreTwoPointsTheSame(A, B, B);
		if (G > 0)
		{
			//two points are the same which means its vertical face
			//find out which one is negative
			A.X += A.Z;
			C.X += C.Z;
			//switch (G)
			//{
			//case 1:
			//	B.X += B.Z;
			//	break;
			//case 2:
			//	A.X += A.Z;
			//	break;
			//case 3:
			//	C.X += C.Z;
			//	break;
			//}
		}
		FVector2D cf;
		cf.X = A.X;
		cf.Y = A.Y;
		if(square.Contains(cf) == false){ square.Add(cf); }
		cf.X = B.X;
		cf.Y = B.Y;
		if (square.Contains(cf) == false) { square.Add(cf); }
		cf.X = C.X;
		cf.Y = C.Y;
		if (square.Contains(cf) == false) { square.Add(cf); }
		i += 3;
		if (clearSquare == true)
		{
			//clear the square	
			if (squareCount == squareAmount / 2)
			{
				startPoint = FVector2D(UvDist, 0);
			}
			rv.Add(startPoint);
			rv.Add(FVector2D(startPoint.X + UvDist, startPoint.Y));
			rv.Add(FVector2D(startPoint.X + UvDist, startPoint.Y + UvDist));
			rv.Add(FVector2D(startPoint.X, startPoint.Y + UvDist));
			startPoint = FVector2D(startPoint.X, startPoint.Y + UvDist);

			square.Empty();
			clearSquare = false;
			squareCount++;
		}
		else
		{
			clearSquare = true;
		}
	}
	return rv;
}

bool UVMapping::AreAllPointsNegative(FVector A, FVector B, FVector C)
{
	if (A.Z < 0 && B.Z < 0 && C.Z < 0)
	{
		//they all less than zero so this is a bottom face
		return true;
	}
	return false;
}

int UVMapping::AreTwoPointsTheSame(FVector A, FVector B, FVector C)
{
	FVector2D a = FVector2D(A.X, A.Y);
	FVector2D b = FVector2D(B.X, B.Y);
	FVector2D c = FVector2D(C.X, C.Y);
	if (a == b) { return 1; }
	if (a == c) { return 2; }
	if (b == c) { return 3; }
	return 0;
}

