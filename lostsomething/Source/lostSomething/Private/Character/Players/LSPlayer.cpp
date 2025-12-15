// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Players/LSPlayer.h"
#include "lostSomething.h"
#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Blueprint/UserWidget.h"
#include "DrawDebugHelpers.h"
#include "Engine/DamageEvents.h"
#include "Character/Players/LSPlayerController.h"
#include "Character/Players/LSPlayerSiJae.h"
#include "Character/Components/LSHpComponent.h"
#include "Character/UI/LSHUDWidget.h"
#include "Character/UI/LSHpWidget.h"
#include "Character/UI/LSDeathWidget.h" 
#include "Components/PrimitiveComponent.h"
#include "Character/Animation/LSPlayerSiJaeAnimInstance.h"
#include "Character/Animation/LSPlayerIJaeAnimInstance.h"
#include "Character/Item/MasterItem.h"
#include "Interface/LSInteractionInterface.h"
#include "Interface/LSStartGameInterface.h"
#include "Game/LSGameInstance.h"
#include "Game/LSNetworkPosition.h"
#include "Interface/LSCharacterChoiceInterface.h"
#include "Interface/LSTakeDamageInterface.h"
#include "Engine/AssetManager.h"
#include "Interaction/LSInteractionScriptData.h"
#include <Game/LSGameMode.h>

/*******************

아래의 주석들 확인해보고 없애주라

********************/

//void ALSPlayer::PickItem(const FItemDetails& PickedItemInfo)
//{
//	// Pick Item In Slot 함수 호출
//	//PickItemInSlot(PickedItemInfo);
//
//}
//	// 아이템이 감지되지 않았거나 MasterItem이 아닌 경우
//	// 현재 선택된 슬롯의 아이템을 드롭
//	LS_LOG(LogLS, Warning, TEXT("No valid item found - attempting to drop current slot item"));
//	DropItemFromSlot();
//	DrawColor = FColor::Red;
//	// 디버그 라인
//	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
//	float CapsuleHalfHeight = PickupRange * 0.5f;
//	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, PickupRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);
//}



void ALSPlayer::ChangeSlot(int32 NewSlot)
{


	if (MaxSlots <= 0 || ItemInfoArray.Num() == 0)
	{
		return;
	}

	// 범위를 벗어나면
	if (NewSlot < 0)
	{
		NewSlot = 4;
		//NewSlot = MaxSlots - 1; 
	}
	else if (NewSlot >= MaxSlots)
	{
		NewSlot = 0; 
	}

	SelectSlot(NewSlot);

}

void ALSPlayer::OnMouseWheelUp(const FInputActionValue& Value)
{
	int32 NewSlot = SelectedSlot + 1;
	ChangeSlot(NewSlot);
}
void ALSPlayer::OnMouseWheelDown(const FInputActionValue& Value)
{
	int32 NewSlot = SelectedSlot - 1;
	ChangeSlot(NewSlot);
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





/*******************

 Main Section - 이 위로 함수 만들지 말기!!!!!!!!!

********************/

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

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	//Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	//Mesh1P->SetOnlyOwnerSee(true);
	//Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	//Mesh1P->bCastDynamicShadow = false;
	//Mesh1P->CastShadow = false;
	////Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	//Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	//컨트롤러 회전시 회전 x
	//bUseControllerRotationPitch = false;

	//// 이동 방향에 따라 캐릭터 회전
	////GetCharacterMovement()->bOrientRotationToMovement = true;
	//bUseControllerRotationYaw = true; //수정

	//bUseControllerRotationRoll = false;

	CombinedMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CombinedMeshComp"));
	CombinedMeshComp->SetupAttachment(RootComponent);
	CombinedMeshComp->SetVisibility(false);
	CombinedMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetMesh());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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


	bIsCombining = false;


	//weapon
	EquippedWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EquippedWeaponMesh"));
	EquippedWeaponMesh->SetupAttachment(GetMesh(), WeaponSocketName); // hand_rSocket 같은 거

	
	EquippedWeaponMesh->SetSimulatePhysics(false);
	EquippedWeaponMesh->SetEnableGravity(false);
	EquippedWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	EquippedWeaponMesh->SetVisibility(false);

}

void ALSPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//Script Section
	UAssetManager& Manager = UAssetManager::Get();

	TArray<FPrimaryAssetId> Assets;
	Manager.GetPrimaryAssetIdList(TEXT("LSScriptData"), Assets);

	if (0 < Assets.Num())
	{
		for (const FPrimaryAssetId& AssetId : Assets)
		{
			if (AssetId.PrimaryAssetName == TEXT("LSAimScript"))
			{
				FSoftObjectPtr AssetPtr(Manager.GetPrimaryAssetPath(AssetId));
				//LS_LOG(LogLS, Log, TEXT("Found TestItem at path: %s"), *AssetPtr.ToString());

				if (AssetPtr.IsPending())
				{
					AssetPtr.LoadSynchronous();
				}
				AimScriptData = Cast<ULSInteractionScriptData>(AssetPtr.Get());
				ensure(AimScriptData);
				break;
			}
		}
	}
	else
	{
		LS_LOG(LogLSls, Error, TEXT("AimScriptData Not Found"));
	}
}



void ALSPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 맵을 떠나기 전에 인벤토리 저장
	SaveInventoryToGameInstance();

	Super::EndPlay(EndPlayReason);
}


// Called when the game starts or when spawned
void ALSPlayer::BeginPlay()
{
	Super::BeginPlay();


	UStaticMeshComponent* MeshComp = FindComponentByClass<UStaticMeshComponent>();
	if (MeshComp)
	{

		UMaterialInterface* OverlayMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Players/Materials/Outline_MAT.Outline_MAT"));
		if (OverlayMat)
		{
			MeshComp->SetOverlayMaterial(OverlayMat);
		}
	}

	UStaticMeshComponent* MeshComp2 = FindComponentByClass<UStaticMeshComponent>();
	if (MeshComp2)
	{

		UMaterialInterface* OverlayMat2 = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Players/Materials/OutlineRemove_MAT.OutlineRemove_MAT"));
		if (OverlayMat2)
		{
			MeshComp2->SetOverlayMaterial(OverlayMat2);
		}
	}

	if (HasAuthority())
	{
		ILSStartGameInterface* GameMode = Cast<ILSStartGameInterface>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameMode)
		{
			GameMode->GetOnStartGameDelegate().AddUObject(this, &ALSPlayer::MulticastRPCStartGame);
		}
	}

}

void ALSPlayer::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);

	if (bIsCombining)
	{

		if (CharacterChoice == ELSCharacterChoice::IJae)
		{
			// 이제(IJae) 캐릭터가 밀리고 있는 상태라면 이동 처리
			HandleCombinedIJaeMovement();
		}

		CheckCombineDistance();
	}

	PerformLineTrace();
	if (bThrowPreview)
	{
		UpdateThrowPreview();
	}

}

// 리플리케이션 설정
void ALSPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALSPlayer, bisSequencePlaying);

	DOREPLIFETIME(ALSPlayer, CurrentHp);
	DOREPLIFETIME(ALSPlayer, bIsDead);

	DOREPLIFETIME(ALSPlayer, CurrentDectectActor);
	DOREPLIFETIME(ALSPlayer, TickDectectResultColor);
	DOREPLIFETIME(ALSPlayer, AimScript);
	DOREPLIFETIME(ALSPlayer, CallElevatorParams);

	DOREPLIFETIME(ALSPlayer, bIsCombining);
	DOREPLIFETIME(ALSPlayer, CharacterChoice);
	DOREPLIFETIME(ALSPlayer, PusherSiJaeCharacter);
	DOREPLIFETIME(ALSPlayer, PushedIJaeCharacter);
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

		//EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ALSPlayer::Attack);
		//EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ALSPlayer::StartThrowPreview);
		//EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &ALSPlayer::EndThrowPreview,true);
		EnhancedInputComponent->BindAction(MouseWheelUpAction, ETriggerEvent::Triggered, this, &ALSPlayer::OnMouseWheelUp);
		EnhancedInputComponent->BindAction(MouseWheelDownAction, ETriggerEvent::Triggered, this, &ALSPlayer::OnMouseWheelDown);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALSPlayer::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ALSPlayer::Look);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ALSPlayer::Interaction);
		EnhancedInputComponent->BindAction(DropAction, ETriggerEvent::Triggered, this, &ALSPlayer::Drop);
		EnhancedInputComponent->BindAction(MenuAction, ETriggerEvent::Triggered, this, &ALSPlayer::Menu);
		EnhancedInputComponent->BindAction(SelectSlot1Action, ETriggerEvent::Triggered, this, &ALSPlayer::OnSelectSlot1);
		EnhancedInputComponent->BindAction(SelectSlot2Action, ETriggerEvent::Triggered, this, &ALSPlayer::OnSelectSlot2);
		EnhancedInputComponent->BindAction(SelectSlot3Action, ETriggerEvent::Triggered, this, &ALSPlayer::OnSelectSlot3);
		EnhancedInputComponent->BindAction(SelectSlot4Action, ETriggerEvent::Triggered, this, &ALSPlayer::OnSelectSlot4);
		EnhancedInputComponent->BindAction(SelectSlot5Action, ETriggerEvent::Triggered, this, &ALSPlayer::OnSelectSlot5);
		EnhancedInputComponent->BindAction(VoiceAction, ETriggerEvent::Started, this, &ALSPlayer::VoiceStart);
		EnhancedInputComponent->BindAction(VoiceAction, ETriggerEvent::Completed, this, &ALSPlayer::VoiceStop);
	}
}



