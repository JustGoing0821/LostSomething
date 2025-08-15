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
#include "Character/Animation/LSPlayerSiJaeAnimInstance.h"
#include "Character/Animation/LSPlayerIJaeAnimInstance.h"
#include "Components/ArrowComponent.h"
#include "Character/UI/LSHUDWidget.h"
#include "Character/Players/LSPlayerSiJae.h"
#include "Character/UI/LSDeathWidget.h" 
#include "Character/Components/LSHpComponent.h"


// Sets default values
ALSPlayer::ALSPlayer()
{
	/*Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));


	Weapon->SetupAttachment(GetMesh());*/

	//소켓 붙이기
	/*FName WeaponSocket(TEXT("hand_rSocket"));
	if (GetMesh()->DoesSocketExist(WeaponSocket))
	{
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
	}*/

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetMesh());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	//Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	//Mesh1P->SetOnlyOwnerSee(true);
	//Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	//Mesh1P->bCastDynamicShadow = false;
	//Mesh1P->CastShadow = false;
	////Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	//Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));


 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	//컨트롤러 회전시 회전 x
	//bUseControllerRotationPitch = false;

	//// 이동 방향에 따라 캐릭터 회전
	////GetCharacterMovement()->bOrientRotationToMovement = true;
	//bUseControllerRotationYaw = true; //수정

	//bUseControllerRotationRoll = false;


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


	////camera boom 
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
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
		LastDistanceCheckTime += DeltaTime;
		if (LastDistanceCheckTime >= DistanceCheckInterval)
		{
			CheckCombineDistance();
			LastDistanceCheckTime = 0.0f;
		}
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


}

void ALSPlayer::ApplyDamage(float DamageAmount)
{
	if (bIsDead) return;

	LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));

	if (HpComponent)
	{
		CurrentHp -= DamageAmount;
		HpComponent->SetHp(CurrentHp);
		LS_LOG(LogLS, Log, TEXT("ApplyDamage SetHp Called"));
	}


	if (CurrentHp <= 0)
	{
		LS_LOG(LogLS, Error, TEXT("hp is zero call Die"));
		Die();
		return;
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
	LS_LOG(LogLS, Warning, TEXT("ALSPlayer::OnHpChanged called with HP: %.1f"), NewHp);

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


//is dead
void ALSPlayer::OnHpReachedZero(float ZeroHp)
{
	UE_LOG(LogTemp, Warning, TEXT("HP reached zero - Player dying"));
	Die();
}

void ALSPlayer::MeshHide()
{
	if (GetMesh())
	{
		GetMesh()->SetVisibility(false);
		UE_LOG(LogTemp, Warning, TEXT("Player mesh hidden (delayed)"));
	}
}

void ALSPlayer::Die()
{
	if (bIsDead) return; // 이미 죽었으면 리턴

	bIsDead = true;
	UE_LOG(LogTemp, Warning, TEXT("Player died"));

	

	if (HasAuthority())
	{
		MultiDie();
		// 모든 입력 차단
		if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetController()))
		{
			//PC->ShowDeathWidget();
			//DisableInput(PC);
		}

		// 메시 숨기기
		if (GetMesh())
		{
			
			/*GetMesh()->SetVisibility(false);
			UE_LOG(LogTemp, Warning, TEXT("Player mesh hidden"));*/
		}



		// 충돌 비활성화
		//SetActorEnableCollision(false);

		// 이동 비활성화
		GetCharacterMovement()->SetMovementMode(MOVE_None);

		// 5초 후 부활 타이머 시작
		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle,this,&ALSPlayer::Respawn,5.0f,false);
		
	}

	
}

void ALSPlayer::ServerDie_Implementation()
{
}

