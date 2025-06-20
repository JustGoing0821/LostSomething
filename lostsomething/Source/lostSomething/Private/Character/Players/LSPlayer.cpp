// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Players/LSPlayer.h"
#include "lostSomething.h"
#include "InputMappingContext.h"
#include "Interface/LSInteractionInterface.h"
#include "Components/CapsuleComponent.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interface/LSTakeDamageInterface.h"
#include "EnhancedInputComponent.h"
#include "Character/Players/LSPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Character/Item/MasterItem.h"
#include "Components/ArrowComponent.h"
#include "Character/UI/LSHUDWidget.h"
#include "Character/Components/LSHpComponent.h"


// Sets default values
ALSPlayer::ALSPlayer()
{

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//컨트롤러 회전시 회전 x
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;


	//Capusule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	//Movement
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate
	//GetCharacterMovement()->bUseControllerDesiredRotation = true;

	//camera boom 
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	
	//hp
	HpComponent = CreateDefaultSubobject<ULSHpComponent>(TEXT("HpComponent"));
	CurrentHp = 100.0f;

	//item 방향 arrow
	DropItemLoc = CreateDefaultSubobject<UArrowComponent>(TEXT("DropItemLoc"));
	DropItemLoc->SetupAttachment(RootComponent);
	// Arrow 위치 조정 (플레이어 앞쪽에 배치)
	DropItemLoc->SetRelativeLocation(FVector(100.0f, 0.0f, 0.0f));

	//wheelchair
	WheelchairMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WheelchairMesh"));
	WheelchairMesh->SetupAttachment(GetMesh());

	bIsBeingPushed = false;
	PusherCharacter = nullptr;
	bCanPushWheelchair = false;

}

void ALSPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
}

// Called when the game starts or when spawned
void ALSPlayer::BeginPlay()
{
	Super::BeginPlay();

	if (HpComponent)
	{
		// HpComponent의 OnHpChanged 델리게이트에 우리의 OnHpChanged 함수를 바인딩
		HpComponent->OnHpChanged.AddDynamic(this, &ALSPlayer::OnHpChanged);
		UE_LOG(LogTemp, Warning, TEXT("OnHpChanged delegate bound success : LSPlayer"));
		
		HpComponent->OnHpZero.AddDynamic(this, &ALSPlayer::OnHpReachedZero);
		UE_LOG(LogTemp, Warning, TEXT("HPzeero delegates bound success : LSPlayer"));

		// 현재 체력값으로 HUD 초기화
		OnHpChanged(HpComponent->GetHp());


	}

	//인벤토리 초기화
	InitializeInventory();

}

// Called every frame
void ALSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 이제(IJae) 캐릭터가 밀리고 있는 상태라면 이동 처리
	if (bIsBeingPushed && PusherCharacter)
	{
		HandleWheelchairMovement();
	}
}

void ALSPlayer::OnRep_CurrentHp()
{
	LS_LOG(LogLS, Log, TEXT("CurrentHp : %f"), CurrentHp);
	HpComponent->SetHp(CurrentHp);
}

float ALSPlayer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	LS_LOG(LogLS, Log, TEXT("Begin : %f"), DamageAmount);


	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	//추가 코드 (feat.슬)
	if (HasAuthority())
	{
		ApplyDamage(DamageAmount);
		LS_LOG(LogLS, Log, TEXT("%s"), TEXT("ApplyDamage called"));
	}
	else
	{
		LS_LOG(LogLS, Error, TEXT("%s"), TEXT("This is Client Character"));
	}

	return DamageAmount;


	//기존 코드
	if (HpComponent)
	{
		const float NewHp = HpComponent->GetHp() - DamageAmount;
		//HpComponent->SetHp(NewHp);
		CurrentHp -= DamageAmount;
		if (HasAuthority())
		{
			HpComponent->SetHp(CurrentHp);
			CurrentHp = HpComponent->GetHp();  //0 이하로 내려가지 않게 동기화

		}


		//UI 업데이트 시도
		UE_LOG(LogTemp, Warning, TEXT("TakeDamage: Attempting direct HUD update"));

		// 손상을 입힌 컨트롤러가 있는 경우 확인
		AController* ValidController = EventInstigator;
		if (!ValidController)
		{
			// 없으면 현재 월드의 첫 번째 플레이어 컨트롤러 가져오기
			if (GetWorld())
			{
				for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
				{
					ValidController = It->Get();
					if (ValidController)
						break;
				}
			}
		}

		if (ValidController)
		{
			ALSPlayerController* LSController = Cast<ALSPlayerController>(ValidController);
			if (LSController && LSController->GetLSHUDWidget())
			{
				UE_LOG(LogTemp, Warning, TEXT("Directly updating HUD with HP: %.1f"), NewHp);
				LSController->GetLSHUDWidget()->UpdateHpBar(NewHp);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Could not get LSPlayerController or HUDWidget"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("No valid controller found for HUD update"));
		}
	}

	return DamageAmount;
}

