// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LSScriptTester.generated.h"

UCLASS()
class LOSTSOMETHING_API ALSScriptTester : public AActor
{
    GENERATED_BODY()

    public:
    // Sets default values for this actor's properties
    ALSScriptTester();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    // 오버랩 이벤트 처리
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

protected:
    // 콜리전 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UBoxComponent* CollisionBox;

    // 메시 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* MeshComponent;

    // 위젯 클래스
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<class UUserWidget> ScriptWidgetClass;

    // 위젯 인스턴스
    UPROPERTY()
    class ULSScriptWidget* ScriptWidget;

    // 표시할 스크립트 텍스트
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Script")
    FString ScriptText = TEXT("이것은 테스트 스크립트입니다.");
};