void ALSPlayer::MultiDie_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("MulticastPlayerDied called"));
	if (GetMesh())
	{
		GetWorld()->GetTimerManager().SetTimer(MeshHideTimerHandle, this, &ALSPlayer::MeshHide, 2.0f, false);

		//GetMesh()->SetVisibility(false);
	}

	ULSPlayerSiJaeAnimInstance* AnimInstance = Cast<ULSPlayerSiJaeAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->SetDeadAnim();
		UE_LOG(LogTemp, Warning, TEXT("Player died ANIMATION SIJAE"));
	}

	ULSPlayerIJaeAnimInstance* AnimInstanceIJae = Cast<ULSPlayerIJaeAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstanceIJae)
	{
		AnimInstanceIJae->SetDeadAnim();
		UE_LOG(LogTemp, Warning, TEXT("Player died ANIMATION IJAE"));
	}

	// 본인 클라이언트에서만 UI 처리
	if (IsLocallyControlled())
	{
		if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetController()))
		{
			PC->ShowDeathWidget();
			DisableInput(PC);
		}
	}
}

void ALSPlayer::ClientDie_Implementation()
{
	
}

void ALSPlayer::Respawn()
{
	if (!bIsDead) return; // 이미 살아있으면 리턴

	bIsDead = false;
	UE_LOG(LogTemp, Warning, TEXT("Player respawned"));

	if (HasAuthority())
	{
		
		// HP 풀로 회복
		if (HpComponent)
		{
			HpComponent->SetHp(100.0f);
			CurrentHp = 100.0f;
		}

		// 타이머 클리어
		GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);

		MultiRespawn();
	}


}

void ALSPlayer::ServerRespawn_Implementation()
{
}

void ALSPlayer::MultiRespawn_Implementation()
{
		// 메시 다시 보이게
	if (GetMesh())
	{
		GetMesh()->SetVisibility(true);
		
	}

	// HP 풀로 회복
	if (HpComponent)
	{
		HpComponent->SetHp(100.0f); // MaxHp로 설정
		CurrentHp = 100.0f;
	}

	// 입력 재활성화
	if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetController()))
	{
		PC->HideDeathWidget();
		EnableInput(PC);
	}

	// 충돌 재활성화
	SetActorEnableCollision(true);

	// 이동 재활성화
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	

}

void ALSPlayer::ClientRespawn_Implementation()
{
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
		EnhancedInputComponent->BindAction(InterreactAction, ETriggerEvent::Triggered, this, &ALSPlayer::Interreact);

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

		//voice
		//Attack
		EnhancedInputComponent->BindAction(VoiceAction, ETriggerEvent::Started, this, &ALSPlayer::VoiceStart);
		EnhancedInputComponent->BindAction(VoiceAction, ETriggerEvent::Completed, this, &ALSPlayer::VoiceStop);

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


	if (HasAuthority())
	{
		ProcessAttack();
	}
	else
	{
		ServerProcessAttack();
	}

	

}

void ALSPlayer::ProcessAttack()
{

	FHitResult OutHitResult;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);
	const float AttackRange = 80.0f;
	const float AttackRadius = 30.0f;
	const float AttackDamage = 10.0f;
	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * AttackRange;
	FColor DrawColor;

	bool HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeSphere(AttackRadius), Params);

	if (HitDetected)
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetController());

		if (ALSPlayer* HitPlayer = Cast<ALSPlayer>(OutHitResult.GetActor()))
		{
			// Player면 데미지 x
			UE_LOG(LogTemp, Warning, TEXT("Hit player - no damage"));
			DrawColor = FColor::Red;
		}

		else if (ILSTakeDamageInterface* HitNPC = Cast<ILSTakeDamageInterface>(OutHitResult.GetActor()))
		{
			// NPC만 데미지 적용
			FDamageEvent DamageEvent;
			HitNPC->TakeDamage(10.0f, DamageEvent, GetController(), this);
			DrawColor = FColor::Blue;
		}

		/*ILSTakeDamageInterface* HitNPC = Cast<ILSTakeDamageInterface>(OutHitResult.GetActor());
		if (HitNPC)
		{
			FDamageEvent DamageEvent;
			HitNPC->TakeDamage(10.0f, DamageEvent, GetController(), this);
			DrawColor = FColor::Blue;
		}*/
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
	//
}