void ALSPlayer::ApplyDamage(float DamageAmount)
{
	LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	CurrentHp -= DamageAmount;

	if (CurrentHp < 0)
	{
		return;
	}

	if (HpComponent)
	{
		HpComponent->SetHp(CurrentHp);
		LS_LOG(LogLS, Log, TEXT("SetHp Called"));
	}
	else
	{
		LS_LOG(LogLS, Error, TEXT("No HpComponent"));
	}
}

bool ALSPlayer::isCombining()
{
	return bIsBeingPushed;
}


void ALSPlayer::OnHpChanged(float NewHp)
{
	//UE_LOG(LogTemp, Warning, TEXT("ALSPlayer::OnHpChanged called with HP: %.1f"), NewHp);

	// 플레이어 컨트롤러 가져오기
	ALSPlayerController* LSController = Cast<ALSPlayerController>(GetController());
	if (LSController && LSController->GetLSHUDWidget())
	{
		// HUD 위젯의 HP 바 업데이트
		LSController->GetLSHUDWidget()->UpdateHpBar(NewHp);
		UE_LOG(LogTemp, Warning, TEXT("ALSPlayer :: Updated HUD with new HP: %.1f"), NewHp);
	}
	else
	{
		if (!LSController)
			UE_LOG(LogTemp, Error, TEXT("LSPLAYER :: LSPlayerController is null"));

	}
}

//아이템 픽업함수
//void ALSPlayer::PickItem(const FItemDetails& PickedItemInfo)
//{
//	// Pick Item In Slot 함수 호출
//	//PickItemInSlot(PickedItemInfo);
//
//}

//슬롯에 아이템 넣기
void ALSPlayer::PickItemInSlot(const FItemDetails& PickedItem)
{
	UE_LOG(LogTemp, Warning, TEXT("ALSPlayer::PickItemInSlot() called"));

	if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetController()))
	{
		if (ULSHUDWidget* HUD = PC->GetLSHUDWidget())
		{
			int32 CurrentSelectedSlot = SelectedSlot;

			//슬롯 번호 유효한지 체크
			if (CurrentSelectedSlot >= 0 && CurrentSelectedSlot < ItemInfoArray.Num())
			{
				//슬롯 배열에서 아이템 정ㅇ보 복사해오기
				FItemDetails CurrentSlotItem = ItemInfoArray[CurrentSelectedSlot];
				bool bCurrentSlotIsEmpty = CurrentSlotItem.IsEmpty;

				if (bCurrentSlotIsEmpty)  // 빈 슬롯인 경우에만 픽업
				{
					// 새 아이템 저장
					ItemInfoArray[CurrentSelectedSlot] = PickedItem;

					// 아이콘 업데이트
					UTexture2D* ItemIcon = PickedItem.Item_Icon.LoadSynchronous();
					HUD->SetIcon(CurrentSelectedSlot, ItemIcon);

					UE_LOG(LogTemp, Warning, TEXT("Item stored in slot %d"), CurrentSelectedSlot);
				}
				else  // 이미 차있는 슬롯인 경우
				{
					UE_LOG(LogTemp, Warning, TEXT("Slot %d is occupied - dropping existing item. Try picking up again."), CurrentSelectedSlot);

					// 기존 아이템만 드롭하고 끝 (새 아이템은 픽업하지 않음)
					DropItemFromSlot();
				}
			}
		}
		
	}

	
}