/*******************

Start Game Section

********************/

void ALSPlayer::StartGame()
{
	if (HpComponent)
	{
		HpComponent->OnHpChanged.AddDynamic(this, &ALSPlayer::OnHpChanged);
		HpComponent->OnHpZero.AddDynamic(this, &ALSPlayer::OnHpReachedZero);
		if (IsLocallyControlled())
		{
			OnHpChanged(HpComponent->GetHp());
		}
	}

	LoadInventoryFromGameInstance();

	//InitializeInventory();

	if (HasAuthority())
	{
		if (!GetController()) return;

		CharacterChoice = Cast<ILSCharacterChoiceInterface>(GetController())->GetCharacterChoice();
		if (CharacterChoice == ELSCharacterChoice::SiJae)
		{
			PusherSiJaeCharacter = this;
		}
		else
		{
			PushedIJaeCharacter = this;
		}

		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALSPlayer::StaticClass(), FoundActors);

		for (AActor* Actor : FoundActors)
		{
			if (ALSPlayer* LSPlayer = Cast<ALSPlayer>(Actor))
			{
				if (LSPlayer->GetCharacterChoice() != CharacterChoice)
				{
					if (CharacterChoice == ELSCharacterChoice::SiJae)
					{
						PushedIJaeCharacter = LSPlayer;
					}
					else
					{
						PusherSiJaeCharacter = LSPlayer;
					}
				}
			}
		}

		CurrentCombineStamina = MaxCombineStamina;
	}
}

void ALSPlayer::MulticastRPCStartGame_Implementation()
{
	StartGame();
}



/*******************

Basic Section

********************/

void ALSPlayer::Move(const FInputActionValue& Value)
{
	if (bIsDead) return;

	// 로컬에서 제어 중인지 확인
	if (!IsLocallyControlled())
		return;

	// 이제(IJae) 캐릭터가 밀리고 있다면 입력 무시
	if (bIsCombining)
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



/*******************

Interaction Section

********************/

void ALSPlayer::PerformLineTrace()
{
	if (!FirstPersonCameraComponent) return;

	if (!IsLocallyControlled()) return;

	if (!AimScriptData)
	{
		LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("No AimScript!!"));
		return;
	}

	FVector StartLocation = FirstPersonCameraComponent->GetComponentLocation();
	FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();
	FVector EndLocation = StartLocation + (ForwardVector * 300.0f);


	if (bIsCombining && CharacterChoice == ELSCharacterChoice::SiJae)
	{
		CurrentDectectActor = nullptr;
		TickDectectResultColor = FColor::Yellow;
		AimScript = AimScriptData->GetInteractionScripts(ELSInteractionEnum::Quest0)[2];
	}
	else
	{
		FHitResult HitResult;
		bool bHit = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			StartLocation,
			EndLocation,
			//ECC_Visibility
			ECC_GameTraceChannel1
		);

		if (bHit && HitResult.GetActor())
		{
			if (ILSInteractionInterface* InteractionActor = Cast<ILSInteractionInterface>(HitResult.GetActor()))
			{
				CurrentDectectActor = HitResult.GetActor();
				TickDectectResultColor = FColor::Green;
				AimScript = AimScriptData->GetInteractionScripts(ELSInteractionEnum::Quest0)[0];
			}
			else if (ALSPlayer* CharacterActor = Cast<ALSPlayer>(HitResult.GetActor()))
			{
				if (Cast<ILSCharacterChoiceInterface>(GetController())->GetCharacterChoice() == ELSCharacterChoice::SiJae && IsPlayerNear())
				{
					CurrentDectectActor = HitResult.GetActor();
					TickDectectResultColor = FColor::Green;
					AimScript = AimScriptData->GetInteractionScripts(ELSInteractionEnum::Quest0)[1];
				
				}
				else if (Cast<ILSCharacterChoiceInterface>(GetController())->GetCharacterChoice() == ELSCharacterChoice::SiJae && !IsPlayerNear())
				
				{
					RemoveOverlayMaterialToActor(CurrentDectectActor, ItemOverlayMaterial2);
					TickDectectResultColor = FColor::Black;
					AimScript = AimScriptData->GetInteractionScripts(ELSInteractionEnum::Quest0)[3];
					CurrentDectectActor = nullptr;
				}
				else
				{
					RemoveOverlayMaterialToActor(CurrentDectectActor, ItemOverlayMaterial2);
					TickDectectResultColor = FColor::Black;
					AimScript = "";
					CurrentDectectActor = nullptr;
				}
			}
			else if (IsActorName(HitResult.GetActor(), "BP_LSElevatorStyle2"))
			{
				CurrentDectectActor = HitResult.GetActor();
				TickDectectResultColor = FColor::Green;
				AimScript = AimScriptData->GetInteractionScripts(ELSInteractionEnum::Quest0)[4];

				FCallElevatorParams Params;
				//Params.Target = HitResult.GetActor();
				Params.HitComponent = HitResult.GetComponent();
				Params.InteractingActor = this;
				Params.ObjectTracingTheLine = GetCapsuleComponent()->GetComponentLocation();
				CallElevatorParams = Params;
			}
			else if (ILSItemInterface* LSItem = Cast< ILSItemInterface>(HitResult.GetActor()))
			{
				CurrentDectectActor = HitResult.GetActor();
				TickDectectResultColor = FColor::Green;

				AimScript = AimScriptData->GetInteractionScripts(ELSInteractionEnum::Quest0)[5];

				ApplyOverlayMaterialToActor(CurrentDectectActor, ItemOverlayMaterial);

			}
			else
			{
				RemoveOverlayMaterialToActor(CurrentDectectActor, ItemOverlayMaterial2);
				TickDectectResultColor = FColor::Black;
				AimScript = "";
				CurrentDectectActor = nullptr;
			}
		}
		else
		{
			RemoveOverlayMaterialToActor(CurrentDectectActor, ItemOverlayMaterial2);
			TickDectectResultColor = FColor::Red;
			AimScript = "";
			CurrentDectectActor = nullptr;

		}
	}

	//#if ENABLE_DRAW_DEBUG
	//	DrawDebugLine(GetWorld(), StartLocation, EndLocation, TickDectectResultColor, false, 0.0f, 0, 0.2f);
	//#endif

	ALSPlayerController* PC = Cast<ALSPlayerController>(GetController());
	if (PC)
	{
		PC->UpdateAim(AimScript);
	}
}

void ALSPlayer::Interaction()
{
	if (bIsDead) return;

	// 로컬 컨트롤러가 있는지 확인
	if (!IsLocallyControlled())
	{
		return;
	}

	if (CharacterChoice == ELSCharacterChoice::SiJae && bIsCombining)
	{
		if (HasAuthority())
		{
			ChangeCombineState(false);
		}
		else
		{
			ServerRPCChangeCombineState(false);
		}
		return;
	}

	if (CurrentDectectActor == nullptr)
	{
		return;
	}

	if (ILSInteractionInterface* InteractionActor = Cast<ILSInteractionInterface>(CurrentDectectActor))
	{
		//액터 상호작용
		InteractionActor->InteractionProcess(Cast<APlayerController>(GetController()));
	}
	else if (ALSPlayer* LSPlayer = Cast<ALSPlayer>(CurrentDectectActor))
	{
		if (CharacterChoice == ELSCharacterChoice::SiJae && IsPlayerNear())
		{
			if (HasAuthority())
			{
				ChangeCombineState(true);
			}
			else
			{
				ServerRPCChangeCombineState(true);
			}
		}
	}
	else if (IsActorName(CurrentDectectActor, "BP_LSElevatorStyle2"))
	{
		CallElevator(CurrentDectectActor);
	}
	else if (ILSItemInterface* LSItem = Cast< ILSItemInterface>(CurrentDectectActor))
	{
		PickUpCore();
	}
}