void ALSPlayer::ServerProcessAttack_Implementation()
{
	ProcessAttack();
	MultiProcessAttack();
}

void ALSPlayer::MultiProcessAttack_Implementation()
{
	ULSPlayerSiJaeAnimInstance* AnimInstance = Cast<ULSPlayerSiJaeAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->SetAttackAnim();
		UE_LOG(LogTemp, Warning, TEXT("Player Attack ANIMATION SIJAE"));
	}
}

void ALSPlayer::ClientProcessAttack_Implementation()
{
}





//
//	// 아이템이 감지되지 않았거나 MasterItem이 아닌 경우
//	// 현재 선택된 슬롯의 아이템을 드롭
//	LS_LOG(LogLS, Warning, TEXT("No valid item found - attempting to drop current slot item"));
//	DropItemFromSlot();
//	DrawColor = FColor::Red;
//
//	// 디버그 라인
//	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
//	float CapsuleHalfHeight = PickupRange * 0.5f;
//	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, PickupRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);
//}


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

void ALSPlayer::Interreact()
{
}

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
	const float InteractionRange = 80.0f;
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

	/*	ILSTakeDamageInterface* HitNPC = Cast<ILSTakeDamageInterface>(HitActor);
		if (HitNPC)
		{
			FDamageEvent DamageEvent;
			HitNPC->TakeDamage(10.0f, DamageEvent, GetController(), this);
			DrawColor = FColor::Blue;
		}*/
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

	// 이미 내가 밀고 있다면 중지, 아니면 시작 (단순화된 로직)
	if (WheelchairPlayer->bIsBeingPushed && WheelchairPlayer->PusherCharacter == this)
	{
		// 밀기 중지
		WheelchairPlayer->bIsBeingPushed = false;
		WheelchairPlayer->PusherCharacter = nullptr;
		this->PushedWheelchairCharacter = nullptr;
		WheelchairPlayer->MulticastWheelchairStateChanged(false, nullptr);
	}
	else
	{
		// 다른 상태면 강제로 초기화하고 밀기 시작
		WheelchairPlayer->bIsBeingPushed = true;
		WheelchairPlayer->PusherCharacter = this;
		this->PushedWheelchairCharacter = WheelchairPlayer;
		WheelchairPlayer->MulticastWheelchairStateChanged(true, this);
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
	PusherCharacter = bPushing ? Pusher : nullptr;

	if (ACharacter* Source = (Pusher ? Pusher : PusherCharacter.Get()))
	{
		if (ALSPlayer* Pusher = Cast<ALSPlayer>(Source))
		{
			if (bPushing)
			{
				Pusher->PushedWheelchairCharacter = this;
			}
			else
			{
				if (Pusher->PushedWheelchairCharacter == this)
				{
					Pusher->PushedWheelchairCharacter = nullptr;
				}
			}
		}
	}

	// 타겟 Yaw 계산
	float TargetYaw = 0.f;
	if (bPushing) // 3인칭 전환 시 (이제가 시제를 미는 경우)
	{
		TargetYaw = GetControlRotation().Yaw; // 이제의 현재 시점
	}
	else // 1인칭 전환 시 (시제가 이제를 밀다 해제)
	{
		if (ACharacter* Source = (Pusher ? Pusher : PusherCharacter.Get()))
		{
			TargetYaw = Source->GetControlRotation().Yaw; // 시제의 시점
		}
		else
		{
			TargetYaw = GetActorRotation().Yaw;
		}
	}

	FRotator TargetRot(0.f, TargetYaw, 0.f);

	if (bIsBeingPushed)
	{
		// 1인칭 -> 3인칭
		if (IsLocallyControlled())
		{
			if (APlayerController* PC = Cast<APlayerController>(Controller))
			{
				PC->SetIgnoreLookInput(true);

				// 컨트롤러 & 액터 회전 동기화
				PC->SetControlRotation(TargetRot);
				SetActorRotation(TargetRot, ETeleportType::TeleportPhysics);

				// 카메라 전환
				if (CameraBoom) CameraBoom->bDoCollisionTest = false;
				FirstPersonCameraComponent->SetActive(false);
				FollowCamera->SetActive(true);

				// 블렌딩 제거
				if (PC->PlayerCameraManager)
					PC->PlayerCameraManager->SetGameCameraCutThisFrame();

				// 입력 해제는 다음 틱
				GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
					{
						if (APlayerController* PC2 = Cast<APlayerController>(Controller))
							PC2->SetIgnoreLookInput(false);
					});
			}
		}

		if (APlayerController* PC = Cast<APlayerController>(Controller))
		{
			PC->SetIgnoreMoveInput(true);
		}

		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		return;
	}

	// 3인칭 -> 1인칭
	if (IsLocallyControlled())
	{
		if (APlayerController* PC = Cast<APlayerController>(Controller))
		{
			PC->SetIgnoreLookInput(true);

			// 컨트롤러 & 액터 회전 동기화
			PC->SetControlRotation(TargetRot);
			SetActorRotation(TargetRot, ETeleportType::TeleportPhysics);

			// 카메라 전환
			if (CameraBoom) CameraBoom->bDoCollisionTest = true;
			FollowCamera->SetActive(false);
			FirstPersonCameraComponent->SetActive(true);

			// 블렌딩 제거
			if (PC->PlayerCameraManager)
				PC->PlayerCameraManager->SetGameCameraCutThisFrame();

			// 입력 해제는 다음 틱
			GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
				{
					if (APlayerController* PC2 = Cast<APlayerController>(Controller))
						PC2->SetIgnoreLookInput(false);
				});
		}
	}

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		PC->SetIgnoreMoveInput(false);
	}

	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void ALSPlayer::HandleWheelchairMovement()
{
	if (!PusherCharacter)
		return;

	FVector PusherLocation = PusherCharacter->GetActorLocation();
	FVector PusherForward = PusherCharacter->GetActorForwardVector();

	FVector TargetLocation = PusherLocation + (PusherForward * NormalCombineDistance);
	FVector CurrentLocation = GetActorLocation();
	TargetLocation.Z = CurrentLocation.Z;
	float DistanceToTarget = FVector::Dist(CurrentLocation, TargetLocation);

	FVector NewLocation;
	if (DistanceToTarget > NormalCombineDistance * 0.5f)
	{
		float InterpSpeed = 5.0f; 
		NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation,
			GetWorld()->GetDeltaSeconds(), InterpSpeed);
	}
	else
	{
		NewLocation = TargetLocation;
	}

	SetActorLocation(NewLocation, true);

	FRotator NewRotation = PusherForward.Rotation();
	NewRotation.Pitch = 0.0f;
	SetActorRotation(NewRotation);
}