//아이템 drop
void ALSPlayer::DropItemFromSlot()
{
	UE_LOG(LogTemp, Warning, TEXT("ALSPlayer::DropItemFromSlot() called"));

	// PlayerController를 통해 HUD에서 현재 선택된 슬롯 인덱스 가져오기
	if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetController()))
	{
		if (ULSHUDWidget* HUD = PC->GetLSHUDWidget())
		{
			// 블루프린트의 SelectedSlots 변수와 동일한 값
			int32 CurrentSelectedSlot = SelectedSlot;

			// ItemInfoArray GET: CurrentSelectedSlot 인덱스로 현재 슬롯 아이템 가져오기
			if (CurrentSelectedSlot >= 0 && CurrentSelectedSlot < ItemInfoArray.Num())
			{
				FItemDetails CurrentSlotItem = ItemInfoArray[CurrentSelectedSlot];

				// Break ItemDetails: IsEmpty와 Item Class 값 추출
				bool bIsSlotEmpty = CurrentSlotItem.IsEmpty;
				TSubclassOf<AMasterItem> ItemClass = CurrentSlotItem.Item_Class;

				// Branch: IsEmpty 조건 확인 (False 분기 - 슬롯이 차있을 때)
				if (!bIsSlotEmpty)  // False 분기
				{
					// Spawn Actor: DropItemLoc 위치에 Item Class로 액터 생성
					if (ItemClass && DropItemLoc)
					{
						// Get World Location: DropItemLoc Arrow의 월드 위치 가져오기
						FVector SpawnLocation = DropItemLoc->GetComponentLocation();
						FRotator SpawnRotation = DropItemLoc->GetComponentRotation();

						FActorSpawnParameters SpawnParams;
						SpawnParams.Instigator = this;

						AMasterItem* SpawnedItem = GetWorld()->SpawnActor<AMasterItem>(
							ItemClass,
							SpawnLocation,
							SpawnRotation,
							SpawnParams
						);

						if (SpawnedItem)
						{
							UE_LOG(LogTemp, Warning, TEXT("Item dropped: %s"), *SpawnedItem->GetName());
						}
					}

					// Set Array Element: 해당 슬롯을 빈 상태로 만들기
					FItemDetails EmptySlot;
					EmptySlot.IsEmpty = true;
					EmptySlot.Item_Name = NAME_None;
					EmptySlot.Item_Icon = nullptr;
					EmptySlot.Item_Nature = EItemNature::IsConsumable;
					EmptySlot.Item_Class = nullptr;

					ItemInfoArray[CurrentSelectedSlot] = EmptySlot;

					// Set Icon: HUD의 아이콘도 비우기 (Empty Item의 아이콘 = nullptr)
					UTexture2D* EmptyIcon = nullptr; // Empty Item의 Item Icon
					HUD->SetIcon(CurrentSelectedSlot, EmptyIcon);

					UE_LOG(LogTemp, Warning, TEXT("Slot %d cleared, item dropped, and icon updated"), CurrentSelectedSlot);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Slot %d is already empty - nothing to drop"), CurrentSelectedSlot);
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Invalid slot index: %d"), CurrentSelectedSlot);
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("HUD Widget not found"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController not found"));
	}
}

//인벤토리 초기화
void ALSPlayer::InitializeInventory()
{
	UE_LOG(LogTemp, Warning, TEXT("ALSPlayer::InitializeInventory() called"));

	// Empty Item 생성
	FItemDetails EmptyItem;
	EmptyItem.Item_Name = NAME_None;
	EmptyItem.Item_Icon = nullptr;
	EmptyItem.Item_Nature = EItemNature::IsConsumable;
	EmptyItem.IsEmpty = true;
	EmptyItem.Item_Class = nullptr;

	// ItemInfoArray를 Empty Item으로 채움
	MaxSlots = 5; 
	ItemInfoArray.Empty(); //배열 비우기

	for (int32 i = 0; i < MaxSlots; ++i)
	{
		ItemInfoArray.Add(EmptyItem);
	}

	UE_LOG(LogTemp, Warning, TEXT("Inventory initialized with %d empty slots"), MaxSlots);



	// 초기화 후 모든 슬롯에 기본 아이콘 설정
	if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetController()))
	{
		if (ULSHUDWidget* HUD = PC->GetLSHUDWidget())
		{
			for (int32 i = 0; i < MaxSlots; ++i)
			{
				HUD->SetIcon(i, nullptr); // null을 전달하면 기본 아이콘 표시
			}
		}
	}

}

void ALSPlayer::ThrowItem()
{
	
	if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetController()))
	{
		if (ULSHUDWidget* HUD = PC->GetLSHUDWidget())
		{
			int32 CurrentSelectedSlot = SelectedSlot;
			if (CurrentSelectedSlot >= 0 && CurrentSelectedSlot < ItemInfoArray.Num())
			{
				FItemDetails CurrentSlotItem = ItemInfoArray[CurrentSelectedSlot];
				if (!CurrentSlotItem.IsEmpty)
				{
					// 포물선으로 아이템 스폰
					SpawnThrowableItem(CurrentSlotItem);

					// 슬롯 비우기 (DropItemFromSlot과 동일)
					FItemDetails EmptySlot;
					EmptySlot.IsEmpty = true;
					ItemInfoArray[CurrentSelectedSlot] = EmptySlot;

					// UI 아이콘 제거
					HUD->SetIcon(CurrentSelectedSlot, nullptr);

					UE_LOG(LogTemp, Warning, TEXT("Item thrown from slot %d"), CurrentSelectedSlot);
				}
			}
		}
	}
}

