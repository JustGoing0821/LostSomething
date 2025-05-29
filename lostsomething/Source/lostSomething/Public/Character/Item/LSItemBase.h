// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/LSInteractionInterface.h"
#include "Interface/LSItemPickupInterface.h"
#include "LSItemBase.generated.h"

UCLASS()
class LOSTSOMETHING_API ALSItemBase : public AActor, public ILSItemPickupInterface
{
	GENERATED_BODY()
	
public:	
	
	ALSItemBase();
	virtual void OnPickup(class APlayerController* Controller) override;

	void OnPickedUp(AActor* Picker);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UStaticMeshComponent* MeshComponent;

	//아이템 이미지 변수 추가
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UTexture2D* ItemIcon;

protected:
	
public:	
	


};