void ALSPlayer::CheckCombineDistance()
{
	if (!bIsBeingPushed || !PusherCharacter)
		return;

	FVector MyLocation = GetActorLocation();
	FVector PusherLocation = PusherCharacter->GetActorLocation();
	float CurrentDistance = FVector::Dist(MyLocation, PusherLocation);

	if (CurrentDistance > MaxCombineDistance)
	{
		if (HasAuthority())
		{
			AutoSeparateFromWheelchair();
		}
		else
		{
			ServerRequestAutoSeparation();
		}
	}
	else if (CurrentDistance > NormalCombineDistance * 1.2f) // 20% 여유분
	{
	}
}

void ALSPlayer::AutoSeparateFromWheelchair()
{
	if (!HasAuthority())
		return;

	// 합체 상태 해제
	bIsBeingPushed = false;
	ACharacter* FormerPusher = PusherCharacter;
	PusherCharacter = nullptr;

	// 모든 클라이언트에 상태 변경 알림
	MulticastWheelchairStateChanged(false, nullptr);
}

bool ALSPlayer::ServerRequestAutoSeparation_Validate()
{
	return true;
}

void ALSPlayer::ServerRequestAutoSeparation_Implementation()
{
	// 서버에서 거리 재확인 후 분리
	if (bIsBeingPushed && PusherCharacter)
	{
		FVector MyLocation = GetActorLocation();
		FVector PusherLocation = PusherCharacter->GetActorLocation();
		float CurrentDistance = FVector::Dist(MyLocation, PusherLocation);

		if (CurrentDistance > MaxCombineDistance)
		{
			AutoSeparateFromWheelchair();
		}
	}
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

//voice section
void ALSPlayer::VoiceStart(const FInputActionValue& Value)
{
	auto pc = GetController<ALSPlayerController>();
	if (pc && pc->IsLocalController())
	{
		pc->StartTalking();
	}
}

void ALSPlayer::VoiceStop(const FInputActionValue& Value)
{
	auto pc = GetController<ALSPlayerController>();
	if (pc && pc->IsLocalController())
	{
		pc->StopTalking();
	}
}

//void ALSPlayer::VoiceStart(const FInputActionValue& Value)
//{
//	auto pc:ANetPlayerController* = GetController<ANetPlayerController>();
//	if (pc && pc->IsLocalController())
//	{
//		pc->StartTalking();
//	}
//}
//
//void ALSPlayer::VoiceStop(const FInputActionValue& Value)
//{
//	auto pc:ANetPlayerController* = GetController<ANetPlayerController>();
//	if (pc && pc->IsLocalController())
//	{
//		pc->StopTalking();
//	}
//}

//아이템 줍기. PickItemInSlot으로 연결
void ALSPlayer::PickUp()
{
	if (!HasAuthority())
	{
		ServerPickUp();
		return;
	}

	MultiPickUp();

}

void ALSPlayer::ServerPickUp_Implementation()
{

	MultiPickUp();


}

void ALSPlayer::MultiPickUp_Implementation()
{
	ULSPlayerSiJaeAnimInstance* AnimInstance = Cast<ULSPlayerSiJaeAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->SetPickUpAnim();
		UE_LOG(LogTemp, Warning, TEXT("Player Picking ANIMATION SIJAE"));
	}
}


