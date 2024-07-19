// Fill out your copyright notice in the Description page of Project Settings.


#include "CTFProjectile.h"

#include "CaptureTheFlag/Character/CTFCharacter.h"
#include "CaptureTheFlag/PlayerController/CTFPlayerController.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACTFProjectile::ACTFProjectile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ACTFProjectile::OnHit);		// set up a notification for when this component hits something blocking


	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	
	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void ACTFProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACTFProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	APawn* InstigatorPawn = GetInstigator();
	ACTFPlayerController* InstigatorController = Cast<ACTFPlayerController>(GetInstigatorController());

	if (OtherActor)
	{
		ACTFCharacter* HitActor = Cast<ACTFCharacter>(OtherActor);

		if (HitActor && HitActor != InstigatorPawn)
		{
			UE_LOG(LogTemp, Log, TEXT("Projectile hit actor: %s"), *HitActor->GetName());

			if (InstigatorController)
			{
				UGameplayStatics::ApplyDamage(HitActor, Damage, InstigatorController, this, UDamageType::StaticClass());
			}
		}
	}
}

void ACTFProjectile::FireInDirection(const FVector& ShootDirection)
{
    ProjectileMovement->Velocity = ShootDirection.GetSafeNormal() * ProjectileMovement->InitialSpeed;
}