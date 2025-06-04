// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Item/MasterItem.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Character/Players/LSPlayer.h"
#include "Character/Players/LSPlayerController.h"

// Sets default values
AMasterItem::AMasterItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	RootComponent = ItemMesh;

	// 콜리전 설정
	CollisionComponent->SetSphereRadius(50.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	CollisionComponent->SetGenerateOverlapEvents(true);

	//부착
	CollisionComponent->SetupAttachment(ItemMesh);

	// 메시 콜리전 설정 
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ItemMesh->SetGenerateOverlapEvents(false);

	// 메시 기본 스케일 설정 (필요에 따라 조정)
	ItemMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));

}

// Called when the game starts or when spawned
void AMasterItem::BeginPlay()
{
	Super::BeginPlay();
	
	// 오버랩 이벤트 바인딩
	if (CollisionComponent)
	{
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AMasterItem::OnBeginOverlap);
		CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &AMasterItem::OnEndOverlap);

		UE_LOG(LogTemp, Warning, TEXT("MasterItem overlap events bound: %s"), *GetName());
	}

}




// Called every frame
void AMasterItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMasterItem::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("MasterItem: OnBeginOverlap with %s"), OtherActor ? *OtherActor->GetName() : TEXT("Unknown"));

	// LSPlayer인지 확인 (ThirdPersonCharacter 대신)
	if (ALSPlayer* Player = Cast<ALSPlayer>(OtherActor))
	{
		// PlayerController 가져오기
		if (ALSPlayerController* PlayerController = Cast<ALSPlayerController>(Player->GetController()))
		{
			// 입력 활성화
			this->EnableInput(PlayerController);

			UE_LOG(LogTemp, Warning, TEXT("Input enabled for player on item: %s"), *GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to get PlayerController from player"));
		}
	}
}

void AMasterItem::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("MasterItem: OnEndOverlap with %s"), OtherActor ? *OtherActor->GetName() : TEXT("Unknown"));

	// LSPlayer인지 확인
	if (ALSPlayer* Player = Cast<ALSPlayer>(OtherActor))
	{
		// PlayerController 가져오기
		if (ALSPlayerController* PlayerController = Cast<ALSPlayerController>(Player->GetController()))
		{
			// 입력 비활성화
			this->DisableInput(PlayerController);

			UE_LOG(LogTemp, Warning, TEXT("Input disabled for player on item: %s"), *GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to get PlayerController from player"));
		}
	}
}