// Called to bind functionality to input
void ALSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (bIsDead) return;

	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	//action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {


		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALSPlayer::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ALSPlayer::Look);

		// Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ALSPlayer::Interaction);

		//Attack
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ALSPlayer::Attack);
	
		//Mouse
		//EnhancedInputComponent->BindAction(MouseWheelUpAction, ETriggerEvent::Triggered, this, &ALSPlayer::OnMouseWheelUp);
		//EnhancedInputComponent->BindAction(MouseWheelDownAction, ETriggerEvent::Triggered, this, &ALSPlayer::OnMouseWheelDown);

		//Pickup
		EnhancedInputComponent->BindAction(PickUpAction, ETriggerEvent::Triggered, this, &ALSPlayer::PickUp);

		// 숫자키 바인딩
		EnhancedInputComponent->BindAction(SelectSlot1Action, ETriggerEvent::Triggered, this, &ALSPlayer::OnSelectSlot1);
		EnhancedInputComponent->BindAction(SelectSlot2Action, ETriggerEvent::Triggered, this, &ALSPlayer::OnSelectSlot2);
		EnhancedInputComponent->BindAction(SelectSlot3Action, ETriggerEvent::Triggered, this, &ALSPlayer::OnSelectSlot3);
		EnhancedInputComponent->BindAction(SelectSlot4Action, ETriggerEvent::Triggered, this, &ALSPlayer::OnSelectSlot4);
		EnhancedInputComponent->BindAction(SelectSlot5Action, ETriggerEvent::Triggered, this, &ALSPlayer::OnSelectSlot5);
	}



	
}

void ALSPlayer::Move(const FInputActionValue& Value)
{
	if (bIsDead) return;

	// 로컬에서 제어 중인지 확인
	if (!IsLocallyControlled())
		return;

	// 이제(IJae) 캐릭터가 밀리고 있다면 입력 무시
	if (bIsBeingPushed)
		return;

	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// 일반 이동 로직
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ALSPlayer::Look(const FInputActionValue& Value)
{
	if (bIsDead) return;


	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}


void ALSPlayer::Attack()
{
	if (bIsDead) return;

	LS_LOG(LogLS, Warning, TEXT("ALSPlayer::Attack() called"));

	int32 CurrentSelectedSlot = SelectedSlot;
	if (CurrentSelectedSlot >= 0 && CurrentSelectedSlot < ItemInfoArray.Num())
	{
		if (!ItemInfoArray[CurrentSelectedSlot].IsEmpty)
		{
			// 아이템이 있으면 던지기
			LS_LOG(LogLS, Warning, TEXT("Item found in slot %d - throwing item"), CurrentSelectedSlot);
			ThrowItem();
			return;
		}
	}

	// 현재 선택된 슬롯에 아이템이 있는지 확인
	//if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetController()))
	//{
	//	

	//	/*if (ULSHUDWidget* HUD = PC->GetLSHUDWidget())
	//	{
	//		
	//	}*/
	//}

	// 아이템이 없으면
	LS_LOG(LogLS, Warning, TEXT("No item in selected slot "));


	FHitResult OutHitResult;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);
	const float AttackRange = 80.0f;
	const float AttackRadius = 50.0f;
	const float AttackDamage = 10.0f;
	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * AttackRange;
	FColor DrawColor;

	bool HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeSphere(AttackRadius), Params);
	if (HitDetected)
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetController());

		ILSTakeDamageInterface* HitNPC = Cast<ILSTakeDamageInterface>(OutHitResult.GetActor());
		if (HitNPC)
		{
			FDamageEvent DamageEvent;
			HitNPC->TakeDamage(10.0f, DamageEvent, GetController(), this);
			DrawColor = FColor::Blue;
		}
	}
	else
	{
		LS_LOG(LogLS, Warning, TEXT("ALSPlayer::Attack() - No hit detected"));
		
		DrawColor = FColor::Red;
	}

#if ENABLE_DRAW_DEBUG

	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	float CapsuleHalfHeight = AttackRange * 0.5f;


	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);

#endif
}





