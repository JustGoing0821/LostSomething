// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LSMapVersionControll.generated.h"

UCLASS()
class LOSTSOMETHING_API ALSMapVersionControll : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALSMapVersionControll();

	FORCEINLINE const int32 GetCurrentMapVersion() { return CurrentMapVersion; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category = "LSCustom")
	int32 CurrentMapVersion;

};