void ALSPlayer::CallElevator(AActor* InActor)
{
	if (!InActor) return;

	if (UFunction* TransferFunc = InActor->FindFunction(FName("TrransferObjectName")))
	{
		InActor->ProcessEvent(TransferFunc, &CallElevatorParams);
	}
	else
	{
		LS_LOG(LogLS, Error, TEXT("%s"), TEXT("No TrransferObjectName"));
		LS_LOG(LogLS, Warning, TEXT("Available functions in %s:"), *InActor->GetName());
		for (TFieldIterator<UFunction> FuncIt(InActor->GetClass()); FuncIt; ++FuncIt)
		{
			UFunction* Function = *FuncIt;
			LS_LOG(LogLS, Warning, TEXT("- %s"), *Function->GetName());
		}
	}
}



/*******************

 Overlay Section

********************/

void ALSPlayer::ApplyOverlayMaterialToActor(AActor* TargetActor, UMaterialInterface* OverlayMaterial)
{
	if (!TargetActor || !OverlayMaterial)
		return;

	if (UStaticMeshComponent* MeshComp = TargetActor->FindComponentByClass<UStaticMeshComponent>())
	{
		MeshComp->SetOverlayMaterial(OverlayMaterial);
		return;
	}
}

void ALSPlayer::RemoveOverlayMaterialToActor(AActor* TargetActor, UMaterialInterface* OverlayMaterial2)
{
	if (!TargetActor || !OverlayMaterial2)
		return;

	if (UStaticMeshComponent* MeshComp = TargetActor->FindComponentByClass<UStaticMeshComponent>())
	{
		MeshComp->SetOverlayMaterial(OverlayMaterial2);
		return;
	}
}



/*******************

 Damage Section

********************/

float ALSPlayer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (bisSequencePlaying) return DamageAmount;

	if (HasAuthority())
	{
		ApplyDamage(DamageAmount);
	}
	else
	{
		LS_LOG(LogLS, Error, TEXT("%s"), TEXT("This is Client Character"));
	}

	if (bIsCombining)
	{
		if (HasAuthority())
		{
			ChangeCombineState(false);
		}
		else
		{
			ServerRPCChangeCombineState(false);
		}
	}

	return DamageAmount;
}

void ALSPlayer::ApplyDamage(float DamageAmount)
{
	if (bIsDead) return;

	if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetController()))
	{
		PC->ShowBloodWidget();
		//DisableInput(PC);
	}

	//if (IsLocallyControlled())
	//{
	//	if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetController()))
	//	{
	//		PC->ShowBloodWidget();
	//		//DisableInput(PC);
	//	}
	//}

	if (HpComponent)
	{
		CurrentHp -= DamageAmount;
		HpComponent->SetHp(CurrentHp);
		//LS_LOG(LogLS, Log, TEXT("ApplyDamage SetHp Called"));
	}

	if (CurrentHp <= 0)
	{
		//LS_LOG(LogLS, Error, TEXT("hp is zero call Die"));
		Die();
		return;
	}
	else
	{
		LS_LOG(LogLS, Error, TEXT("No HpComponent"));
	}
}

void ALSPlayer::OnHpChanged(float NewHp)
{
	//UE_LOG(LogTemp, Warning, TEXT("ALSPlayer::OnHpChanged called with HP: %.1f"), NewHp);
	//LS_LOG(LogLS, Warning, TEXT("ALSPlayer::OnHpChanged called with HP: %.1f"), NewHp);
	/*ULSPlayerSiJaeAnimInstance* AnimInstance = Cast<ULSPlayerSiJaeAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->HitAnim();

	}*/
	// 플레이어 컨트롤러 가져오기
	//ALSPlayerController* LSController = Cast<ALSPlayerController>(GetController());
	//if (LSController && LSController->GetLSHUDWidget())
	//{
	//	// HUD 위젯의 HP 바 업데이트
	//	LSController->GetLSHUDWidget()->UpdateHpBar(NewHp);
	//	LSController->GetLSHpWidget()->UpdateHpBar(NewHp);
	//	UE_LOG(LogTemp, Warning, TEXT("ALSPlayer :: Updated HUD with new HP: %.1f"), NewHp);
	//}
	//hpwidget따로
	ALSPlayerController* LSController = Cast<ALSPlayerController>(GetController());
	if (LSController && LSController->GetLSHpWidget())
	{
		// Hp 위젯의 HP 바 업데이트
		LSController->GetLSHpWidget()->UpdateHpBar(NewHp);
		//UE_LOG(LogTemp, Warning, TEXT("ALSPlayer :: Updated Hhp wbp with new HP: %.1f"), NewHp);
	}
	else
	{
		if (!LSController)
			UE_LOG(LogTemp, Error, TEXT("LSPLAYER :: LSPlayerController is null"));
	}
}

void ALSPlayer::OnRep_CurrentHp()
{
	//LS_LOG(LogLS, Log, TEXT("CurrentHp : %f"), CurrentHp);
	HpComponent->SetHp(CurrentHp);
}

void ALSPlayer::MeshHide()
{
	if (GetMesh())
	{
		GetMesh()->SetVisibility(false);
		//UE_LOG(LogTemp, Warning, TEXT("Player mesh hidden (delayed)"));
	}
}

void ALSPlayer::Die()
{
	if (bIsDead) return; // 이미 죽었으면 리턴

	bIsDead = true;
	//UE_LOG(LogTemp, Warning, TEXT("Player died"));

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
		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &ALSPlayer::Respawn, 5.0f, false);
	}


	if (!HasAuthority())
	{
		ServerDie();

	}
}

void ALSPlayer::Respawn()
{
	if (!bIsDead) return; // 이미 살아있으면 리턴

	bIsDead = false;
	//UE_LOG(LogTemp, Warning, TEXT("Player respawned"));

	ALSGameMode* GameMode = Cast<ALSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
		if (UGameplayStatics::GetCurrentLevelName(GetWorld()) == "BossMap")
		{
			GameMode->TransferPlayerLocation(FVector(-145.421501, 487.230132, 114.334673), FVector(84.450062, 487.230132, 114.334674));
		}
	}

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

void ALSPlayer::ServerDie_Implementation()
{
	MultiDie();

}


void ALSPlayer::MultiDie_Implementation()
{
	//UE_LOG(LogTemp, Warning, TEXT("MulticastPlayerDied called"));
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



/*******************

Voice Section

********************/

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



/*******************

Item	Section

********************/

void ALSPlayer::PickUpCore()
{
	if (ALSPlayerSiJae* SiJae = Cast<ALSPlayerSiJae>(this))
	{
		//SiJae->WeaponPickUp();
		//LS_LOG(LogLS, Warning, TEXT("ALSPlayer::weaponpickup() called"));

	}

	if (bIsDead) return;

	FHitResult OutHitResult;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(PickUp), false, this);
	const float PickupRange = 200.0f;
	const float PickupRadius = 100.0f;
	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * PickupRange;
	//FColor DrawColor;

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
		//LS_LOG(LogLS, Warning, TEXT("HIT DETECTED: %s"), HitActor ? *HitActor->GetName() : TEXT("Unknown"));

		// MasterItem 픽업 처리
		AMasterItem* HitItem = Cast<AMasterItem>(HitActor);
		if (HitItem)
		{
			//아이템 중첩상태에서 pick (인벤차있는상태에서 콜리전 겹쳐서 주우면)
			if (!CurrentSlotItem.IsEmpty)
			{
				//LS_LOG(LogLS, Warning, TEXT("Slot %d is occupied - only dropping existing item"), CurrentSelectedSlot);
				//DropItemFromSlot();
				//DrawColor = FColor::Orange;

				// 디버그 라인
				FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
				float CapsuleHalfHeight = PickupRange * 0.5f;
				//DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, PickupRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);
				return; // 드롭만 하고 함수 종료
			}
		}

		if (!HitItem)
		{
			//LS_LOG(LogLS, Warning, TEXT("HitActor is not a MasterItem - ignoring"));
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
		//LS_LOG(LogLS, Warning, TEXT("Item picked up and destroyed: %s"), *HitItem->GetName());
		//DrawColor = FColor::Green;
		return; // 픽업했으면 함수 종료
	}
	else
	{
		if (!CurrentSlotItem.IsEmpty)
		{
			//LS_LOG(LogLS, Warning, TEXT("Slot %d is occupied - only dropping existing item"), CurrentSelectedSlot);
			//DropItemFromSlot();
			//DrawColor = FColor::Orange;

			//// 디버그 라인
			//FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
			//float CapsuleHalfHeight = PickupRange * 0.5f;
			//DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, PickupRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);

		}
		//DrawColor = FColor::Yellow;
	}
}

