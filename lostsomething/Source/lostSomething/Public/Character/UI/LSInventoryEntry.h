// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "LSInventoryEntry.generated.h"


class UTextBlock;
class UButton;


UCLASS()
class LOSTSOMETHING_API ULSInventoryEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct();
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemName;

	UPROPERTY(meta = (BindWidget))
	UButton* DropButton;

	UFUNCTION()
	void DropItem();

	void ClearButton();
};
