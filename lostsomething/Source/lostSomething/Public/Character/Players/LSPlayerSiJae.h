// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Players/LSPlayer.h"
#include "Character/Players/LSPlayerIJae.h"
#include "Blueprint/UserWidget.h"
#include "Character/UI/LSDarkWidget.h"
#include "LSPlayerSiJae.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALSPlayerSiJae : public ALSPlayer
{
	GENERATED_BODY()



	virtual void BeginPlay() override;
	//virtual void GetDistance()

	UPROPERTY()
	TObjectPtr<ALSPlayerIJae> IJae;


	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<ULSDarkWidget> DarkWidgetClass;

	UPROPERTY()
	ULSDarkWidget* DarkWidgetInstance;
	
public: 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	UStaticMeshComponent* Weapon;
	ALSPlayerSiJae();
	void WeaponPickUp();


	UFUNCTION(Server, Reliable)
	virtual void ServerWeaponPickUp();

	UFUNCTION(NetMulticast, Reliable)
	void MultiWeaponPickUp();

	UFUNCTION(Client, Reliable)
	void ClientWeaponPickUp();
	


protected:
	virtual bool CanPushWheelchair() const override;

	virtual void Attack() override;
	
	void Tick(float DeltaTime);

	

};