void ALSPlayer::Drop()
{
	int32 CurrentSelectedSlot = SelectedSlot;
	FItemDetails CurrentSlotItem = ItemInfoArray[CurrentSelectedSlot];
	bool bCurrentSlotIsEmpty = CurrentSlotItem.IsEmpty;

	//UE_LOG(LogTemp, Warning, TEXT("Slot %d IsEmpty: %s"),
	//CurrentSelectedSlot,
	//bCurrentSlotIsEmpty ? TEXT("true") : TEXT("false"));

	//PickUpCore();
	if (!CurrentSlotItem.IsEmpty)
	{
		//LS_LOG(LogLS, Warning, TEXT("Slot %d is occupied - only dropping existing item"), CurrentSelectedSlot);
		DropItemFromSlot();

		//UE_LOG(LogTemp, Warning, TEXT("Slot %d IsEmpty: %s"),
			//CurrentSelectedSlot,
			//bCurrentSlotIsEmpty ? TEXT("true") : TEXT("false"));
	}
	else
	{
		//PickUpCore();
		//UE_LOG(LogTemp, Warning, TEXT("Slot %d IsEmpty: %s"),
			//CurrentSelectedSlot,
			//sbCurrentSlotIsEmpty ? TEXT("true") : TEXT("false"));
	}
}

void ALSPlayer::DropItemFromSlot()
{
	//UE_LOG(LogTemp, Warning, TEXT("ALSPlayer::DropItemFromSlot() called"));

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
							/*AMasterItem* SpawnedItem = GetWorld()->SpawnActor<AMasterItem>(
								ItemClass,
								SpawnLocation,
								SpawnRotation,
								SpawnParams
							);*/

							AMasterItem* SpawnedItem = GetWorld()->SpawnActor<AMasterItem>(
								ItemClass,
								//위로 조금 띄워서 자연스럽게
								DropItemLoc->GetComponentLocation() + FVector(0, 0, 20.f),
								DropItemLoc->GetComponentRotation()

							);

							RefreshWeaponEquipFromCurrentSlot();

							if (SpawnedItem)
							{
								if (UStaticMeshComponent* ItemMesh = SpawnedItem->FindComponentByClass<UStaticMeshComponent>())
								{
									ItemMesh->SetSimulatePhysics(true);      // 물리 활성화
									ItemMesh->SetEnableGravity(true);        // 중력 적용
									ItemMesh->AddImpulse(FVector(0, 0, 100.f)); // 위로 살짝 힘
								}
							}

							RefreshWeaponEquipFromCurrentSlot();

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
							RefreshWeaponEquipFromCurrentSlot();
							//RefreshWeaponEquipFromCurrentSlot();
							// 서버 : 클라이언트 것도 삭제
							//MultiDropItemFromSlot(ItemClass,SpawnLocation,SpawnRotation);
							SaveInventoryToGameInstance();
						}
						else
						{
							// 클라이언트  : 서버야 삭제 해줘
							ServerDropItemFromSlot(ItemClass, SpawnLocation, SpawnRotation, SelectedSlot);
							RefreshWeaponEquipFromCurrentSlot();
							SaveInventoryToGameInstance();
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
					//UE_LOG(LogTemp, Warning, TEXT("Slot %d cleared, item dropped, and icon updated"), CurrentSelectedSlot);
				}
			}
		}
	}
}

void ALSPlayer::PickItemInSlot(const FItemDetails& PickedItem)
{
	//UE_LOG(LogTemp, Warning, TEXT("ALSPlayer::PickItemInSlot() called"));

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

					RefreshWeaponEquipFromCurrentSlot();
					//UE_LOG(LogTemp, Warning, TEXT("Item stored in slot %d"), CurrentSelectedSlot);
					SaveInventoryToGameInstance();
				
				}
				else  // 이미 차있는 슬롯인 경우
				{
					//UE_LOG(LogTemp, Warning, TEXT("Slot %d is occupied - dropping existing item. Try picking up again."), CurrentSelectedSlot);

					// 기존 아이템만 드롭하고 끝 (새 아이템은 픽업하지 않음)
					DropItemFromSlot();
					RefreshWeaponEquipFromCurrentSlot();
				}
			}
		}
	}
}

void ALSPlayer::InitializeInventory()
{
	//UE_LOG(LogTemp, Warning, TEXT("ALSPlayer::InitializeInventory() called"));

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

	//UE_LOG(LogTemp, Warning, TEXT("Inventory initialized with %d empty slots"), MaxSlots);
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

void ALSPlayer::SaveInventoryToGameInstance()
{

	if (!IsLocallyControlled())
		return;

	if (ULSGameInstance* GI = GetWorld()->GetGameInstance<ULSGameInstance>())
	{
		const ELSNetworkPosition Position = GetNetworkPositionForInventory();
		GI->SaveInventory(Position, ItemInfoArray, SelectedSlot);
	}


	// 서버도 아니고, 로컬도 아니면 패스
	/*if (!HasAuthority() && !IsLocallyControlled())
	{
		return;
	}

	if (ULSGameInstance* LSGameInstance = GetWorld()->GetGameInstance<ULSGameInstance>())
	{
		const ELSNetworkPosition Position = GetNetworkPositionForInventory();

		LSGameInstance->SaveInventory(Position, ItemInfoArray, SelectedSlot);

		UE_LOG(LogTemp, Log, TEXT("ALSPlayer::SaveInventoryToGameInstance - Pos:%d, Slots:%d, Selected:%d"),
			static_cast<int32>(Position),
			ItemInfoArray.Num(),
			SelectedSlot);
	}*/
}

void ALSPlayer::LoadInventoryFromGameInstance()
{
	
	InitializeInventory();


	/*if (!HasAuthority() && !IsLocallyControlled())
	{
		return;
	}*/

	if (ULSGameInstance* LSGameInstance = GetWorld()->GetGameInstance<ULSGameInstance>())
	{
		const ELSNetworkPosition Position = GetNetworkPositionForInventory();

		TArray<FItemDetails> LoadedItems;
		int32 LoadedSelectedSlot = 0;

		if (LSGameInstance->LoadInventory(Position, LoadedItems, LoadedSelectedSlot))
		{
			// 슬롯 배열 / 선택 슬롯 덮어쓰기
			ItemInfoArray = LoadedItems;
			MaxSlots = ItemInfoArray.Num();
			SelectedSlot = LoadedSelectedSlot;

			// HUD 아이콘 갱신
			if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetController()))
			{
				if (ULSHUDWidget* HUD = PC->GetLSHUDWidget())
				{
					for (int32 i = 0; i < ItemInfoArray.Num(); ++i)
					{
						const FItemDetails& SlotItem = ItemInfoArray[i];
						UTexture2D* Icon = nullptr;

						if (!SlotItem.IsEmpty)
						{
							Icon = SlotItem.Item_Icon.LoadSynchronous();
						}

						HUD->SetIcon(i, Icon);
					}

					// 선택 슬롯 테두리 갱신
					HUD->UpdateSlotBorderColors(SelectedSlot);
				}
			}

		
			RefreshWeaponEquipFromCurrentSlot();

			UE_LOG(LogTemp, Log, TEXT("ALSPlayer::LoadInventoryFromGameInstance - Pos:%d, LoadedSlots:%d, Selected:%d"),
				static_cast<int32>(Position),
				ItemInfoArray.Num(),
				SelectedSlot);
		}
		else
		{
			// 저장된 게 없으면 그냥 초기화된 빈 인벤토리 유지
			UE_LOG(LogTemp, Log, TEXT("ALSPlayer::LoadInventoryFromGameInstance - No saved data (Pos:%d)"),
				static_cast<int32>(Position));
		}
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
				//UE_LOG(LogTemp, Warning, TEXT("Direct slot selection: %d"), SlotIndex);

				// 슬롯 색상 업데이트
				//LSHUDWidget->UpdateSlotBorderColors();

				// 플레이어 컨트롤러 가져오기
				ALSPlayerController* LSController = Cast<ALSPlayerController>(GetController());
				if (LSController && LSController->GetLSHUDWidget())
				{
					LSController->GetLSHUDWidget()->UpdateSlotBorderColors(SelectedSlot);
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

	//무기인지 아닌지 체크용
	RefreshWeaponEquipFromCurrentSlot();
}

void ALSPlayer::OnSelectSlot1()
{
	//UE_LOG(LogTemp, Warning, TEXT("Player: Slot 1 key pressed"));
	SelectSlot(0);

	//if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetController()))
	//{
	//	SelectSlot(0);
	//}
}

void ALSPlayer::OnSelectSlot2()
{
	//UE_LOG(LogTemp, Warning, TEXT("Player: Slot 2 key pressed"));

	if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetController()))
	{
		SelectSlot(1);
	}
}

