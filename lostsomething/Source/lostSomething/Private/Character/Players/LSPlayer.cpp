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
#include "Character/UI/LSHUDWidget.h"
#include "Character/Components/LSHpComponent.h"


// Sets default values
ALSPlayer::ALSPlayer()
{
	//// Stat Component 
	//Stat = CreateDefaultSubobject<ULSCharacterStatComponent>(TEXT("Stat"));

	//// Widget Component 
	//HpBar = CreateDefaultSubobject<ULSWidgetComponent>(TEXT("Widget"));
	//HpBar->SetupAttachment(GetMesh());
	//HpBar->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	//static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef(TEXT("/Game/Players/UI/WBP_HpBar.WBP_HpBar_C"));
	//if (HpBarWidgetRef.Class)
	//{
	//	HpBar->SetWidgetClass(HpBarWidgetRef.Class);
	//	HpBar->SetWidgetSpace(EWidgetSpace::Screen);
	//	HpBar->SetDrawSize(FVector2D(150.0f, 15.0f));
	//	HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//}

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//컨트롤러 회전시 회전 x
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
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

		// 현재 체력값으로 HUD 초기화
		OnHpChanged(HpComponent->GetHp());


	}

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


float ALSPlayer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	LS_LOG(LogLS, Warning, TEXT("LSPLAYER  :: Take Damage : %f"), DamageAmount);

	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (HpComponent)
	{
		const float NewHp = HpComponent->GetHp() - DamageAmount;
		HpComponent->SetHp(NewHp);

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


void ALSPlayer::OnHpChanged(float NewHp)
{
	UE_LOG(LogTemp, Warning, TEXT("ALSPlayer::OnHpChanged called with HP: %.1f"), NewHp);

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
void ALSPlayer::PickItem(const FItemDetails& PickedItemInfo)
{
	// Pick Item In Slot 함수 호출
	PickItemInSlot(PickedItemInfo);

	//// ItemInfoArray에서 빈 슬롯 찾기 (IsEmpty = true인 슬롯)
	//for (int32 i = 0; i < ItemInfoArray.Num(); ++i)
	//{
	//	if (ItemInfoArray[i].IsEmpty)
	//	{
	//		// 빈 슬롯에 아이템 정보 저장
	//		ItemInfoArray[i] = PickedItemInfo;
	//		ItemInfoArray[i].IsEmpty = false;

	//		UE_LOG(LogTemp, Warning, TEXT("Item picked up and stored in slot %d"), i);
	//		return; // 첫 번째 빈 슬롯에 저장 후 함수 종료
	//	}
	//}

	//// 빈 슬롯이 없는 경우
	//UE_LOG(LogTemp, Warning, TEXT("Inventory is full! Cannot pick up item."));

}

//슬롯에 아이템 넣기
void ALSPlayer::PickItemInSlot(const FItemDetails& PickedItem)
{
	UE_LOG(LogTemp, Warning, TEXT("ALSPlayer::PickItemInSlot() called"));

	// PlayerController를 통해 HUD에서 현재 선택된 슬롯 인덱스 가져오기
	if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetController()))
	{
		if (ULSHUDWidget* HUD = PC->GetLSHUDWidget())
		{
			//SelectedSlots 변수
			int32 CurrentSelectedSlot = HUD->GetSelectedSlot();

			// ItemInfoArray GET: CurrentSelectedSlot 인덱스로 현재 슬롯 아이템 가져오기
			if (CurrentSelectedSlot >= 0 && CurrentSelectedSlot < ItemInfoArray.Num())
			{
				FItemDetails CurrentSlotItem = ItemInfoArray[CurrentSelectedSlot];

				//현재 슬롯 아이템의 IsEmpty 값 추출
				bool bCurrentSlotIsEmpty = CurrentSlotItem.IsEmpty;

				//IsEmpty 조건 확인
				if (bCurrentSlotIsEmpty) 
				{
					// 선택된 슬롯에 새 아이템 저장
					ItemInfoArray[CurrentSelectedSlot] = PickedItem;

					// Break ItemDetails에서 Item Icon 추출
					UTexture2D* ItemIcon = PickedItem.Item_Icon.LoadSynchronous();

					// Set Icon: HUD의 Set Icon 이벤트 호출
					// Current Slot: CurrentSelectedSlot, Item Icon: ItemIcon
					HUD->SetIcon(CurrentSelectedSlot, ItemIcon);

					//item class추출
					TSubclassOf<AMasterItem> ItemClass = PickedItem.Item_Class;

					// Spawn Actor: Item Class로 액터 생성
					if (ItemClass)
					{
						FVector SpawnLocation = GetActorLocation();
						FRotator SpawnRotation = GetActorRotation();
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
							UE_LOG(LogTemp, Warning, TEXT("Item spawned: %s"));
						}
						else
						{
							UE_LOG(LogTemp, Error, TEXT("Failed to spawn item"));
						}
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("Item Class is null - no item to spawn"));
					}

					UE_LOG(LogTemp, Warning, TEXT("Item stored in slot %d and icon updated"), CurrentSelectedSlot);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Slot %d is already occupied"), CurrentSelectedSlot);
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



// Called to bind functionality to input
void ALSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
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
		EnhancedInputComponent->BindAction(MouseWheelUpAction, ETriggerEvent::Triggered, this, &ALSPlayer::OnMouseWheelUp);
		EnhancedInputComponent->BindAction(MouseWheelDownAction, ETriggerEvent::Triggered, this, &ALSPlayer::OnMouseWheelDown);

		//Pickup
		EnhancedInputComponent->BindAction(PickUpAction, ETriggerEvent::Triggered, this, &ALSPlayer::PickUp);

	}



	
}

void ALSPlayer::Move(const FInputActionValue& Value)
{
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

	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}


void ALSPlayer::Attack()
{
	LS_LOG(LogLS, Warning, TEXT("ALSPlayer::Attack() called"));

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
	LS_LOG(LogLS, Warning, TEXT("ALSPlayer::PickUp() called"));

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
		LS_LOG(LogLS, Warning, TEXT("ALSPlayer::PickUp() - No hit detected"));

		DrawColor = FColor::Red;
	}

#if ENABLE_DRAW_DEBUG

	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	float CapsuleHalfHeight = AttackRange * 0.5f;


	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);

#endif
}

void ALSPlayer::OnMouseWheelUp(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Player: Mouse wheel up detected"));

	// PlayerController를 통해 HUD에 접근
	if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetController()))
	{
		PC->SelectNextSlot();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController cast failed in OnMouseWheelUp"));
	}
}

void ALSPlayer::OnMouseWheelDown(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Player: Mouse wheel down detected"));

	// PlayerController를 통해 HUD에 접근
	if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetController()))
	{
		PC->SelectPreviousSlot();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController cast failed in OnMouseWheelDown"));
	}
}


//Wheelchair part
void ALSPlayer::Interaction()
{
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

	// 이 함수는 이제(IJae) 캐릭터에서 호출됨
	if (HasAuthority())
	{
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
}


//Widget
//void ALSPlayer::SetupCharacterWidget(ULSUserWidget* InUserWidget)
//{
//	ULSHpBarWidget* HpBarWidget = Cast<ULSHpBarWidget>(InUserWidget);
//	if (HpBarWidget)
//	{
//		HpBarWidget->SetMaxHp(Stat->GetMaxHP());
//		HpBarWidget->UpdateHpBar(Stat->GetCurrentHP());
//		Stat->OnHpChanged.AddUObject(HpBarWidget, &ULSHpBarWidget::UpdateHpBar);
//	}
//
//}
