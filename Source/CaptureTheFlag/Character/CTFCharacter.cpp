// Fill out your copyright notice in the Description page of Project Settings.


#include "CTFCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "CaptureTheFlag//Flag/CTFFlag.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "CaptureTheFlag/PlayerController/CTFPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "CaptureTheFlag/GameModes/CTFGameMode.h"
#include "CaptureTheFlag/PlayerState/CTFPlayerState.h"
#include "CaptureTheFlag/SpawnArea/CTFSpawnArea.h"
#include "CaptureTheFlag/Weapons/CTFWeapon.h"
#include "Windows/WindowsApplication.h"

class UEnhancedInputLocalPlayerSubsystem;

ACTFCharacter::ACTFCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	/* Default Mesh */
	CharacterMesh = GetMesh();
	CharacterMesh->CastShadow = false;
	/* Mesh used for casting shadow as we are hiding the head from the default mesh */
	ShadowMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShadowMesh"));
	if(ShadowMesh)
	{
		ShadowMesh->SetupAttachment(RootComponent);
		ShadowMesh->bHiddenInGame = true;
		ShadowMesh->SetCastHiddenShadow(true);
	}
	// SpringArm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	if(SpringArm)
	{
		SpringArm->SetupAttachment(GetMesh(), "HeadSocket"); //attaching to the Head socket
		SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
		SpringArm->TargetArmLength = 0.f;
		SpringArm->bUsePawnControlRotation = true;
	}
	// Camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	if(Camera)
	{
		Camera->SetupAttachment(SpringArm);
	}
	MovementComponent = GetCharacterMovement();
	if(MovementComponent)
	{
		// Walk of Ledges
		MovementComponent->bCanWalkOffLedges = true;
		MovementComponent->bCanWalkOffLedgesWhenCrouching = true;
		MovementComponent->LedgeCheckThreshold = 0.0f;
	}
	HeldFlag = nullptr;
	bIsAlive = true;
}

void ACTFCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACTFCharacter, bWalking);
	DOREPLIFETIME(ACTFCharacter, AimPitch);
	DOREPLIFETIME(ACTFCharacter, Weapon);
	DOREPLIFETIME(ACTFCharacter, EquippedWeaponType);
	DOREPLIFETIME(ACTFCharacter, Health);
	DOREPLIFETIME(ACTFCharacter, HeldFlag);
	DOREPLIFETIME(ACTFCharacter, bIsAlive);
}

void ACTFCharacter::BeginPlay()
{
	Super::BeginPlay();

	if(ACTFPlayerController* PC = Cast<ACTFPlayerController>(GetController()))
	{
		/* Mapping Input*/
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			UE_LOG(LogTemp, Warning, TEXT("Beginplay - Subsystem is valid!"));
			Subsystem->AddMappingContext(PlayerMappingContext, 0);
		}
	}
	
	CTFPlayerController = Cast<ACTFPlayerController>(GetController());
	if(CTFPlayerController)
	{
		CTFPlayerController->CreateGameplayWidget();
	}
	
	if (IsLocallyControlled() && GetMesh())
	{
		// Hide Head bone to not overlap with camera
		GetMesh()->HideBoneByName("head", PBO_None);
	}
	
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ACTFCharacter::ACTFCharacter::OnOverlapBegin);
	OnTakeAnyDamage.AddDynamic(this, &ACTFCharacter::ReceiveDamage);
}

void ACTFCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CalculateAimDirection();
	UpdateHUDHealth();
}

void ACTFCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
    	// Bind inputs
    	EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACTFCharacter::Move);
    	EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACTFCharacter::Look);
    	EIC->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ACTFCharacter::CrouchPressed);
    	EIC->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ACTFCharacter::CrouchReleased);
    	EIC->BindAction(WalkAction, ETriggerEvent::Triggered, this, &ACTFCharacter::WalkPressed);
    	EIC->BindAction(WalkAction, ETriggerEvent::Completed, this, &ACTFCharacter::WalkReleased);
    	EIC->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACTFCharacter::Jump);
    	EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACTFCharacter::StopJumping);
    	
    	EIC->BindAction(FireWeaponAction, ETriggerEvent::Started, this, &ACTFCharacter::FireButtonPressed);
    }	
}

