// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/LobbyWidgetActor.h"
#include "UserInterface/Network/LobbyWidget.h"
#include "Components/WidgetComponent.h"  

// Sets default values
ALobbyWidgetActor::ALobbyWidgetActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	WidgetComponent->SetWidgetClass(ULobbyWidget::StaticClass());
	WidgetComponent->SetDrawSize(FVector2D(1920, 1080));
	WidgetComponent->SetTwoSided(true);

	WidgetComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WidgetComponent->SetCollisionResponseToAllChannels(ECR_Block);
}

// Called when the game starts or when spawned
void ALobbyWidgetActor::BeginPlay()
{
	Super::BeginPlay();

	if (WidgetClass)
	{
		// 위젯 클래스 설정
		WidgetComponent->SetWidgetClass(WidgetClass);

		// 위젯 인스턴스 생성 (자동으로 표시됨)
		UUserWidget* CreatedWidget = WidgetComponent->GetWidget();
		if (CreatedWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("Widget displayed successfully!"));
		}
	}
	
}

// Called every frame
void ALobbyWidgetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

