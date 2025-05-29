// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Item/Item.h"

AItem::AItem()
{
	MeshComponent = GetStaticMeshComponent(); // AStaticMeshActor에 기본적으로 있음
	SetMobility(EComponentMobility::Movable);
}

void AItem::InitFromData(const FItemData& Data)
{
	ItemName = FName("Default");
	if (Data.Mesh)
	{
		MeshComponent->SetStaticMesh(Data.Mesh);
	}
	Icon = Data.Icon;
}
