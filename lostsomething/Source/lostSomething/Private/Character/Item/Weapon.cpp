// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Item/Weapon.h"

// AWeapon.cpp
AWeapon::AWeapon()
{
    PrimaryActorTick.bCanEverTick = false;

    WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
    RootComponent = WeaponMesh;

    WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
}