void ALSPlayer::OnSelectSlot3()
{
	//UE_LOG(LogTemp, Warning, TEXT("Player: Slot 3 key pressed"));

	if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetController()))
	{
		SelectSlot(2);
	}
}

void ALSPlayer::OnSelectSlot4()
{
	//UE_LOG(LogTemp, Warning, TEXT("Player: Slot 4 key pressed"));

	if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetController()))
	{
		SelectSlot(3);
	}
}

void ALSPlayer::OnSelectSlot5()
{
	//UE_LOG(LogTemp, Warning, TEXT("Player: Slot 5 key pressed"));

	if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetController()))
	{
		SelectSlot(4);
	}
}

void ALSPlayer::ServerPickUpCore_Implementation(AMasterItem* TargetItem)
{
	FItemDetails ItemData = TargetItem->GetItemInfo();
	TargetItem->Destroy();
	//MultiPickUpCore(TargetItem);
	ClientPickUpCore(ItemData);
}

void ALSPlayer::MultiPickUpCore_Implementation(AActor* TargetItem)
{
	ULSPlayerSiJaeAnimInstance* AnimInstance = Cast<ULSPlayerSiJaeAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		//AnimInstance->SetPickUpAnim();
		//UE_LOG(LogTemp, Warning, TEXT("Player Picking ANIMATION SIJAE"));
	}
}

void ALSPlayer::ClientPickUpCore_Implementation(FItemDetails ItemData)
{
	PickItemInSlot(ItemData);
}

void ALSPlayer::ServerDrop_Implementation()
{
	//MultiPickUp();
}

void ALSPlayer::MultiDrop_Implementation()
{
	/*ULSPlayerSiJaeAnimInstance* AnimInstance = Cast<ULSPlayerSiJaeAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->SetPickUpAnim();
		UE_LOG(LogTemp, Warning, TEXT("Player Picking ANIMATION SIJAE"));
	}*/

	/*if (IsLocallyControlled())
	{
		GetMesh()->SetVisibility(false);
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
			{
				GetMesh()->SetVisibility(true);
			}
		), 2.f, false);
	}*/
}

void ALSPlayer::ServerDropItemFromSlot_Implementation(TSubclassOf<AMasterItem> ItemClass, FVector SpawnLocation, FRotator SpawnRotation, int32 SlotIndex)
{
	//MultiDropItemFromSlot(ItemClass, SpawnLocation, SpawnRotation);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = this;

	//서버에서 아이템 스폰
	//AMasterItem* SpawnedItem = GetWorld()->SpawnActor<AMasterItem>(ItemClass,SpawnLocation,SpawnRotation,SpawnParams);

	AMasterItem* SpawnedItem = GetWorld()->SpawnActor<AMasterItem>(ItemClass,DropItemLoc->GetComponentLocation() + FVector(0, 0, 20.f),DropItemLoc->GetComponentRotation());

	if (SpawnedItem)
	{
		if (UStaticMeshComponent* ItemMesh = SpawnedItem->FindComponentByClass<UStaticMeshComponent>())
		{
			ItemMesh->SetSimulatePhysics(true);      // 물리 활성화
			ItemMesh->SetEnableGravity(true);        // 중력 적용
			ItemMesh->AddImpulse(FVector(0, 0, 100.f)); // 위로 살짝 힘
		}
	}



	//클라이언트 인벤토리 수정요청
	ClientDropItemFromSlot(SlotIndex);
	RefreshWeaponEquipFromCurrentSlot();
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
			UTexture2D* EmptyIcon = nullptr;
			HUD->SetIcon(SlotIndex, EmptyIcon);
		}
		RefreshWeaponEquipFromCurrentSlot();
	}
	SaveInventoryToGameInstance();
}



/*******************

 Throw Section

********************/

void ALSPlayer::StartThrowPreview()
{
	if (bIsDead) return;

	//궤적 초기화
	bThrowPreview = true;
	CachedPathPoints.Reset();

	// 한번만 그림
	UpdateThrowPreview();

	if (HasAuthority())
	{
		MultiStartThrowPreview();
		return;
	}

	if (!HasAuthority())
	{
		ServerStartThrowPreview();
		return;
	}
}

void ALSPlayer::UpdateThrowPreview()
{
	if (!DropItemLoc) return;

	//포물선 시작점 (BP Arrow)
	const FVector StartLoc = DropItemLoc->GetComponentLocation();

	//속력
	const FVector V0 = ComputeThrowInitialVelocity_ByCamPitch();

	FPredictProjectilePathParams P;
	P.StartLocation = StartLoc;
	P.LaunchVelocity = V0;
	P.ProjectileRadius = 8.f;
	//궤적 그리는 시간
	P.MaxSimTime = PreviewTime;
	P.SimFrequency = PreviewSegments / FMath::Max(PreviewTime, 0.01f);
	P.bTraceWithCollision = true;
	P.TraceChannel = ECC_Visibility;
	P.OverrideGravityZ = GetWorld()->GetGravityZ();

	//포물선 계싼
	FPredictProjectilePathResult Result;
	const bool bHit = UGameplayStatics::PredictProjectilePath(GetWorld(), P, Result);

	// 디버그라인
	CachedPathPoints.Reset();
	for (int32 i = 0; i < Result.PathData.Num() - 1; ++i)
	{
		const FVector P0 = Result.PathData[i].Location;
		const FVector P1 = Result.PathData[i + 1].Location;
		CachedPathPoints.Add(P0);

		// 선분으로 궤적 
		DrawDebugLine(GetWorld(), P0, P1, FColor::Cyan, false, 0.f, 0, 1.5f);

		//연한 회색 like 배그
		//FColor LightGray = FColor(150, 150, 150);
		//DrawDebugLine(GetWorld(), P0, P1, LightGray, false, 0.f, 0, 1.5f);
	}
	if (Result.PathData.Num() > 0)
	{
		CachedPathPoints.Add(Result.PathData.Last().Location);
	}

	// 착지 지점 표시
	if (bHit)
	{
		//DrawDebugSphere(GetWorld(), Result.HitResult.Location, 8.f, 12, FColor::Yellow, false, 0.f);
	}
}

void ALSPlayer::EndThrowPreview(bool bDoThrow)
{
	if (!bThrowPreview) return;
	bThrowPreview = false;

	if (bDoThrow)
	{
		ThrowItem();
	}

	if (HasAuthority())
	{
		MultiEndThrowPreview(bDoThrow);
		return;
	}

	if (!HasAuthority())
	{
		ServerEndThrowPreview(bDoThrow);
		return;
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

					SaveInventoryToGameInstance();

					// UI 아이콘 제거
					HUD->SetIcon(CurrentSelectedSlot, nullptr);
					//UE_LOG(LogTemp, Warning, TEXT("Item thrown from slot %d"), CurrentSelectedSlot);
				}
			}
			//SaveInventoryToGameInstance();
		}
	}
}

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
		//LS_LOG(LogLS, Error, TEXT("ItemClass or DropItemLoc is null"));
		return;
	}

	const FVector ThrowStartLocation = DropItemLoc->GetComponentLocation();
	const FRotator ThrowRotation = GetActorRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = this;

	AMasterItem* ThrownItem = GetWorld()->SpawnActor<AMasterItem>(ItemClass, ThrowStartLocation, ThrowRotation, SpawnParams);
	if (!ThrownItem) { LS_LOG(LogLS, Error, TEXT("Failed to spawn throwable item")); return; }

	ThrownItem->bIsThrown = true;
	if (UStaticMeshComponent* ItemMesh = ThrownItem->FindComponentByClass<UStaticMeshComponent>())
	{
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetNotifyRigidBodyCollision(true);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
		//NPC HIT
		ItemMesh->OnComponentHit.AddDynamic(ThrownItem, &AMasterItem::OnItemHit);
		//초기 속도
		const FVector V0 = ComputeThrowInitialVelocity_ByCamPitch();
		ItemMesh->SetPhysicsLinearVelocity(FVector::ZeroVector, false);
		ItemMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector, false);
		ItemMesh->SetPhysicsLinearVelocity(V0, false);
	}

	MultiSpawnThrowableItem(ItemToThrow);
}

