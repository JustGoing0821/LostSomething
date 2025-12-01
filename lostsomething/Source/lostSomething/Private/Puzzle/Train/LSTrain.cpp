// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/Train/LSTrain.h"
#include "lostSomething.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "Net/UnrealNetwork.h"
#include "Physics/LSCollisionProfile.h"

ALSTrain::ALSTrain()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	//Replication
	bReplicates = true;

	//Gate Side
	bisGateLeftSide =false;

	// Moving Location
	WaitLocation = FVector(3586.0f, -290.0f, -6.2f);
	LeaveLocation = FVector(8000.0f, -290.0f, -6.2f);

	//Root Component
	SharedRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SharedRoot"));
	RootComponent = SharedRoot;

	LeftSideGatesRoot = CreateDefaultSubobject<USceneComponent>(TEXT("LeftSideGatesRoot"));
	LeftSideGatesRoot->SetupAttachment(SharedRoot);
	LeftSideGatesRoot->SetRelativeLocation(FVector(127.42f, 0, 0));

	RightSideGatesRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RightSideGatesRoot"));
	RightSideGatesRoot->SetupAttachment(SharedRoot);
	RightSideGatesRoot->SetRelativeLocation(FVector(-127.42f, 0, 0));

	LeftSideCrowdRoot = CreateDefaultSubobject<USceneComponent>(TEXT("LeftSideCrowdRoot"));
	LeftSideCrowdRoot->SetupAttachment(SharedRoot);

	RightSideCrowdRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RightSideCrowdRoot"));
	RightSideCrowdRoot->SetupAttachment(SharedRoot);

	// Train Trigger Section
	TrainTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TrainTrigger"));
	TrainTrigger->SetupAttachment(SharedRoot);
	TrainTrigger->SetBoxExtent(FVector(150.0f, 1200.0f, 150.0f));
	TrainTrigger->SetRelativeLocation(FVector(0, -1000.f, 80.f));
	TrainTrigger->SetCollisionProfileName(CPROFILE_LSPAWN);

	// Mesh Ref
	//static ConstructorHelpers::FObjectFinder<UStaticMesh> WallMeshRef(TEXT("/Game/Asset/Map/CitySubwayTrainModuler/Meshes/Structure/SM_ext_wall_100_01.SM_ext_wall_100_01"));
	//static ConstructorHelpers::FObjectFinder<UStaticMesh> DoorMeshRef(TEXT("/Game/Asset/Map/CitySubwayTrainModuler/Meshes/Structure/SM_wall_doorway_01.SM_wall_doorway_01"));
	//static ConstructorHelpers::FObjectFinder<UStaticMesh> RoofMeshRef(TEXT("/Game/Asset/Map/CitySubwayTrainModuler/Meshes/Structure/SM_ext_roof_200_01.SM_ext_roof_200_01"));
	//static ConstructorHelpers::FObjectFinder<UStaticMesh> DoorLMeshRef(TEXT("/Game/Asset/Map/CitySubwayTrainModuler/Meshes/Structure/SM_door_wall_01.SM_door_wall_01"));
	//static ConstructorHelpers::FObjectFinder<UStaticMesh> DoorRMeshRef(TEXT("/Game/Asset/Map/CitySubwayTrainModuler/Meshes/Structure/SM_door_wall_02.SM_door_wall_02"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DoorMeshRef(TEXT("/Game/Asset/Map/ModSubwayStation/StaticMeshes/SM_R211_Door_Ext.SM_R211_Door_Ext"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CrowdMeshRef(TEXT("/Game/Level/Puzzle/Train/SM_SM_Crowd.SM_SM_Crowd"));

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> DoorMaterialRef(TEXT("/Game/Asset/Map/ModSubwayStation/Materials/MI_SubwayCar_Ext"));
	static ConstructorHelpers::FObjectFinder<UMaterial> CrowdMaterialRef(TEXT("/Game/Level/Puzzle/Train/MALE.MALE"));
	static FName GateNames[] = { TEXT("Gate1") , TEXT("Gate2"), TEXT("Gate3"), TEXT("Gate4"), TEXT("Gate5"), TEXT("Gate6") };

	// Car Generate
	FVector LeftSideDoorLLocation = FVector(0, 0, 0);
	FVector LeftSideDoorRLocation = FVector(0, -74.f, 0);
	FVector RightSideDoorLLocation = FVector(0, -148.f, 0);
	FVector RightSideDoorRLocation = FVector(0, -74.f, 0);
	FVector LeftSideCrowdLocation = FVector(70.f, -74.f, 0);
	FVector RightSideCrowdLocation = FVector(-80.f, -74.f, 0);


	for (FName GateName : GateNames)
	{
		FName LeftSideDoorLName = *GateName.ToString().Append(TEXT("LeftSideDoorL"));
		UStaticMeshComponent* LeftSideDoorL = CreateDefaultSubobject<UStaticMeshComponent>(LeftSideDoorLName);
		LeftSideDoorL->SetupAttachment(LeftSideGatesRoot);
		LeftSideDoorL->SetStaticMesh(DoorMeshRef.Object);
		LeftSideDoorL->SetMaterial(0, DoorMaterialRef.Object);
		LeftSideDoorL->SetRelativeLocationAndRotation(LeftSideDoorLLocation, FRotator(0, -90.f, 0));
		LeftSideDoorL->SetCollisionProfileName(TEXT("IgnoreOnlyPawn"));
		LeftSideDoorLs.Add(LeftSideDoorL);

		FName LeftSideDoorRName = *GateName.ToString().Append(TEXT("LeftSideDoorR"));
		UStaticMeshComponent* LeftSideDoorR = CreateDefaultSubobject<UStaticMeshComponent>(LeftSideDoorRName);
		LeftSideDoorR->SetupAttachment(LeftSideGatesRoot);
		LeftSideDoorR->SetStaticMesh(DoorMeshRef.Object);
		LeftSideDoorR->SetMaterial(0, DoorMaterialRef.Object);
		LeftSideDoorR->SetRelativeLocationAndRotation(LeftSideDoorRLocation, FRotator(0, -90.f, 0));
		LeftSideDoorR->SetCollisionProfileName(TEXT("IgnoreOnlyPawn"));
		LeftSideDoorRs.Add(LeftSideDoorR);

		FName RightSideDoorLName = *GateName.ToString().Append(TEXT("RightSideDoorL"));
		UStaticMeshComponent* RightSideDoorL = CreateDefaultSubobject<UStaticMeshComponent>(RightSideDoorLName);
		RightSideDoorL->SetupAttachment(RightSideGatesRoot);
		RightSideDoorL->SetStaticMesh(DoorMeshRef.Object);
		RightSideDoorL->SetMaterial(0, DoorMaterialRef.Object);
		RightSideDoorL->SetRelativeLocationAndRotation(RightSideDoorLLocation, FRotator(0, 90.f, 0));
		RightSideDoorL->SetCollisionProfileName(TEXT("IgnoreOnlyPawn"));
		RightSideDoorLs.Add(RightSideDoorL);

		FName RightSideDoorRName = *GateName.ToString().Append(TEXT("RightSideDoorR"));
		UStaticMeshComponent* RightSideDoorR = CreateDefaultSubobject<UStaticMeshComponent>(RightSideDoorRName);
		RightSideDoorR->SetupAttachment(RightSideGatesRoot);
		RightSideDoorR->SetStaticMesh(DoorMeshRef.Object);
		RightSideDoorR->SetMaterial(0, DoorMaterialRef.Object);
		RightSideDoorR->SetRelativeLocationAndRotation(RightSideDoorRLocation, FRotator(0, 90.f, 0));
		RightSideDoorR->SetCollisionProfileName(TEXT("IgnoreOnlyPawn"));
		RightSideDoorRs.Add(RightSideDoorR);

		LeftSideDoorLLocation += FVector(0, -400.f, 0);
		LeftSideDoorRLocation += FVector(0, -400.f, 0);
		RightSideDoorLLocation += FVector(0, -400.f, 0);
		RightSideDoorRLocation += FVector(0, -400.f, 0);

		FName LeftSideCrowdName = *GateName.ToString().Append(TEXT("LeftSideCrowd"));
		UStaticMeshComponent* LeftSideCrowd = CreateDefaultSubobject<UStaticMeshComponent>(LeftSideCrowdName);
		LeftSideCrowd->SetupAttachment(LeftSideCrowdRoot);
		LeftSideCrowd->SetStaticMesh(CrowdMeshRef.Object);
		LeftSideCrowd->SetRelativeLocationAndRotation(LeftSideCrowdLocation, FRotator(0,0,0));
		LeftSideCrowd->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
		LeftSideCrowd->SetVisibility(true);
		LeftSideCrowd->SetMaterial(0, CrowdMaterialRef.Object);
		LeftSideCrowds.Add(LeftSideCrowd);
		LeftSideCrowdsLocation.Add(LeftSideCrowdLocation);

		FName RightSideCrowdName = *GateName.ToString().Append(TEXT("RightSideCrowd"));
		UStaticMeshComponent* RightSideCrowd = CreateDefaultSubobject<UStaticMeshComponent>(RightSideCrowdName);
		RightSideCrowd->SetupAttachment(RightSideCrowdRoot);
		RightSideCrowd->SetStaticMesh(CrowdMeshRef.Object);
		RightSideCrowd->SetRelativeLocationAndRotation(RightSideCrowdLocation, FRotator(0, 180, 0));
		RightSideCrowd->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
		RightSideCrowd->SetVisibility(true);
		RightSideCrowd->SetMaterial(0, CrowdMaterialRef.Object);
		RightSideCrowds.Add(RightSideCrowd);
		RightSideCrowdsLocation.Add(RightSideCrowdLocation);

		LeftSideCrowdLocation += FVector(0, -400.f, 0);
		RightSideCrowdLocation += FVector(0, -400.f, 0);
	}

	//Sound
	TrainArriveAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("TrainArriveAudioComponent"));
	GetOffAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("GetOffAudioComponent"));
	TrainLeaveAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("TrainLeaveAudioComponent"));
	TrainArriveAudioComponent->SetAutoActivate(true);
	GetOffAudioComponent->SetAutoActivate(false);
	TrainLeaveAudioComponent->SetAutoActivate(false);

	TimeBeforeGateOpen = 4.0f;
	TimeTrainWait = 0.0f;
	TimeBeforeTrainLeave = 4.0f;
	bisPassengersGettingOff = false;
}

void ALSTrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentTrainState== ETrainState::Comming)
	{
		CurrentTrainAlpha += DeltaTime * LerpSpeed;
		CurrentTrainAlpha = FMath::Clamp(CurrentTrainAlpha, 0.0f, 1.0f);
		FVector CurrentLocation = GetActorLocation();
		FVector NewLocation = FMath::Lerp(CurrentLocation, WaitLocation, CurrentTrainAlpha);
		SetActorLocation(NewLocation);

		if (HasAuthority() && (CurrentTrainAlpha ==1.0f))
		{
			CurrentTrainState = ETrainState::Waiting;

			if (GetWorld()->GetTimerManager().IsTimerActive(TrainTimerHandle))
			{
				GetWorld()->GetTimerManager().ClearTimer(TrainTimerHandle);
			}
			GetWorld()->GetTimerManager().SetTimer(TrainTimerHandle, FTimerDelegate::CreateLambda([&]
				{
					OnTrainArrived.Broadcast();
					CurrentTrainAlpha = 0.0f;
				}
			), TimeBeforeGateOpen, false);			
		}
	}
	else if (CurrentTrainState == ETrainState::Waiting)
	{
		if (bisPassengersGettingOff && CurrentPassengersAlpha != 1.0f)
		{
			CurrentPassengersAlpha += DeltaTime * 2.0f;
			CurrentPassengersAlpha = FMath::Clamp(CurrentPassengersAlpha, 0.0f, 1.0f);

			if (bisGateLeftSide)
			{
				for (int32 Num = 0; Num < LeftSideDoorLs.Num(); Num++)
				{
					//LS_LOG(LogLS, Log, TEXT("Num : %d"), Num);
					if (Num != CorrectDoorIndex)
					{
						FVector CurrentLocation = LeftSideCrowds[Num]->GetRelativeLocation();
						FVector NewLocation = LeftSideCrowdsLocation[Num] + FVector(150.f, 0, 0);
						NewLocation = FMath::Lerp(CurrentLocation, NewLocation, CurrentPassengersAlpha);
						LeftSideCrowds[Num]->SetRelativeLocation(NewLocation);
						//LS_LOG(LogLS, Log, TEXT("Crowd Moved"));
					}
				}
			}
			else
			{
				for (int32 Num = 0; Num < RightSideDoorLs.Num(); Num++)
				{
					//LS_LOG(LogLS, Log, TEXT("Num : %d"), Num);
					if (Num != CorrectDoorIndex)
					{
						FVector CurrentLocation = RightSideCrowds[Num]->GetRelativeLocation();
						FVector NewLocation = RightSideCrowdsLocation[Num] + FVector(-150.f, 0, 0);
						NewLocation = FMath::Lerp(CurrentLocation, NewLocation, CurrentPassengersAlpha);
						RightSideCrowds[Num]->SetRelativeLocation(NewLocation);
						//LS_LOG(LogLS, Log, TEXT("Crowd Moved"));
					}
				}
			}
		}
	}
	else if (CurrentTrainState == ETrainState::Leaving)
	{
		CurrentTrainAlpha += DeltaTime * LerpSpeed;
		CurrentTrainAlpha = FMath::Clamp(CurrentTrainAlpha, 0.0f, 1.0f);
		FVector CurrentLocation = GetActorLocation();
		FVector NewLocation = FMath::Lerp(CurrentLocation, LeaveLocation, CurrentTrainAlpha);
		SetActorLocation(NewLocation);
	}
}

