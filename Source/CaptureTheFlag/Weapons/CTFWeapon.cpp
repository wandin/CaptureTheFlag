// Fill out your copyright notice in the Description page of Project Settings.


#include "CTFWeapon.h"

#include "CaptureTheFlag/Character/CTFCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACTFWeapon::ACTFWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;
	WeaponMesh->SetCollisionObjectType(ECC_WorldStatic);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	WeaponMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	WeaponMesh->SetSimulatePhysics(true);
	WeaponMesh->SetEnableGravity(true);
	
	CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollision"));
	CapsuleCollision->SetupAttachment(WeaponMesh);
	CapsuleCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	bReplicates = true;
	AActor::SetReplicateMovement(true);
	WeaponProperties.WeaponName = "DefaultWeapon";
}

void ACTFWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACTFWeapon, MuzzleFlash);
	DOREPLIFETIME(ACTFWeapon, FireSound);
}

void ACTFWeapon::Fire()
{
	if (GetWorld() && WeaponProperties.ProjectileClass)
	{
		if (UCameraComponent* Camera = GetOwner()->FindComponentByClass<UCameraComponent>())
		{
			ACTFCharacter* OwnerCharacter = Cast<ACTFCharacter>(GetOwner());
			FVector StartLocation = OwnerCharacter->GetMesh()->GetBoneLocation(FName("pelvis"));

			// Mover o StartLocation um pouco à frente do personagem
			FVector ForwardVector = Camera->GetForwardVector();
			StartLocation += ForwardVector * 100.0f; // Ajuste a distância conforme necessário

			FRotator FireDirection = Camera->GetComponentRotation();
			FVector EndLocation = StartLocation + FireDirection.Vector() * 10000.f;

			FActorSpawnParameters SpawnParams;
			SpawnParams.Instigator = Cast<APawn>(GetOwner());
			SpawnParams.Owner = this;
	
			ACTFProjectile* Projectile = GetWorld()->SpawnActor<ACTFProjectile>(WeaponProperties.ProjectileClass, StartLocation, FireDirection, SpawnParams);
			if (Projectile)
			{
				FVector ShootDirection = EndLocation - StartLocation;
				Projectile->FireInDirection(ShootDirection);
			}
			Multicast_PlayFireEffects();

			DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 2.f);
		}
	}
}

void ACTFWeapon::Multicast_PlayFireEffects_Implementation()
{
	// Play fire sound specified
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}
	// Play muzzle flash if specified
	if (MuzzleFlash)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, WeaponMesh, FName("MuzzleSocket"));
	}
}