void ALSPlayer::PickUp()
{
	if (bIsDead) return;

	LS_LOG(LogLS, Warning, TEXT("ALSPlayer::PickUp() called"));

	FHitResult OutHitResult;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(PickUp), false, this);
	const float PickupRange = 200.0f;
	const float PickupRadius = 100.0f;
	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * PickupRange;
	FColor DrawColor;

	bool HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeSphere(PickupRadius), Params);

	if (HitDetected)
	{
		AActor* HitActor = OutHitResult.GetActor();
		LS_LOG(LogLS, Warning, TEXT("HIT DETECTED: %s"), HitActor ? *HitActor->GetName() : TEXT("Unknown"));

		// MasterItem 픽업 처리
		AMasterItem* HitItem = Cast<AMasterItem>(HitActor);
		if (HitItem)
		{
			LS_LOG(LogLS, Warning, TEXT("MASTER ITEM FOUND: %s"), *HitItem->GetName());

			// 아이템 픽업
			PickItemInSlot(HitItem->GetItemInfo());

			// 아이템 제거
			HitItem->Destroy();

			LS_LOG(LogLS, Warning, TEXT("Item picked up and destroyed: %s"), *HitItem->GetName());
			DrawColor = FColor::Green;
			return; // 픽업했으면 함수 종료
		}
		else
		{
			LS_LOG(LogLS, Warning, TEXT("HIT ACTOR IS NOT MASTER ITEM: %s"), *HitActor->GetName());
			DrawColor = FColor::Yellow;
		}

	}



	// 아이템이 감지되지 않았거나 MasterItem이 아닌 경우
	// 현재 선택된 슬롯의 아이템을 드롭
	LS_LOG(LogLS, Warning, TEXT("No valid item found - attempting to drop current slot item"));
	DropItemFromSlot();
	DrawColor = FColor::Red;

	// 디버그 라인
	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	float CapsuleHalfHeight = PickupRange * 0.5f;
	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, PickupRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);
}

void ALSPlayer::SpawnThrowableItem(const FItemDetails& ItemToThrow)
{
	LS_LOG(LogLS, Warning, TEXT("ALSPlayer::SpawnThrowableItem() called"));


	TSubclassOf<AMasterItem> ItemClass = ItemToThrow.Item_Class;
	if (ItemClass && DropItemLoc)
	{
		// 던지기 시작 위치 (플레이어 앞쪽)
		FVector ThrowStartLocation = DropItemLoc->GetComponentLocation();
		FRotator ThrowRotation = GetActorRotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = this;

		AMasterItem* ThrownItem = GetWorld()->SpawnActor<AMasterItem>(
			ItemClass,
			ThrowStartLocation,
			ThrowRotation,
			SpawnParams
		);

		if (ThrownItem)
		{
			// 던져진 아이템으로 설정
			ThrownItem->bIsThrown = true;

			if (UStaticMeshComponent* ItemMesh = ThrownItem->FindComponentByClass<UStaticMeshComponent>())
			{
				// 물리 시뮬레이션 활성화
				ItemMesh->SetSimulatePhysics(true);
				// 이 부분들 추가:
				ItemMesh->SetNotifyRigidBodyCollision(true); 
			
				// Hit 이벤트를 위해 충돌 설정
				ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				ItemMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

				// 던지는 방향과 힘 
				FVector ThrowDirection = GetActorForwardVector() + FVector(0, 0, 0.3f);
				const float THROW_FORCE = 1000.0f;

				// 임펄스 적용
				ItemMesh->AddImpulse(ThrowDirection * THROW_FORCE, NAME_None, true);

				// Hit 이벤트 바인딩 
				ItemMesh->OnComponentHit.AddDynamic(ThrownItem, &AMasterItem::OnItemHit);

				LS_LOG(LogLS, Warning, TEXT("Throwable item spawned: %s"), *ThrownItem->GetName());
			}
		}
		else
		{
			LS_LOG(LogLS, Error, TEXT("Failed to spawn throwable item"));
		}
	}
	else
	{
		LS_LOG(LogLS, Error, TEXT("ItemClass or DropItemLoc is null"));
	}
}

//void ALSPlayer::OnMouseWheelUp(const FInputActionValue& Value)
//{
//	UE_LOG(LogTemp, Warning, TEXT("Player: Mouse wheel up detected"));
//
//	// PlayerController를 통해 HUD에 접근
//	if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetController()))
//	{
//		PC->SelectNextSlot();
//	}
//	else
//	{
//		UE_LOG(LogTemp, Error, TEXT("PlayerController cast failed in OnMouseWheelUp"));
//	}
//}
//
//void ALSPlayer::OnMouseWheelDown(const FInputActionValue& Value)
//{
//	UE_LOG(LogTemp, Warning, TEXT("Player: Mouse wheel down detected"));
//
//	// PlayerController를 통해 HUD에 접근
//	if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetController()))
//	{
//		PC->SelectPreviousSlot();
//	}
//	else
//	{
//		UE_LOG(LogTemp, Error, TEXT("PlayerController cast failed in OnMouseWheelDown"));
//	}
//}


