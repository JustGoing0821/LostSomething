// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "LSItemStructures.h"
#include "MasterItem.generated.h"

UCLASS()
class AMasterItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMasterItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 메시 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ItemMesh;

	// 콜리전 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionComponent;

	//ItemInfo 변수 (Item Details 구조체)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	FItemDetails ItemInfo;

	// 오버랩 이벤트 : 오버랩 범위에 들어가면 input 활성화, 나가면 입력 비활성화
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// ItemInfo 접근자 함수들
	UFUNCTION(BlueprintCallable, Category = "Item Info")
	FItemDetails GetItemInfo() const { return ItemInfo; }

	UFUNCTION(BlueprintCallable, Category = "Item Info")
	void SetItemInfo(const FItemDetails& NewItemInfo) { ItemInfo = NewItemInfo; }

};
