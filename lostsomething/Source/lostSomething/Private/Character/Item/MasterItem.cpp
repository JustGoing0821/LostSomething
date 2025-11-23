// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Item/MasterItem.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Character/Players/LSPlayer.h"
#include "Engine/DamageEvents.h"
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


	bReplicates = true;

	//부착
	CollisionComponent->SetupAttachment(ItemMesh);

	// 메시 콜리전 설정 
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ItemMesh->SetGenerateOverlapEvents(false);

	// 메시 기본 스케일 설정 (필요에 따라 조정)
	ItemMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));

}

//무기 떨림 보정
//void AMasterItem::SetEquipped(bool bNewEquipped)
//{
//	bEquipped = bNewEquipped;
//
//	if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(GetRootComponent()))
//	{
//		// 손에 들고 있을 때 물리관련 비활성화
//		const bool bEnablePhysics = !bEquipped;
//
//		Prim->SetSimulatePhysics(bEnablePhysics);
//		Prim->SetEnableGravity(bEnablePhysics);
//		Prim->SetCollisionEnabled(
//			bEnablePhysics ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision
//		);
//	}
//
//
//	SetReplicateMovement(!bEquipped);
//}

// Called when the game starts or when spawned
void AMasterItem::BeginPlay()
{
	Super::BeginPlay();
	
	// 오버랩 이벤트 바인딩
	/*if (CollisionComponent)
	{
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AMasterItem::OnBeginOverlap);
		CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &AMasterItem::OnEndOverlap);

		UE_LOG(LogTemp, Warning, TEXT("MasterItem overlap events bound: %s"), *GetName());
	}*/

	//// 던진 아이템의 Hit 이벤트 바인딩
	//if (ItemMesh && bIsThrown)
	//{
	//	ItemMesh->OnComponentHit.AddDynamic(this, &AMasterItem::OnItemHit);
	//	UE_LOG(LogTemp, Warning, TEXT("Thrown item hit events bound: %s"), *GetName());
	//}

}




// Called every frame
void AMasterItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMasterItem::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("MasterItem: OnBeginOverlap with %s"), OtherActor ? *OtherActor->GetName() : TEXT("Unknown"));

	//// LSPlayer인지 확인
	//if (ALSPlayer* Player = Cast<ALSPlayer>(OtherActor))
	//{
	//	// PlayerController 가져오기
	//	if (ALSPlayerController* PlayerController = Cast<ALSPlayerController>(Player->GetController()))
	//	{
	//		// 입력 활성화
	//		this->EnableInput(PlayerController);

	//		UE_LOG(LogTemp, Warning, TEXT("Input enabled for player on item: %s"), *GetName());
	//	}
	//	else
	//	{
	//		UE_LOG(LogTemp, Error, TEXT("Failed to get PlayerController from player"));
	//	}
	//}
}

void AMasterItem::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//UE_LOG(LogTemp, Warning, TEXT("MasterItem: OnEndOverlap with %s"), OtherActor ? *OtherActor->GetName() : TEXT("Unknown"));

	//// LSPlayer인지 확인
	//if (ALSPlayer* Player = Cast<ALSPlayer>(OtherActor))
	//{
	//	// PlayerController 가져오기
	//	//if (ALSPlayerController* PlayerController = Cast<ALSPlayerController>(Player->GetController()))
	//	//{
	//	//	// 입력 비활성화
	//	//	this->DisableInput(PlayerController);

	//	//	UE_LOG(LogTemp, Warning, TEXT("Input disabled for player on item: %s"), *GetName());
	//	//}
	//	//else
	//	//{
	//	//	UE_LOG(LogTemp, Error, TEXT("Failed to get PlayerController from player"));
	//	//}
	//}
}

//onhit함수 item
void AMasterItem::OnItemHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse,
	const FHitResult& Hit)
{
	if (!bIsThrown) return; // 던져진 아이템이 아니면 무시
	UE_LOG(LogTemp, Warning, TEXT("Thrown item hit: %s"),
		OtherActor ? *OtherActor->GetName() : TEXT("Unknown"));

	// 바닥이나 벽에 맞으면 Hit 이벤트 해제
	if (!Cast<ILSTakeDamageInterface>(OtherActor) && !Cast<ALSPlayer>(OtherActor))
	{
		// Hit 이벤트 바인딩 해제
		if (ItemMesh)
		{
			ItemMesh->OnComponentHit.RemoveAll(this);
		}
		bIsThrown = false; // 던져진 상태 해제
		return;
	}

	// 플레이어나 다른 액터에게 맞은 경우
	if (ALSPlayer* HitPlayer = Cast<ALSPlayer>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("Item hit player: %s"), *HitPlayer->GetName());
		// 플레이어에게 맞았을 때
	}
	// NPC에게 데미지
	else if (ILSTakeDamageInterface* HitNPC = Cast<ILSTakeDamageInterface>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit NPC: %s, dealing damage: %.1f"),
			*OtherActor->GetName(), ThrowDamage);

		// 던진 캐릭터의 컨트롤러 가져오기
		AController* ThrowerController = GetInstigatorController();

		if (ThrowerController)
		{
			UE_LOG(LogTemp, Warning, TEXT("Thrower controller found: %s"),
				*ThrowerController->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Thrower controller is null!"));
		}

		// 데미지 이벤트 생성
		FDamageEvent DamageEvent;
		// NPC에게 데미지 적용 - 던진 캐릭터 컨트롤러 전달
		HitNPC->TakeDamage(ThrowDamage, DamageEvent, ThrowerController, this);

		// 던져진 상태 해제하고 아이템 파괴
		bIsThrown = false;
		Destroy();
	}
}