//Wheelchair part
void ALSPlayer::Interaction()
{
	if (bIsDead) return;

	// 로컬 컨트롤러가 있는지 확인
	if (!IsLocallyControlled())
	{
		return;
	}

	LS_LOG(LogLS, Warning, TEXT("[%s] ALSPlayer::Interaction() called"),
		HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT"));

	FHitResult OutHitResult;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);
	const float InteractionRange = 150.0f;
	const float InteractionRadius = 50.0f;
	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * InteractionRange;
	FColor DrawColor;

	bool HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeSphere(InteractionRadius), Params);
	if (HitDetected)
	{
		AActor* HitActor = OutHitResult.GetActor();
		if (!HitActor) return;

		LS_LOG(LogLS, Warning, TEXT("Hit Actor: %s"), *HitActor->GetName());

		// 휠체어 인터페이스 체크
		if (HitActor->GetClass()->ImplementsInterface(ULSWheelchairInterface::StaticClass()))
		{
			// 타겟 액터가 다른 플레이어인지 확인
			ALSPlayer* HitPlayer = Cast<ALSPlayer>(HitActor);
			if (HitPlayer)
			{
				// 내가 휠체어를 밀 수 있는지 확인
				if (CanPushWheelchair())
				{
					// 시제(SiJae)가 이제(IJae)와 상호작용
					if (HasAuthority())
					{
						// 서버에서 직접 실행
						bool bIsPushed = ILSWheelchairInterface::Execute_IsBeingPushed(HitActor);
						if (bIsPushed && HitPlayer->PusherCharacter == this)
						{
							// 밀기 중지
							ILSWheelchairInterface::Execute_StopPushingWheelchair(HitActor, this);
						}
						else if (!bIsPushed)
						{
							// 밀기 시작
							ILSWheelchairInterface::Execute_StartPushingWheelchair(HitActor, this);
						}
					}
					else
					{
						// 클라이언트에서는 서버에 요청
						ServerRequestWheelchairInteraction(HitActor);
					}
				}
				else
				{
					// 이제(IJae)는 다른 플레이어를 밀 수 없음을 로그로 남김
					LS_LOG(LogLS, Warning, TEXT("This character cannot push other players in wheelchairs"));
				}
				DrawColor = FColor::Yellow;
				return;
			}
		}

		// 여기서부터는 일반 상호작용 처리 (모든 캐릭터가 수행 가능)
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		ILSInteractionInterface* HitInteractable = Cast<ILSInteractionInterface>(HitActor);
		if (HitInteractable)
		{
			HitInteractable->InteractionProcess(PlayerController);
			DrawColor = FColor::Green;
		}

		ILSTakeDamageInterface* HitNPC = Cast<ILSTakeDamageInterface>(HitActor);
		if (HitNPC)
		{
			FDamageEvent DamageEvent;
			HitNPC->TakeDamage(10.0f, DamageEvent, GetController(), this);
			DrawColor = FColor::Blue;
		}
	}
	else
	{
		DrawColor = FColor::Red;
	}

#if ENABLE_DRAW_DEBUG
	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	float CapsuleHalfHeight = InteractionRange * 0.5f;
	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, InteractionRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);
#endif
}

bool ALSPlayer::CanPushWheelchair() const
{
	return bCanPushWheelchair;
}

bool ALSPlayer::ServerRequestWheelchairInteraction_Validate(AActor* TargetActor)
{
	return true;
}