ACTFCharacter* ACTFCharacter::GetCharacterRef()
{
	return this;
}

void ACTFCharacter::InitializeCharacter(AController* NewController)
{
	APlayerController* PC = Cast<APlayerController>(NewController);
	if (PC)
	{
		EnableInput(PC);

		CTFPlayerController = Cast<ACTFPlayerController>(PC);
		if (CTFPlayerController)
		{
			CTFPlayerController->CreateGameplayWidget();
		}
	}
}

void ACTFCharacter::OnRep_Health()
{
	UpdateHUDHealth();
}

void ACTFCharacter::CalculateAimDirection()
{
	if (HasAuthority())
	{
		Server_CalculateAimDirection();
	}
	FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), GetActorRotation());
	AimPitch = DeltaRotation.Pitch;
}

void ACTFCharacter::Server_CalculateAimDirection_Implementation()
{
	FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), GetActorRotation());
	AimPitch = DeltaRotation.Pitch;
	Multicast_UpdateAimPitch(AimPitch); // Call Multicast to update AimPitch in all clients

}
void ACTFCharacter::Multicast_UpdateAimPitch_Implementation(float NewAimPitch)
{
	AimPitch = NewAimPitch;
}

bool ACTFCharacter::CanJumpInternal_Implementation() const
{
	if(bIsCrouched && !MovementComponent->IsFalling())
	{
		return true;
	}
	return Super::CanJumpInternal_Implementation();
}

void ACTFCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	if(JumpLandingSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, JumpLandingSound, GetActorLocation());
	}
}

void ACTFCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACTFWeapon* OverlappingWeapon = Cast<ACTFWeapon>(OtherActor))
	{
		EquipWeapon(OverlappingWeapon);
	}
	if (ACTFSpawnArea* TeamBase = Cast<ACTFSpawnArea>(OtherActor))
	{
		HandleTeamBaseOverlap(TeamBase);
	}
}

void ACTFCharacter::Move(const FInputActionValue& Value)
{
	const FVector2d MovementVector = Value.Get<FVector2d>();
	AddMovementInput(GetActorForwardVector(), MovementVector.X);
	AddMovementInput(GetActorRightVector(), MovementVector.Y);
}

void ACTFCharacter::Look(const FInputActionValue& Value)
{
	// AddControllerPitchInput takes a Vector
	const FVector2d LookAxisVector = Value.Get<FVector2d>();
	APlayerController* PC = Cast<APlayerController>(GetController());
	if(PC)
	{
		PC->AddPitchInput(LookAxisVector.Y);
	}
	AddControllerYawInput(LookAxisVector.X);
}

void ACTFCharacter::CrouchPressed()
{
	Crouch();
}

void ACTFCharacter::CrouchReleased()
{
	UnCrouch();
}

void ACTFCharacter::WalkPressed()
{
	if(IsLocallyControlled())
	{
		Server_WalkPressed();
	}
	bWalking = true;
	GetCharacterMovement()->MaxWalkSpeed = 220.f;
}

void ACTFCharacter::WalkReleased()
{
	if(IsLocallyControlled())
	{
		Server_WalkReleased();
	}
	bWalking = false;
	GetCharacterMovement()->MaxWalkSpeed = 420.f;
}

void ACTFCharacter::Server_WalkPressed_Implementation()
{
	bWalking = true;
	GetCharacterMovement()->MaxWalkSpeed = 220.f;
}

void ACTFCharacter::Server_WalkReleased_Implementation()
{
	bWalking = false;
	GetCharacterMovement()->MaxWalkSpeed = 420.f;
}

void ACTFCharacter::Jump()
{	
	if(CanJumpInternal_Implementation())
	{
		if(JumpLandingSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, JumpLandingSound, GetActorLocation());
		}
		Super::Jump();
	}
}

void ACTFCharacter::StopJumping()
{
	Super::StopJumping();
}

void ACTFCharacter::FireButtonPressed()
{
	FireWeapon();
}