void ALSPlayer::PickUpCore()
{

	/*if (ALSPlayerSiJae* SiJae = Cast<ALSPlayerSiJae>(this))
	{
		SiJae->WeaponPickUp();
		LS_LOG(LogLS, Warning, TEXT("ALSPlayer::weaponpickup() called"));

	}*/

	if (bIsDead) return;

	LS_LOG(LogLS, Warning, TEXT("ALSPlayer::PickUp() called"));

	FHitResult OutHitResult;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(PickUp), false, this);
	const float PickupRange = 200.0f;
	const float PickupRadius = 100.0f;
	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * PickupRange;
	FColor DrawColor;

	//아이템 버리기 (중첩 아이템 없을때)
	int32 CurrentSelectedSlot = SelectedSlot;
	FItemDetails CurrentSlotItem = ItemInfoArray[CurrentSelectedSlot];
	bool bCurrentSlotIsEmpty = CurrentSlotItem.IsEmpty;

	//슬롯이 차있다면
	

	//아이템 hit 시

	bool HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeSphere(PickupRadius), Params);
	if (HitDetected)
	{
		AActor* HitActor = OutHitResult.GetActor();
		LS_LOG(LogLS, Warning, TEXT("HIT DETECTED: %s"), HitActor ? *HitActor->GetName() : TEXT("Unknown"));

		// MasterItem 픽업 처리
		AMasterItem* HitItem = Cast<AMasterItem>(HitActor);
		if (HitItem)
		{
			//아이템 중첩상태에서 pick (인벤차있는상태에서 콜리전 겹쳐서 주우면)
			if (!CurrentSlotItem.IsEmpty)
			{
				LS_LOG(LogLS, Warning, TEXT("Slot %d is occupied - only dropping existing item"), CurrentSelectedSlot);
				DropItemFromSlot();
				DrawColor = FColor::Orange;

				// 디버그 라인
				FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
				float CapsuleHalfHeight = PickupRange * 0.5f;
				DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, PickupRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);
				return; // 드롭만 하고 함수 종료
			}
		}

		
		if (!HitItem)
		{
			LS_LOG(LogLS, Warning, TEXT("HitActor is not a MasterItem - ignoring"));
			return; // 마스터 아이템이 아니면 픽업 무시하고 종료
		}

		//수정
		// 아이템 픽업
		

		if (HasAuthority())
		{
			// 서버
			PickItemInSlot(HitItem->GetItemInfo());
			HitItem->Destroy();
		}
		else
		{
			// 클라이언트인 경우: 서버에 삭제 요청
			ServerPickUpCore(HitItem);
			
		}

		// 아이템 제거
		//HitItem->Destroy();

		LS_LOG(LogLS, Warning, TEXT("Item picked up and destroyed: %s"), *HitItem->GetName());
		DrawColor = FColor::Green;
		return; // 픽업했으면 함수 종료
	}
	else
	{
		if (!CurrentSlotItem.IsEmpty)
		{
			LS_LOG(LogLS, Warning, TEXT("Slot %d is occupied - only dropping existing item"), CurrentSelectedSlot);
			DropItemFromSlot();
			DrawColor = FColor::Orange;

			//// 디버그 라인
			//FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
			//float CapsuleHalfHeight = PickupRange * 0.5f;
			//DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, PickupRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);

		}

		DrawColor = FColor::Yellow;
	}
	
}