FVector ALSPlayer::ComputeThrowInitialVelocity() const
{
	const FVector Dir = (GetActorForwardVector() + FVector(0, 0, ThrowUpRatio)).GetSafeNormal();
	return Dir * ThrowSpeed; // cm/s
}

float ALSPlayer::GetThrowAlphaFromPitch() const
{
	const FRotator CamRot = GetControlRotation();                 // 컨트롤 기준
	const float Pitch = FRotator::NormalizeAxis(CamRot.Pitch);    // -180~180 -> 정규화
	return FMath::GetMappedRangeValueClamped(
		FVector2D(PitchMinDeg, PitchMaxDeg),
		FVector2D(0.f, 1.f),
		Pitch
	);
}

FVector ALSPlayer::ComputeThrowInitialVelocity_ByCamPitch() const
{
	const float A = GetThrowAlphaFromPitch();
	const float Speed = FMath::Lerp(SpeedNear, SpeedFar, A);
	const float UpBias = FMath::Lerp(UpBiasNear, UpBiasFar, A);
	// 카메라 전방 + 위쪽 바이어스
	const FVector CamForward = FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X);
	const FVector Dir = (CamForward + FVector(0, 0, UpBias)).GetSafeNormal();

	return Dir * Speed;
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
		//UE_LOG(LogTemp, Warning, TEXT("Player throwing ANIMATION"));
	}
	//return;
}

void ALSPlayer::ServerStartThrowPreview_Implementation()
{
	MultiStartThrowPreview();
}

void ALSPlayer::MultiStartThrowPreview_Implementation()
{
	ULSPlayerIJaeAnimInstance* AnimInstance = Cast<ULSPlayerIJaeAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->StartThrowingAnim();
		//UE_LOG(LogTemp, Warning, TEXT("Player start throwing ANIMATION"));
	}
}

void ALSPlayer::ServerEndThrowPreview_Implementation(bool bDoThrow)
{
	MultiEndThrowPreview(bDoThrow);
}

void ALSPlayer::MultiEndThrowPreview_Implementation(bool bDoThrow)
{
	ULSPlayerIJaeAnimInstance* AnimInstance = Cast<ULSPlayerIJaeAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->EndThrowingAnim();
		//UE_LOG(LogTemp, Warning, TEXT("Player End throwing ANIMATION"));
	}
}



/*******************

 Attack Section

********************/

void ALSPlayer::Attack()
{
	if (bIsDead) return;

	if (bIsCombining)
	{
		ILSScriptWidgetInterface* ScriptWidget = Cast<ILSScriptWidgetInterface>(GetController());
		if (ScriptWidget)
		{
			ScriptWidget->UpdateScriptWidget("Decouple to Attack");
		}
		return;
	}

	//UE_LOG(LogTemp, Error, TEXT("=== Attack() CALLED ==="));
	//LS_LOG(LogLS, Warning, TEXT("Attack() called"));
	//MultiAttack();


	//LS_LOG(LogLS, Warning, TEXT(":Attack() called"));

	int32 CurrentSelectedSlot = SelectedSlot;
	if (CurrentSelectedSlot >= 0 && CurrentSelectedSlot < ItemInfoArray.Num())
	{
		if (!ItemInfoArray[CurrentSelectedSlot].IsEmpty)
		{
			// 아이템이 있으면 던지기
			//LS_LOG(LogLS, Warning, TEXT("Item found in slot %d - throwing item"), CurrentSelectedSlot);
			ThrowItem();
			//return;
		}
	}

	// 아이템이 없으면
	//LS_LOG(LogLS, Warning, TEXT("No item in selected slot "));
	if (HasAuthority())
	{
		if (CharacterChoice == ELSCharacterChoice::SiJae)
		{
			const int32 SlotIndex = SelectedSlot;
			if (!ItemInfoArray.IsValidIndex(SlotIndex))
				return;
			const FItemDetails& Info = ItemInfoArray[SlotIndex];

			if (Info.IsWeapon)
			{
				MultiAttack();
			}

			
		}
		

		
	}
	else
	{
		if (CharacterChoice == ELSCharacterChoice::SiJae)
		{
			const int32 SlotIndex = SelectedSlot;
			if (!ItemInfoArray.IsValidIndex(SlotIndex))
				return;
			const FItemDetails& Info = ItemInfoArray[SlotIndex];

			if (Info.IsWeapon)
			{
				ServerAttack();
			}
		
			
		}
		
	}
}

void ALSPlayer::ProcessAttack()
{
	//LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("Begin"));

	FHitResult OutHitResult;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);
	const float AttackRange = 80.0f;
	const float AttackRadius = 30.0f;
	const float AttackDamage = 10.0f;
	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * AttackRange;
	FColor DrawColor;

	bool HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeSphere(AttackRadius), Params);
	//MultiProcessAttack();



	if (HitDetected)
	{
		//LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("HitDetected"));
		APlayerController* PlayerController = Cast<APlayerController>(GetController());

		if (ALSPlayer* HitPlayer = Cast<ALSPlayer>(OutHitResult.GetActor()))
		{
			// Player면 데미지 x
			//UE_LOG(LogTemp, Warning, TEXT("Hit player - no damage"));
			DrawColor = FColor::Red;
		}

		else if (ILSTakeDamageInterface* HitNPC = Cast<ILSTakeDamageInterface>(OutHitResult.GetActor()))
		{
			//LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("ILSTakeDamageInterface Detected"));
			
			
			//사운드
			const FVector HitLocation = OutHitResult.ImpactPoint;
			if (HitSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, HitSound, HitLocation);
			}

			// NPC만 데미지 적용
			FDamageEvent DamageEvent;
			HitNPC->TakeDamage(20.0f, DamageEvent, GetController(), this);
			DrawColor = FColor::Blue;
		}
	}
	else
	{
		//LS_LOG(LogLS, Warning, TEXT("processattack() - No hit detected"));

		//DrawColor = FColor::Red;
	}

	//#if ENABLE_DRAW_DEBUG
	//
	//	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	//	float CapsuleHalfHeight = AttackRange * 0.5f;
	//
	//
	//	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);
	//
	//#endif
		//
}

void ALSPlayer::ServerAttack_Implementation()
{
	MultiAttack();
}

void ALSPlayer::MultiAttack_Implementation()
{
	/*if (!EquippedWeaponMesh)
	{
		LS_LOG(LogLS, Warning, TEXT("No weapon equipped - Attack blocked"));
		return;
	}*/

	

	// 슬롯이 비었거나 무기가 아니면 해제
	


	ULSPlayerSiJaeAnimInstance* AnimInstance = Cast<ULSPlayerSiJaeAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->SetAttackAnim();
		//UE_LOG(LogTemp, Warning, TEXT("Player Attack ANIMATION SIJAE"));
	}
}

void ALSPlayer::ServerProcessAttack_Implementation()
{
	ProcessAttack();
}

void ALSPlayer::MultiProcessAttack_Implementation()
{
	//UE_LOG(LogTemp, Error, TEXT("=== MultiAttack_Implementation() CALLED ==="));

	/*ULSPlayerSiJaeAnimInstance* AnimInstance = Cast<ULSPlayerSiJaeAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->SetAttackAnim();
		UE_LOG(LogTemp, Warning, TEXT("Player Attack ANIMATION SIJAE"));
	}*/
}



/*******************

Combine Section

********************/
//PusherSiJaeCharacter, PushedIJaeCharacter는 게임이 시작될 때부터 변하지 않으므로 StartGame이 호출될 때 처음 초기화하고 Replicated해놨음.
//CharacterChoice도 마찬가지로 게임이 시작될 때부터 변하지 않으므로 StartGame이 호출될 때 처음 초기화하고 Replicated해놨음.