void ALSTrain::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALSTrain, CurrentTrainState);
	DOREPLIFETIME(ALSTrain, TimeTrainWait);
	DOREPLIFETIME(ALSTrain, CorrectDoorIndex);
}

void ALSTrain::BeginPlay()
{
	Super::BeginPlay();
}

void ALSTrain::OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//LS_LOG(LogLS, Log, TEXT("Begin"));
}

void ALSTrain::GateOpen()
{
	if (bisGateLeftSide)
	{
		for (UStaticMeshComponent* DoorL : LeftSideDoorLs)
		{
			DoorL->SetVisibility(false);
			DoorL->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		for (UStaticMeshComponent* DoorR : LeftSideDoorRs)
		{
			DoorR->SetVisibility(false);
			DoorR->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
	else
	{
		for (UStaticMeshComponent* DoorL : RightSideDoorLs)
		{
			DoorL->SetVisibility(false);
			DoorL->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		for (UStaticMeshComponent* DoorR : RightSideDoorRs)
		{
			DoorR->SetVisibility(false);
			DoorR->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}

	if (TimeTrainWait > 0.f)
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(TrainTimerHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(TrainTimerHandle);
		}
		GetWorld()->GetTimerManager().SetTimer(TrainTimerHandle, FTimerDelegate::CreateLambda([&]
			{
				LeaveTrain();
				TimeTrainWait = 0.f;
			}
		), TimeTrainWait, false);
	}
}

void ALSTrain::GateClose()
{
	if (bisGateLeftSide)
	{
		for (UStaticMeshComponent* DoorL : LeftSideDoorLs)
		{
			DoorL->SetVisibility(true);
			DoorL->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		for (UStaticMeshComponent* DoorR : LeftSideDoorRs)
		{
			DoorR->SetVisibility(true);
			DoorR->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
	}
	else
	{
		for (UStaticMeshComponent* DoorL : RightSideDoorLs)
		{
			DoorL->SetVisibility(true);
			DoorL->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		for (UStaticMeshComponent* DoorR : RightSideDoorRs)
		{
			DoorR->SetVisibility(true);
			DoorR->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
	}

	if (GetWorld()->GetTimerManager().IsTimerActive(TrainTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(TrainTimerHandle);
	}
	GetWorld()->GetTimerManager().SetTimer(TrainTimerHandle, FTimerDelegate::CreateLambda([&]
		{
			CurrentTrainState = ETrainState::Leaving;
		}
	), TimeBeforeTrainLeave, false);
}

void ALSTrain::PuzzleCheck(bool bCorrect, int32 InCorrectGate)
{
	//LS_LOG(LogLS, Log, TEXT("InCorrectGate : %d"), InCorrectGate);

	if (bCorrect)
	{
		//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("True"));
		MulticastRPCGateOpen();
		CorrectDoorIndex = InCorrectGate - 1;
		MulticastRPCGetOffPassengers();
	}
	else
	{
		//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("False"));
		TimeTrainWait = 2.0f;
		MulticastRPCGateOpen();
		CorrectDoorIndex = -1;
		MulticastRPCGetOffPassengers();
	}
}

void ALSTrain::GetOffPassengers()
{
	if (HasAuthority())
	{
		MulticastRPCPlayTrainAudioComponent(ETrainSoundState::GetOff);
	}

	if (bisGateLeftSide)
	{
		for (int32 Num = 0; Num < LeftSideDoorLs.Num(); Num++)
		{
			//LS_LOG(LogLS, Log, TEXT("Num : %d"), Num);
			if (Num == CorrectDoorIndex)
			{
				//LS_LOG(LogLS, Log, TEXT("Correct Gate"));
				LeftSideCrowds[Num]->SetVisibility(false);
				LeftSideCrowds[Num]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				continue;
			}
			else
			{
				LeftSideCrowds[Num]->SetVisibility(true);
			}
		}
	}
	else
	{
		for (int32 Num = 0; Num < RightSideDoorLs.Num(); Num++)
		{
			//LS_LOG(LogLS, Log, TEXT("Num : %d"), Num);
			if (Num == CorrectDoorIndex)
			{
				//LS_LOG(LogLS, Log, TEXT("Correct Gate"));
				RightSideCrowds[Num]->SetVisibility(false);
				RightSideCrowds[Num]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				continue;
			}
			else
			{
				RightSideCrowds[Num]->SetVisibility(true);
				//Crowds[Num]->SetRelativeLocation(FVector(80, 80, 0));
				//LS_LOG(LogLS, Log, TEXT("Crowd Moved"));
			}
		}
	}
	
	CurrentPassengersAlpha = 0.0f;
	bisPassengersGettingOff = true;
}

void ALSTrain::GetOnPassengers()
{
	//LS_LOG(LogLS, Log, TEXT("Begin"));

	if (bisGateLeftSide)
	{
		for (int32 Num = 0; Num < LeftSideDoorLs.Num(); Num++)
		{
			LeftSideCrowds[Num]->SetVisibility(false);
			LeftSideCrowds[Num]->SetRelativeLocation(FVector(80, -40, 0));
			//LS_LOG(LogLS, Log, TEXT("Crowd Moved"));
		}
	}
	else
	{
		for (int32 Num = 0; Num < RightSideDoorLs.Num(); Num++)
		{
			RightSideCrowds[Num]->SetVisibility(false);
			RightSideCrowds[Num]->SetRelativeLocation(FVector(80, -40, 0));
			//LS_LOG(LogLS, Log, TEXT("Crowd Moved"));
		}
	}

	bisPassengersGettingOff = false;
}

void ALSTrain::StopTrain()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	if (HasAuthority())
	{
		CurrentTrainState = ETrainState::Stop;
	}
	if (GetWorld()->GetTimerManager().IsTimerActive(TrainTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(TrainTimerHandle);
		//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("TrainTimerHandle Cleared."));
	}
}

void ALSTrain::LeaveTrain()
{
	if (HasAuthority())
	{
		MulticastRPCPlayTrainAudioComponent(ETrainSoundState::StopGetOff);

		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
			{
				MulticastRPCPlayTrainAudioComponent(ETrainSoundState::Leave);
			}
		), TimeBeforeTrainLeave*0.9, false);
	}

	GetOnPassengers();
	GateClose();
}

void ALSTrain::PlayTrainAudioComponent(ETrainSoundState InTrainSoundState)
{
	if (InTrainSoundState == ETrainSoundState::Arrive)
	{
		if (TrainArriveAudioComponent)
		{
			TrainArriveAudioComponent->Play();
		}
		else
		{
			LS_LOG(LogLSls, Error, TEXT("%s"), TEXT("No TrainArriveAudioComponent!"));
		}
	}
	else if (InTrainSoundState == ETrainSoundState::GetOff)
	{
		if (GetOffAudioComponent)
		{
			GetOffAudioComponent->Play();
		}
		else
		{
			LS_LOG(LogLSls, Error, TEXT("%s"), TEXT("No GetOffAudioComponent!"));
		}
	}
	else if (InTrainSoundState == ETrainSoundState::StopGetOff)
	{
		if (GetOffAudioComponent)
		{
			GetOffAudioComponent->FadeOut(1.0f, 0.0f);
		}
		else
		{
			LS_LOG(LogLSls, Error, TEXT("%s"), TEXT("No GetOffAudioComponent!"));
		}
	}
	else
	{
		if (TrainLeaveAudioComponent)
		{
			TrainLeaveAudioComponent->Play();
		}
		else
		{
			LS_LOG(LogLSls, Error, TEXT("%s"), TEXT("No TrainLeaveAudioComponent!"));
		}
	}
}

void ALSTrain::MulticastRPCGateOpen_Implementation()
{
	GateOpen();
}

void ALSTrain::MulticastRPCGetOffPassengers_Implementation()
{
	GetOffPassengers();
}

void ALSTrain::MulticastRPCStopTrain_Implementation()
{
	StopTrain();
}

void ALSTrain::MulticastRPCLeaveTrain_Implementation()
{
	LeaveTrain();
}

void ALSTrain::MulticastRPCPlayTrainAudioComponent_Implementation(ETrainSoundState InTrainSoundState)
{
	PlayTrainAudioComponent(InTrainSoundState);
}