void ALSPlayer::ServerRequestWheelchairInteraction_Implementation(AActor* TargetActor)
{
	// 서버에서 요청 처리
	if (!TargetActor || !TargetActor->GetClass()->ImplementsInterface(ULSWheelchairInterface::StaticClass()))
	{
		return;
	}

	ALSPlayer* WheelchairPlayer = Cast<ALSPlayer>(TargetActor);
	if (!WheelchairPlayer) return;

	// 현재 상태 로깅
	LS_LOG(LogLS, Warning, TEXT("Current wheelchair state: bIsBeingPushed=%s, PusherCharacter=%s"),
		WheelchairPlayer->bIsBeingPushed ? TEXT("true") : TEXT("false"),
		WheelchairPlayer->PusherCharacter ? *WheelchairPlayer->PusherCharacter->GetName() : TEXT("nullptr"));

	// 이미 내가 밀고 있다면 중지, 아니면 시작 (단순화된 로직)
	if (WheelchairPlayer->bIsBeingPushed && WheelchairPlayer->PusherCharacter == this)
	{
		// 밀기 중지
		WheelchairPlayer->bIsBeingPushed = false;
		WheelchairPlayer->PusherCharacter = nullptr;
		WheelchairPlayer->MulticastWheelchairStateChanged(false, nullptr);
		LS_LOG(LogLS, Warning, TEXT("Stopping wheelchair push"));
	}
	else
	{
		// 다른 상태면 강제로 초기화하고 밀기 시작
		WheelchairPlayer->bIsBeingPushed = true;
		WheelchairPlayer->PusherCharacter = this;
		WheelchairPlayer->MulticastWheelchairStateChanged(true, this);
		LS_LOG(LogLS, Warning, TEXT("Starting wheelchair push"));
	}
}



// 리플리케이션 설정
void ALSPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALSPlayer, CurrentHp);
	DOREPLIFETIME(ALSPlayer, bIsDead);

	DOREPLIFETIME(ALSPlayer, bIsBeingPushed);
	DOREPLIFETIME(ALSPlayer, PusherCharacter);
	DOREPLIFETIME(ALSPlayer, PushedWheelchairCharacter);
}

// 휠체어 인터페이스 구현
void ALSPlayer::StartPushingWheelchair_Implementation(ACharacter* Pusher)
{
	if (HasAuthority())
	{
		// 서버일 경우 직접 처리
		if (!bIsBeingPushed && Pusher != nullptr)
		{
			bIsBeingPushed = true;
			PusherCharacter = Pusher;

			MulticastWheelchairStateChanged(true, Pusher);
		}
	}
	else
	{
		ServerStartPushingWheelchair(Pusher);
	}
}

void ALSPlayer::StopPushingWheelchair_Implementation(ACharacter* Pusher)
{
	if (HasAuthority())
	{
		if (bIsBeingPushed && PusherCharacter == Pusher)
		{
			bIsBeingPushed = false;
			PusherCharacter = nullptr;

			MulticastWheelchairStateChanged(false, nullptr);
		}
	}
	else
	{
		ServerStopPushingWheelchair();
	}
}

bool ALSPlayer::IsBeingPushed_Implementation() const
{
	return bIsBeingPushed;
}

// Server RPC
void ALSPlayer::ServerStartPushingWheelchair_Implementation(ACharacter* Pusher)
{
	StartPushingWheelchair(Pusher);
}

bool ALSPlayer::ServerStartPushingWheelchair_Validate(ACharacter* Pusher)
{
	return true;
}

void ALSPlayer::ServerStopPushingWheelchair_Implementation()
{
	StopPushingWheelchair(PusherCharacter);
}

bool ALSPlayer::ServerStopPushingWheelchair_Validate()
{
	return true;
}

// Multicast RPC
void ALSPlayer::MulticastWheelchairStateChanged_Implementation(bool bPushing, ACharacter* Pusher)
{
	bIsBeingPushed = bPushing;
	PusherCharacter = Pusher;

	// 이제(IJae) 캐릭터의 움직임 제어 설정
	if (bIsBeingPushed)
	{
		// 이제 캐릭터는 스스로 움직일 수 없음 (MOVE_None)
		GetCharacterMovement()->SetMovementMode(MOVE_None);
	}
	else
	{
		// 이제 캐릭터가 다시 스스로 움직일 수 있음
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

void ALSPlayer::HandleWheelchairMovement()
{
	if (!PusherCharacter)
		return;

	// 시제(SiJae)의 위치와 방향 가져오기
	FVector PusherLocation = PusherCharacter->GetActorLocation();
	FVector PusherForward = PusherCharacter->GetActorForwardVector();

	// 이제(IJae) 캐릭터를 시제(SiJae) 캐릭터 앞에 위치시키기
	FVector NewLocation = PusherLocation + (PusherForward * 100.0f);

	// 이제(IJae) 캐릭터의 위치와 회전 설정
	// 이제 캐릭터는 시제 캐릭터가 보는 방향으로 회전
	SetActorLocation(NewLocation, true);

	FRotator NewRotation = PusherForward.Rotation();
	NewRotation.Pitch = 0.0f;
	SetActorRotation(NewRotation);
}


//is dead
void ALSPlayer::OnHpReachedZero(float ZeroHp)
{
	UE_LOG(LogTemp, Warning, TEXT("HP reached zero - Player dying"));
	Die();
}

void ALSPlayer::Die()
{
	if (bIsDead) return; // 이미 죽었으면 리턴

	bIsDead = true;
	UE_LOG(LogTemp, Warning, TEXT("Player died"));

	// 모든 입력 차단
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		DisableInput(PC);
	}

	// 충돌 비활성화
	SetActorEnableCollision(false);

	// 이동 비활성화
	GetCharacterMovement()->SetMovementMode(MOVE_None);

	// 5초 후 부활 타이머 시작
	GetWorld()->GetTimerManager().SetTimer(
		RespawnTimerHandle,
		this,
		&ALSPlayer::Respawn,
		5.0f,
		false
	);
}

void ALSPlayer::Respawn()
{
	if (!bIsDead) return; // 이미 살아있으면 리턴

	bIsDead = false;
	UE_LOG(LogTemp, Warning, TEXT("Player respawned"));

	// HP 풀로 회복
	if (HpComponent)
	{
		HpComponent->SetHp(100.0f); // MaxHp로 설정
		CurrentHp = 100.0f;
	}

	// 입력 재활성화
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		EnableInput(PC);
	}

	// 충돌 재활성화
	SetActorEnableCollision(true);

	// 이동 재활성화
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	// 타이머 클리어
	GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);
}


