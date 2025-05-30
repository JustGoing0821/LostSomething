// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "LSItemDataBase.h"
#include "Item.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API AItem : public AStaticMeshActor
{
	GENERATED_BODY()

public:

	AItem();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemName;

	// 메시와 아이콘은 에디터에서도 볼 수 있도록
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item")
	UStaticMeshComponent* MeshComponent;

	// 아이콘이 꼭 필요하다면 HUD용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UTexture2D* Icon;

	// FItemData를 기반으로 설정
	void InitFromData(const FItemData& Data);
};