void ACTFCharacter::EquipWeapon(ACTFWeapon* PrimaryWeaponToEquip)
{
	if(PrimaryWeaponToEquip == nullptr) return;
	
	if(HasAuthority() && !Weapon)
	{
		Weapon = PrimaryWeaponToEquip;
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "HandSocket_AK47");
		Weapon->SetOwner(this);
		if(Weapon)
		{
			EquippedWeaponType = Weapon->WeaponProperties.WeaponType;
		}
	}
	else
	{
		Server_EquipWeapon(PrimaryWeaponToEquip);
	}
	OnRep_EquippedWeapon();
}

void ACTFCharacter::Server_EquipWeapon_Implementation(ACTFWeapon* PrimaryWeaponToEquip)
{
	if(PrimaryWeaponToEquip && !Weapon)
	{
		Weapon = PrimaryWeaponToEquip;
		Weapon->SetOwner(this);
		EquipWeapon(PrimaryWeaponToEquip);
		OnRep_EquippedWeapon();
	}
}

void ACTFCharacter::OnRep_EquippedWeapon()
{
	// Handle the replication of the primary weapon (e.g., attach it to the character's mesh).
	if(Weapon)
	{
		Weapon->SetOwner(this);
		Weapon->WeaponMesh->SetSimulatePhysics(false);
		Weapon->WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Weapon->CapsuleCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "HandSocket_AK47");

		EquippedWeaponType = Weapon->WeaponProperties.WeaponType;
	}
}

void ACTFCharacter::FireWeapon()
{
	if(Weapon)
	{
		if(HasAuthority())
		{
			Weapon->Fire();
			Multicast_PlayFireEffects();
		}
		else
		{
			Server_FireWeapon();
		}
	}
}

void ACTFCharacter::Server_FireWeapon_Implementation()
{
	FireWeapon();
	Multicast_PlayFireEffects();
}

void ACTFCharacter::Multicast_PlayFireEffects_Implementation()
{
	if(Weapon)
	{
		Weapon->Multicast_PlayFireEffects();
	}
}

void ACTFCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatorController, AActor* DamageCauser)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	UpdateHUDHealth();

	if(Health <= 0.f)
	{
		bIsAlive = false;
		DropFlag();
		if(Weapon)
		{
			Weapon->Destroy();
		}
		
		
		CTFPlayerController = CTFPlayerController == nullptr ? Cast<ACTFPlayerController>(Controller) : CTFPlayerController;
		ACTFPlayerController* AttackerController = Cast<ACTFPlayerController>(InstigatorController);
		ACTFGameModeBase* CTFGameMode = GetWorld()->GetAuthGameMode<ACTFGameModeBase>();
		if(CTFGameMode)
		{
			if(AttackerController)
			{
				CTFGameMode->PlayerEliminated(Cast<ACTFCharacter>(DamagedActor), CTFPlayerController, AttackerController);
			}
		}
	}
}

void ACTFCharacter::DropFlag()
{
	if (HeldFlag)
	{
		HeldFlag->DropFlag();
		if(IsLocallyControlled())
		{
			HeldFlag = nullptr;
		}
	}
}

void ACTFCharacter::UpdateHUDHealth()
{
	CTFPlayerController = CTFPlayerController == nullptr ? Cast<ACTFPlayerController>(Controller) : CTFPlayerController;
	if(CTFPlayerController)
	{
		CTFPlayerController->UpdatePlayerHealth(Health, MaxHealth);
	}
}

void ACTFCharacter::HandleTeamBaseOverlap(ACTFSpawnArea* TeamBase)
{
	if (!TeamBase) return;
	if (!HeldFlag) return;
	
	ACTFGameModeBase* CTFGameMode = GetWorld()->GetAuthGameMode<ACTFGameModeBase>();
	ACTFPlayerState* CTFPlayerState = Cast<ACTFPlayerState>(GetPlayerState());
	CTFPlayerController = Cast<ACTFPlayerController>(GetController());

	if (!CTFGameMode || !CTFPlayerState || !CTFPlayerController) return;

	if (TeamBase->TeamID == -1) // if TeamBase->TeamID == -1 it means it's the flag SpawnArea -> do nothing.
	{
		UE_LOG(LogTemp, Warning, TEXT("Overlapping Flag SpawnArea"));
		return;
	}
	if(HeldFlag)
	{
		DropFlag();
		if (TeamBase->TeamID == TeamID)
		{		
			CTFGameMode->OnTeamScored(this, CTFPlayerState, CTFPlayerController);
		}
	}
}