bool ALSPlayer::isCombining()
{
	return bIsCombining;
}

void ALSPlayer::ChangeCombineState(uint8 InIsCombining)
{
	//LS_LOG(LogLS, Log, TEXT("Begin : %d"), InIsPushing);

	//제일 먼저 호출되는 함수.
	//캐릭터에 상관 없이 서버에서만 호출되도록 구현해놨음!
	//아래의 코드들은 서버에서 작동하는 코드라고 생각하고 읽어보세요
	//자동 풀림 (스테미나 고갈, 거리 멀어짐)일땐 서버 중 로컬 플레이어일때만 호출됨.
	//공격받을때 풀리는건 서버로컬이 아닐 수도 있음.

	if (HasAuthority())
	{
		//서버의 모든 플레이어 캐릭터(자기자신 포함) 순환
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALSPlayer::StaticClass(), FoundActors);
		for (AActor* Actor : FoundActors)
		{
			ALSPlayer* PlayerCharacter = Cast<ALSPlayer>(Actor);
			if (PlayerCharacter)
			{
				PlayerCharacter->SetbIsCombining(InIsCombining);		//bIsCombining는 Replicated가 되어있으므로 서버 플레이어만 호출함.
				PlayerCharacter->MulticastRPCApplyCombineState();	//클라이언트까지 호출하기위해 멀티캐스트함수 호출
			}
		}
	}
}

