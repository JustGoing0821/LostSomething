// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "LSScriptWidget.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ULSScriptWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	
public:
    UFUNCTION(BlueprintCallable, Category = "Script")
    void SetScriptText(const FString& NewText);

    UFUNCTION(BlueprintCallable, Category = "Script")
    void ShowScriptWidget();

    UFUNCTION(BlueprintCallable, Category = "Script")
    void HideScriptWidget();

protected:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* ScriptText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Script")
    float AutoHideDelay = 5.0f;

    FTimerHandle AutoHideTimerHandle;
};