void ALSPlayer::OnSelectSlot1()
{
	UE_LOG(LogTemp, Warning, TEXT("Player: Slot 1 key pressed"));

	SelectSlot(0);

	/*if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetController()))
	{
		SelectSlot(0); 
	}*/
}

void ALSPlayer::OnSelectSlot2()
{
	UE_LOG(LogTemp, Warning, TEXT("Player: Slot 2 key pressed"));

	if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetController()))
	{
		SelectSlot(1);
	}
}

void ALSPlayer::OnSelectSlot3()
{
	UE_LOG(LogTemp, Warning, TEXT("Player: Slot 3 key pressed"));

	if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetController()))
	{
		SelectSlot(2);
	}
}

void ALSPlayer::OnSelectSlot4()
{
	UE_LOG(LogTemp, Warning, TEXT("Player: Slot 4 key pressed"));

	if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetController()))
	{
		SelectSlot(3);
	}
}

void ALSPlayer::OnSelectSlot5()
{
	UE_LOG(LogTemp, Warning, TEXT("Player: Slot 5 key pressed"));

	if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetController()))
	{
		SelectSlot(4);
	}
}

void ALSPlayer::SelectSlot(int32 SlotIndex)
{
	if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetController())) 
	{
		if (ULSHUDWidget* HUD = PC->GetLSHUDWidget())
		{
			// 슬롯 인덱스 유효성 검사
			if (SlotIndex >= 0 && SlotIndex <= 4) // 0~4 슬롯
			{
				SelectedSlot = SlotIndex;
				//ChangeSlot(SlotIndex);
				UE_LOG(LogTemp, Warning, TEXT("Direct slot selection: %d"), SlotIndex);

				// 슬롯 색상 업데이트
				//LSHUDWidget->UpdateSlotBorderColors();


				// 플레이어 컨트롤러 가져오기
				ALSPlayerController* LSController = Cast<ALSPlayerController>(GetController());
				if (LSController && LSController->GetLSHUDWidget())
				{
					LSController->GetLSHUDWidget()->UpdateSlotBorderColors(SelectedSlot);


				}


				else
				{
					UE_LOG(LogTemp, Error, TEXT("Invalid slot index: %d"), SlotIndex);
				}

		}
	}
	
	
		





		//if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetController()))
		//{
		//	if (ULSHUDWidget* HUD = PC->GetLSHUDWidget())
		//	{
		//		HUD->UpdateSlotBorderColors();
		//	}
		//}

	}
}



//void ALSPlayer::ChangeSlot(int32 NewSlot)
//{
//	//SelectedSlot = NewSlot;
//
//	//// 슬롯 범위 검증 및 순환 처리
//	//if (NewSlot < 0)
//	//{
//	//	SelectedSlot = MaxSlots;
//	//}
//	//else if (NewSlot > MaxSlots)
//	//{
//	//	SelectedSlot = 0;
//	//}
//	//else
//	//{
//	//	
//	//}
//
//	//UE_LOG(LogTemp, Warning, TEXT("Slot changed to: %d"), SelectedSlot);
//
//	// 슬롯 색상 업데이트
//	//UpdateSlotBorderColors();
//}