void ALSPlayer::ApplyCombineState()
{
	//이녀석은 멀티캐스트를 통해 호출되기 때문에 서버의 시제, 이제 / 클라이언트의 시제, 이제 총 4번 호출됨

	if (HasAuthority())
	{
		//스테미너 제어는 무조건 서버 플레이어들만 제어. (시제 이제 같이 진행함)

		//스테미나 타이머 초기화 - 스테미너 타이머는 감소든 증가든 하나만 활성화되기때문에 1개로 합쳤음
		//타이머 변경을 시작하는 순간에 한번만 초기화하면 됨.
		if (GetWorldTimerManager().IsTimerActive(CombineStaminaTimer))
		{
			GetWorldTimerManager().ClearTimer(CombineStaminaTimer);
		}

		//스테미나 타이머 활성화, 함수 호출 시작
		if (bIsCombining)
		{
			//Set Decrease Stemina
			GetWorld()->GetTimerManager().SetTimer(CombineStaminaTimer, this, &ALSPlayer::DecreaseCombineStamina, 0.1f, true);
		}
		else
		{
			//Set Increase Stamina
			GetWorld()->GetTimerManager().SetTimer(CombineStaminaTimer, this, &ALSPlayer::IncreaseCombineStamina, 0.1f, true, CombineStaminaIncreaseDelay);
		}
	}



	/***************************************/
	/****아래는 이제의 카메라 변경 코드****/
	/***************************************/


	if (CharacterChoice == ELSCharacterChoice::SiJae) //시제면 바로 리턴
		return;

	if (bIsCombining)
	{
		/************************************
		/**휠체어를 밀기 시작할때 상태변경
		/************************************/

		//움직임 제어 - 내부내용은 이전과 똑같음.
		if (APlayerController* PC = Cast<APlayerController>(Controller))
		{
			PC->SetIgnoreMoveInput(true);
		}

		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		//LS_LOG(LogLS, Log, TEXT("Movement Set : %d, %d"), bUseControllerRotationYaw, GetCharacterMovement()->bOrientRotationToMovement);


		//여기부터는 카메라 제어
		//따라서 로컬 플레이어인지 확인 - 내부내용은 이전과 똑같음.
		if (IsLocallyControlled())
		{
			//카메라 계산
			FRotator TargetRot(0.f, 0.f, 0.f);
			TargetRot.Yaw = GetControlRotation().Yaw; //이제의 현재 시점으로 Yaw회전 지정

			if (APlayerController* PC = Cast<APlayerController>(Controller))
			{
				PC->SetIgnoreLookInput(true);

				// 컨트롤러 & 액터 회전 동기화
				PC->SetControlRotation(TargetRot);
				SetActorRotation(TargetRot, ETeleportType::TeleportPhysics);

				// 카메라 전환
				//if (CameraBoom) CameraBoom->bDoCollisionTest = false;
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
	}
	else
	{
		/************************************
		/**휠체어를 밀기 종료일때 상태변경
		/************************************/

		//움직임 제어 - 내부내용은 이전과 똑같음.
		if (APlayerController* PC = Cast<APlayerController>(Controller))
		{
			PC->SetIgnoreMoveInput(false);
		}

		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		//LS_LOG(LogLS, Log, TEXT("Movement Set : %d, %d"), bUseControllerRotationYaw, GetCharacterMovement()->bOrientRotationToMovement);

		//여기부터는 카메라 제어
		//따라서 로컬 플레이어인지 확인 - 내부내용은 이전과 똑같음.
		if (IsLocallyControlled())
		{
			//카메라 계산
			FRotator TargetRot(0.f, 0.f, 0.f);
			if (PushedIJaeCharacter)
			{
				TargetRot.Yaw = PushedIJaeCharacter->GetControlRotation().Yaw; //시제의 현재 시점으로 Yaw회전 지정
			}
			else
			{
				LS_LOG(LogLS, Error, TEXT("No PusherCharacter!!"));
				return;
			}

			if (APlayerController* PC = Cast<APlayerController>(Controller))
			{
				PC->SetIgnoreLookInput(true);

				// 컨트롤러 & 액터 회전 동기화
				PC->SetControlRotation(TargetRot);
				SetActorRotation(TargetRot, ETeleportType::TeleportPhysics);

				// 카메라 전환
				//if (CameraBoom) CameraBoom->bDoCollisionTest = true;
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
	}
}

void ALSPlayer::HandleCombinedIJaeMovement()
{
	//이제가 bIsCombining일때 Tick으로 호출되는 함수.
	//호출되는 변수들의 변경은 있으나 구조는 똑같음.

	if (CharacterChoice == ELSCharacterChoice::SiJae || !bIsCombining)
		return;

	if (!PusherSiJaeCharacter)
	{
		LS_LOG(LogLS, Error, TEXT("No REFEC_PusherCharacter!!!"));
		return;
	}

	FVector PusherLocation = PusherSiJaeCharacter->GetActorLocation();
	FVector PusherForward = PusherSiJaeCharacter->GetActorForwardVector();

	if (!HasAuthority())
	{
		//클라이언트 이제인 경우 회전만 적용
		FRotator NewRotation = PusherForward.Rotation();
		NewRotation.Pitch = 0.0f;
		SetActorRotation(NewRotation);

		return;
	}

	FVector TargetLocation = PusherLocation + (PusherForward * CombineDistance);
	FVector CurrentLocation = GetActorLocation();
	TargetLocation.Z = CurrentLocation.Z;
	SetActorLocation(TargetLocation, true);

	FRotator NewRotation = PusherForward.Rotation();
	NewRotation.Pitch = 0.0f;
	SetActorRotation(NewRotation);
}

void ALSPlayer::CheckCombineDistance()
{
	//서버의 로컬플레이어가 bIsCombining일때 Tick으로 호출되는 함수.
	// 즉 얘도 서버의 로컬플레이어 캐릭터에서만 기능하는 코드들인 것을 알아두셈!

	if (HasAuthority())
	{
		if (!bIsCombining || !IsLocallyControlled())	//오류방지 확인용 코드
			return;

		if (!IsPlayerNear())	//시제, 이제 거리 확인 멀면 true 가까우면 false
		{
			ChangeCombineState(false);		//멀면 합체 해제함.
		}
	}
}

bool ALSPlayer::IsPlayerNear()
{
	if (!PusherSiJaeCharacter || !PushedIJaeCharacter) //오류방지 확인용 코드
	{
		if (!PusherSiJaeCharacter)
		{
			LS_LOG(LogLS, Error, TEXT("%s"), TEXT("No PusherCharacter!!"));
		}
		else if (!PushedIJaeCharacter)
		{
			LS_LOG(LogLS, Error, TEXT("%s"), TEXT("No PushedCharacter!!"));
		}

		return false;
	}

	//호출되는 변수들의 변경은 있으나 구조는 똑같음.
	FVector PushedLocation = PushedIJaeCharacter->GetActorLocation();
	FVector PusherLocation = PusherSiJaeCharacter->GetActorLocation();
	float CurrentDistance = FVector::Dist(PushedLocation, PusherLocation);
	return CurrentDistance <= MaxCombineDistance;
}

void ALSPlayer::DecreaseCombineStamina()
{
	// 이 함수는 서버 플레이어 캐릭터에서만 호출됨!
	// 아래의 코드들이 서버에서만 기능하고있다는 것을 알고있으셈

	if (!HasAuthority() || !bIsCombining) return;

	float CheckSpeed = 0.0f;

	if (CharacterChoice == ELSCharacterChoice::SiJae)
	{
		CheckSpeed = GetVelocity().Size();
	}
	else if (PusherSiJaeCharacter)
	{
		CheckSpeed = PusherSiJaeCharacter->GetVelocity().Size();
	}

	if (CheckSpeed < 10.0f)
	{
		CurrentCombineStamina = FMath::Min(MaxCombineStamina, CurrentCombineStamina + (CombineStaminaIncreaseRate * 0.5f) * 0.1f);

		if (IsLocallyControlled())
		{
			UpdateCombineStaminaWidget(CurrentCombineStamina);
		}
		else
		{
			ClientRPCUpdateCombineStaminaWidget(CurrentCombineStamina);
		}
		return;
	}

	//이거는 호출되는 변수들의 변경은 있으나 구조는 똑같음.
	CurrentCombineStamina = FMath::Max(0.0f, CurrentCombineStamina - CombineStaminaDecreaseRate * 0.1f);

	if (IsLocallyControlled())
	{
		//스테미나 위젯 업데이트 - 로컬컨트롤러니까 그대로 업데이트함수 호출
		UpdateCombineStaminaWidget(CurrentCombineStamina);

		//서버의 로컬플레이어 캐릭터에서만 ChangeCombineState를 호출해야됨. 
		if (CurrentCombineStamina <= 0.0f)	//0보다 작은거 확인하고 호출
		{
			ChangeCombineState(false);
		}
	}
	else
	{
		//로컬컨트롤러가 아니라는건 서버의 프록시캐릭터라는 것.
		//컨트롤러를 가진 클라이언트의 캐릭터를 호출하기위해 클라이언트RPC호출
		ClientRPCUpdateCombineStaminaWidget(CurrentCombineStamina);
	}
}

void ALSPlayer::IncreaseCombineStamina()
{
	// 이 함수는 서버 플레이어 캐릭터에서만 호출됨!
	if (!HasAuthority() || bIsCombining) return;

	//다 충전됐으면 타이머 해제
	if (CurrentCombineStamina >= MaxCombineStamina)
	{
		if (GetWorldTimerManager().IsTimerActive(CombineStaminaTimer))
		{
			GetWorldTimerManager().ClearTimer(CombineStaminaTimer);
		}
	}

	//그거요 그거
	CurrentCombineStamina = FMath::Min(MaxCombineStamina, CurrentCombineStamina + CombineStaminaIncreaseRate * 0.1f);

	//위젯업데이트. DecreaseCombineStamina랑 똑같은 과정임
	if (IsLocallyControlled())
	{
		UpdateCombineStaminaWidget(CurrentCombineStamina);
	}
	else
	{
		ClientRPCUpdateCombineStaminaWidget(CurrentCombineStamina);
	}
}

void ALSPlayer::UpdateCombineStaminaWidget(float InCurrentStemina)
{
	//위젯 업데이트하는 코드 그대로 가져온거
	if (IsLocallyControlled())
	{
		if (ALSPlayerController* PC = Cast<ALSPlayerController>(GetController()))
		{
			if (ULSHUDWidget* HUD = PC->GetLSHUDWidget())
			{
				HUD->UpdateStaminaBar(InCurrentStemina, MaxCombineStamina);
				HUD->ShowStaminaBar(true);
			}
		}
	}
}

//RPC 함수들은 Wrapper용으로만 동작하게 로컬함수 호출하는 코드만 두고 다른 기능은 작성하지 않는 것이 좋음
//왜냐면...헤더에서 함수 찾아서 F12눌러도 "_Implementation" 문구때문에 cpp의 구현부로 바로 안 넘어가거든...
void ALSPlayer::ServerRPCChangeCombineState_Implementation(uint8 InIsCombining)
{
	ChangeCombineState(InIsCombining);
}

void ALSPlayer::MulticastRPCApplyCombineState_Implementation()
{
	ApplyCombineState();
	ApplyCombineVisual(bIsCombining);
}

void ALSPlayer::ClientRPCUpdateCombineStaminaWidget_Implementation(float InCurrentStemina)
{
	UpdateCombineStaminaWidget(InCurrentStemina);
}



/*******************

 ETC Section

********************/

bool ALSPlayer::IsActorName(AActor* InActor, const FString& InString) const
{

	if (!InActor) return false;

	FString ActorName = InActor->GetName();

	//LS_LOG(LogLS, Log, TEXT("Begin : %s"), *ActorName);
	return ActorName.Contains(InString);
}


/*******************

 Weapon Section

********************/


void ALSPlayer::RefreshWeaponEquipFromCurrentSlot()
{
	if (CharacterChoice != ELSCharacterChoice::SiJae)
	{
		return;
	}

	const int32 SlotIndex = SelectedSlot;

	if (!ItemInfoArray.IsValidIndex(SlotIndex))
		return;

	const FItemDetails& Info = ItemInfoArray[SlotIndex];

	// 슬롯이 비었거나 무기가 아니면 해제
	if (Info.IsEmpty || !Info.IsWeapon)
	{
		if (HasAuthority())
		{
			MultiApplyWeaponVisualFromItem(Info); 
			
		}
		else
		{
			ServerApplyWeaponVisualFromItem(Info);
		}
		return;
	}

	if (HasAuthority())
	{
		 
		MultiApplyWeaponVisualFromItem(Info);          
		     
	}
	else
	{
		ServerApplyWeaponVisualFromItem(Info);
	}
	
}

/*******************

Combine Section

********************/

void ALSPlayer::Menu()
{
	
	ALSPlayerController* PC = Cast<ALSPlayerController>(GetController());
	if (!PC)
		return;
	PC->OpenMenu();
}


//item instance
ELSNetworkPosition ALSPlayer::GetNetworkPositionForInventory() const
{
	if (HasAuthority())
	{
		
		return IsLocallyControlled() ? ELSNetworkPosition::Server : ELSNetworkPosition::Client;
	}

	// 클라이언트
	return ELSNetworkPosition::Client;
}



//무기 mesh용 함수
void ALSPlayer::ApplyWeaponVisualFromItem(const FItemDetails& Info)
{

}

void ALSPlayer::MultiApplyWeaponVisualFromItem_Implementation(const FItemDetails& Info)
{
	if (!EquippedWeaponMesh)
		return;

	// 비었거나 무기가 아니면 메쉬 숨기기
	if (Info.IsEmpty || !Info.IsWeapon)
	{
		EquippedWeaponMesh->SetStaticMesh(nullptr);
		EquippedWeaponMesh->SetVisibility(false);
		return;
	}

	UStaticMesh* NewMesh = Info.EquippedMesh.LoadSynchronous();
	if (!NewMesh)
	{
		EquippedWeaponMesh->SetStaticMesh(nullptr);
		EquippedWeaponMesh->SetVisibility(false);
		return;
	}

	EquippedWeaponMesh->SetStaticMesh(NewMesh);
	EquippedWeaponMesh->SetVisibility(true);
}

void ALSPlayer::ServerApplyWeaponVisualFromItem_Implementation(const FItemDetails& Info)
{
	MultiApplyWeaponVisualFromItem(Info);
}



//합체 껐다키기
void ALSPlayer::ApplyCombineVisual(bool bCombined)
{
	if (!GetMesh() || !CombinedMeshComp)
		return;

	if (CharacterChoice != ELSCharacterChoice::SiJae)
	{
		return;
	}

	if (PusherSiJaeCharacter && this != PusherSiJaeCharacter)
		return;

	if (bCombined)
	{
		GetMesh()->SetVisibility(false);
		CombinedMeshComp->SetVisibility(true);
	}
	else
	{
		GetMesh()->SetVisibility(true);
		CombinedMeshComp->SetVisibility(false);
	}
}




//sequence
void ALSPlayer::StartSequence_Implementation()
{
	bisSequencePlaying = true;
}

void ALSPlayer::EndSequence_Implementation()
{
	bisSequencePlaying = false;
}