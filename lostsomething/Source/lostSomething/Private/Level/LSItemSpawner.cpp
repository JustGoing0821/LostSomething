// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/LSItemSpawner.h"
#include "lostSomething.h"
#include "Components/BoxComponent.h"
#include "Physics/LSCollisionProfile.h"
#include "Character/Item/MasterItem.h"

// Sets default values
ALSItemSpawner::ALSItemSpawner()
{
	//»ý¼ºÀÚ
	TriggerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerComponent"));
	RootComponent = TriggerComponent;
	TriggerComponent->SetBoxExtent(FVector(30, 30, 30));

	static ConstructorHelpers::FClassFinder<AMasterItem> CanItemRef(TEXT("/Game/Players/BluePrints/BP_Can.BP_Can_C"));
	if (CanItemRef.Succeeded())
	{
		ThrowItemArray.Add(CanItemRef.Class);
	}

	static ConstructorHelpers::FClassFinder<AMasterItem> CupItemRef(TEXT("/Game/Players/BluePrints/BP_Cup.BP_Cup_C"));
	if (CupItemRef.Succeeded())
	{
		ThrowItemArray.Add(CupItemRef.Class);
	}

	static ConstructorHelpers::FClassFinder<AMasterItem> AxeItemRef(TEXT("/Game/Players/BluePrints/BP_AxeWeapon.BP_AxeWeapon_C"));
	if (AxeItemRef.Succeeded())
	{
		WeaponItemArray.Add(AxeItemRef.Class);
	}

	static ConstructorHelpers::FClassFinder<AMasterItem> KnifeItemRef(TEXT("/Game/Players/BluePrints/BP_KnifeWeapon.BP_KnifeWeapon_C"));
	if (KnifeItemRef.Succeeded())
	{
		WeaponItemArray.Add(KnifeItemRef.Class);
	}

	bIsWeapon = false;
	SpawnTimer = 180.f;
}

// Called when the game starts or when spawned
void ALSItemSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		TriggerComponent->OnComponentEndOverlap.AddDynamic(this, &ALSItemSpawner::OnTriggerEndOverlap);
		SpawnItem();
	}
}


void ALSItemSpawner::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!HasAuthority()) return;
	AMasterItem* Item = Cast<AMasterItem>(OtherActor);
	if (Item)
	{
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, this, &ALSItemSpawner::SpawnItem, SpawnTimer, false);
	}
}

void ALSItemSpawner::SpawnItem()
{
	if (!HasAuthority()) return;

	int32 RandItem = FMath::RandRange(0, 1);

	if (bIsWeapon)
	{
		GetWorld()->SpawnActor<AMasterItem>(WeaponItemArray[RandItem], GetActorLocation() - FVector(0, 0, 10), GetActorRotation());
	}
	else
	{
		GetWorld()->SpawnActor<AMasterItem>(ThrowItemArray[RandItem], GetActorLocation() - FVector(0, 0, 30), GetActorRotation());
	}
	
}