void ALSPlayer::ServerPickUpCore_Implementation(AMasterItem* TargetItem)
{
	if (!TargetItem) return;

	FItemDetails ItemData = TargetItem->GetItemInfo();
	TargetItem->Destroy();
	MultiPickUpCore(TargetItem);
	ClientPickUpCore(ItemData);

}

void ALSPlayer::MultiPickUpCore_Implementation(AActor* TargetItem)
{
	ULSPlayerSiJaeAnimInstance* AnimInstance = Cast<ULSPlayerSiJaeAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->SetPickUpAnim();
		UE_LOG(LogTemp, Warning, TEXT("Player Picking ANIMATION SIJAE"));
	}
}


void ALSPlayer::ClientPickUpCore_Implementation(FItemDetails ItemData)
{
	PickItemInSlot(ItemData);
}

//아이템 drop
void ALSPlayer::DropItemFromSlot()
{
	UE_LOG(LogTemp, Warning, TEXT("ALSPlayer::DropItemFromSlot() called"));

	
	if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetController()))
	{
		if (ULSHUDWidget* HUD = PC->GetLSHUDWidget())
		{

			int32 CurrentSelectedSlot = SelectedSlot;

			//현재 슬롯 아이템 가져오기
			if (CurrentSelectedSlot >= 0 && CurrentSelectedSlot < ItemInfoArray.Num())
			{
				FItemDetails CurrentSlotItem = ItemInfoArray[CurrentSelectedSlot];

				// ItemDetails: IsEmpty와 Item Class 값 추출
				bool bIsSlotEmpty = CurrentSlotItem.IsEmpty;
				TSubclassOf<AMasterItem> ItemClass = CurrentSlotItem.Item_Class;

				//슬롯이 차있을 때
				if (!bIsSlotEmpty)
				{
					// Spawn Actor: DropItemLoc 위치에 Item Class로 액터 생성
					if (ItemClass && DropItemLoc)
					{
						// Get World Location: DropItemLoc Arrow의 월드 위치 가져오기
						FVector SpawnLocation = DropItemLoc->GetComponentLocation();
						FRotator SpawnRotation = DropItemLoc->GetComponentRotation();

						FActorSpawnParameters SpawnParams;
						SpawnParams.Instigator = this;


						if (HasAuthority())
						{
							//FActorSpawnParameters SpawnParams;
							//SpawnParams.Instigator = this;

							// 모든 클라이언트에서 아이템 스폰
							AMasterItem* SpawnedItem = GetWorld()->SpawnActor<AMasterItem>(
								ItemClass,
								SpawnLocation,
								SpawnRotation,
								SpawnParams
							);

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


							// 서버 : 클라이언트 것도 삭제
							//MultiDropItemFromSlot(ItemClass,SpawnLocation,SpawnRotation);
						}
						else
						{
							// 클라이언트  : 서버야 삭제 해줘
							ServerDropItemFromSlot(ItemClass,SpawnLocation,SpawnRotation, SelectedSlot);
						}

						/*AMasterItem* SpawnedItem = GetWorld()->SpawnActor<AMasterItem>(
							ItemClass,
							SpawnLocation,
							SpawnRotation,
							SpawnParams
						);

						if (SpawnedItem)
						{
							UE_LOG(LogTemp, Warning, TEXT("Item dropped: %s"), *SpawnedItem->GetName());
						}*/
					}

					
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

void ALSPlayer::ServerDropItemFromSlot_Implementation(TSubclassOf<AMasterItem> ItemClass, FVector SpawnLocation, FRotator SpawnRotation, int32 SlotIndex)
{
	//MultiDropItemFromSlot(ItemClass, SpawnLocation, SpawnRotation);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = this;

	//서버에서 아이템 스폰
	AMasterItem* SpawnedItem = GetWorld()->SpawnActor<AMasterItem>(
		ItemClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);


	//클라이언트 인벤토리 수정요청
	ClientDropItemFromSlot(SlotIndex);
}

void ALSPlayer::MultiDropItemFromSlot_Implementation(TSubclassOf<AMasterItem> ItemClass, FVector SpawnLocation, FRotator SpawnRotation)
{
	
}

void ALSPlayer::ClientDropItemFromSlot_Implementation(int32 SlotIndex)
{
	if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetController()))
	{
		if (ULSHUDWidget* HUD = PC->GetLSHUDWidget())
		{
			
			// Set Array Element: 해당 슬롯을 빈 상태로 만들기
			FItemDetails EmptySlot;
			EmptySlot.IsEmpty = true;
			EmptySlot.Item_Name = NAME_None;
			EmptySlot.Item_Icon = nullptr;
			EmptySlot.Item_Nature = EItemNature::IsConsumable;
			EmptySlot.Item_Class = nullptr;

			ItemInfoArray[SlotIndex] = EmptySlot;

			// Set Icon: HUD의 아이콘도 비우기 (Empty Item의 아이콘 = nullptr)
			UTexture2D* EmptyIcon = nullptr; // Empty Item의 Item Icon
			HUD->SetIcon(SlotIndex, EmptyIcon);


		}
	}
}

//
//void ALSPlayer::SpawnThrowableItem(const FItemDetails& ItemToThrow)
//{
//	LS_LOG(LogLS, Warning, TEXT("ALSPlayer::SpawnThrowableItem() called"));
//
//
//	TSubclassOf<AMasterItem> ItemClass = ItemToThrow.Item_Class;
//	if (ItemClass && DropItemLoc)
//	{
//		// 던지기 시작 위치 (플레이어 앞쪽)
//		FVector ThrowStartLocation = DropItemLoc->GetComponentLocation();
//		FRotator ThrowRotation = GetActorRotation();
//
//		FActorSpawnParameters SpawnParams;
//		SpawnParams.Instigator = this;
//
//
//
//		if (HasAuthority()) 
//		{
//
//			//스폰부분
//			AMasterItem* ThrownItem = GetWorld()->SpawnActor<AMasterItem>(
//				ItemClass,
//				ThrowStartLocation,
//				ThrowRotation,
//				SpawnParams
//			);
//
//
//			if (ThrownItem)
//			{
//				// 던져진 아이템으로 설정
//				ThrownItem->bIsThrown = true;
//
//				if (UStaticMeshComponent* ItemMesh = ThrownItem->FindComponentByClass<UStaticMeshComponent>())
//				{
//					// 물리 시뮬레이션 활성화
//					//ItemMesh->SetSimulatePhysics(true);
//					//ItemMesh->SetNotifyRigidBodyCollision(true); 
//
//					// Hit 이벤트를 위해 충돌 설정
//					//ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
//					//ItemMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
//
//					// 던지는 방향과 힘 
//					FVector ThrowDirection = GetActorForwardVector() + FVector(0, 0, 0.3f);
//					const float THROW_FORCE = 1000.0f;
//
//					// 임펄스 적용
//					ItemMesh->AddImpulse(ThrowDirection * THROW_FORCE, NAME_None, true);
//
//					// Hit 이벤트 바인딩 
//					ItemMesh->OnComponentHit.AddDynamic(ThrownItem, &AMasterItem::OnItemHit);
//
//					LS_LOG(LogLS, Warning, TEXT("Throwable item spawned: %s"), *ThrownItem->GetName());
//				}
//			}
//			else
//			{
//				LS_LOG(LogLS, Error, TEXT("Failed to spawn throwable item"));
//			}
//
//		}
//		else
//		{
//			ServerSpawnThrowableItem();
//		}
//
//	}
//	else
//	{
//		LS_LOG(LogLS, Error, TEXT("ItemClass or DropItemLoc is null"));
//	}
//}

//void ALSPlayer::ServerSpawnThrowableItem_Implementation(const FItemDetails& ItemToThrow)
//{
//
//	
//
//}

void ALSPlayer::SpawnThrowableItem(const FItemDetails& ItemToThrow)
{
	

	if (!HasAuthority())
	{
		
		ServerSpawnThrowableItem(ItemToThrow);
		return;
	}

	TSubclassOf<AMasterItem> ItemClass = ItemToThrow.Item_Class;
	if (!ItemClass || !DropItemLoc)
	{
		LS_LOG(LogLS, Error, TEXT("ItemClass or DropItemLoc is null"));
		return;
	}

	FVector ThrowStartLocation = DropItemLoc->GetComponentLocation();
	FRotator ThrowRotation = GetActorRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = this;

	AMasterItem* ThrownItem = GetWorld()->SpawnActor<AMasterItem>(ItemClass,ThrowStartLocation,ThrowRotation,SpawnParams);

	if (ThrownItem)
	{
		ThrownItem->bIsThrown = true;

		if (UStaticMeshComponent* ItemMesh = ThrownItem->FindComponentByClass<UStaticMeshComponent>())
		{
			ItemMesh->SetSimulatePhysics(true);
			ItemMesh->SetNotifyRigidBodyCollision(true);
			ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			ItemMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

			FVector ThrowDirection = GetActorForwardVector() + FVector(0, 0, 0.3f);
			const float THROW_FORCE = 300.0f;
			ItemMesh->AddImpulse(ThrowDirection * THROW_FORCE, NAME_None, true);

			ItemMesh->OnComponentHit.AddDynamic(ThrownItem, &AMasterItem::OnItemHit);

			LS_LOG(LogLS, Warning, TEXT("Throwable item spawned: %s"), *ThrownItem->GetName());
		}
		MultiSpawnThrowableItem(ItemToThrow);
	}

	else
	{
		LS_LOG(LogLS, Error, TEXT("Failed to spawn throwable item"));
	}
}



void ALSPlayer::ServerSpawnThrowableItem_Implementation(const FItemDetails& ItemToThrow)
{
	SpawnThrowableItem(ItemToThrow);
	
}

void ALSPlayer::MultiSpawnThrowableItem_Implementation(const FItemDetails& ItemToThrow)
{
	ULSPlayerIJaeAnimInstance* AnimInstance = Cast<ULSPlayerIJaeAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->ThrowingAnim();
		UE_LOG(LogTemp, Warning, TEXT("Player throwing ANIMATION"));
	}
	//return;
}

void ALSPlayer::ClientSpawnThrowableItem_Implementation(int32 SlotIndex)
{
}
