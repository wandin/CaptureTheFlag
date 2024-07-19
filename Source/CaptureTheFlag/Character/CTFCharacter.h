// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "CaptureTheFlag//Interfaces/CharacterInterface.h"
#include "CaptureTheFlag/Enums/WeaponTypes.h"
#include "CaptureTheFlag/Flag/CTFFlag.h"
#include "GameFramework/Character.h"
#include "CTFCharacter.generated.h"

class ACTFSpawnArea;
class ACTFWeapon;
class USphereComponent;
class ACTFPlayerController;
class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;


UCLASS()
class CAPTURETHEFLAG_API ACTFCharacter : public ACharacter, public ICharacterInterface
{
	GENERATED_BODY()

public:
	ACTFCharacter();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual ACTFCharacter* GetCharacterRef() override;
	
	void InitializeCharacter(AController* NewController);

	// Overriding CanJumpInternal function to allow jumping while crouched.
	virtual bool CanJumpInternal_Implementation() const override;
	// We are overriding this function mainly to Play a sound At location when the Character lands
	virtual void Landed(const FHitResult& Hit) override;

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
		
	// Default Character Mesh (inherited mesh from ACharacter class) - The mesh we see (declaring here to not use GetMesh() all the time)
	UPROPERTY()
	USkeletalMeshComponent* CharacterMesh;
	// A Mesh for casting a proper shadow, as in the (inherited mesh from ACharacter class) we are hiding the Defauolt Mesh's head to attach a camera
	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* ShadowMesh;
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void CrouchPressed();			// handles Crouch input Pressed
	void CrouchReleased();			// handles Crouch input Released
	void WalkPressed();
	void WalkReleased();
	UFUNCTION(Server, Unreliable)
	void Server_WalkPressed();		// Handles replicating the movement while Press Walking
	UFUNCTION(Server, Unreliable)
	void Server_WalkReleased();		// Handles replicating the movement while Releasing Walking
	virtual void Jump() override;
	virtual void StopJumping() override;
	
	void FireButtonPressed();


	UFUNCTION()
    void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);
    void UpdateHUDHealth();

private:
	/*		Spring Arm & Camera */
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	USpringArmComponent* SpringArm;
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	UCameraComponent* Camera;

	// To simplify avoid calling GetCharacterMovement() all the time
	UPROPERTY()
	UCharacterMovementComponent* MovementComponent;

	/* INPUTS */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* PlayerMappingContext;


	/*		Inputs		*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* WalkAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* FireWeaponAction;
	
	/* Player Health */
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;
	
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Health, Category = "Player Stats")
	float Health = 100.f;
	
	UFUNCTION()
	void OnRep_Health();
	
	
	//	Functions
	UFUNCTION()
	void CalculateAimDirection();
	UFUNCTION(Server, Reliable)
	void Server_CalculateAimDirection();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateAimPitch(float NewAimPitch);


	/* WEAPON */
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	ACTFWeapon* Weapon;
	void EquipWeapon(ACTFWeapon* PrimaryWeaponToEquip);
	UFUNCTION(Server, Reliable)
	void Server_EquipWeapon(ACTFWeapon* PrimaryWeaponToEquip);
	UFUNCTION()
	void OnRep_EquippedWeapon();


	//void AttachFlag(ACTFFlag* Flag);

	void DropFlag();
	
	void HandleTeamBaseOverlap(ACTFSpawnArea* TeamBase);
	
	UPROPERTY()
	ACTFPlayerController* CTFPlayerController;
	
	UPROPERTY(EditDefaultsOnly)
	int32 TeamID;


	
public:
	
	UPROPERTY(Replicated)
	bool bIsAlive;

	UPROPERTY(Replicated)
	ACTFFlag* HeldFlag;
	
	void FireWeapon();

	UFUNCTION(Server, Reliable)
	void Server_FireWeapon();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayFireEffects();
	
	UPROPERTY(Replicated)
	float AimPitch;
	
	UPROPERTY(Replicated)
	bool bWalking;

	UPROPERTY(EditDefaultsOnly)
	USoundBase* JumpLandingSound;

	// Equipped Weapon type
	UPROPERTY(Replicated)
	EWeaponTypes EquippedWeaponType;


	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetHealth() const { return Health; }
	
	FORCEINLINE EWeaponTypes GetWeaponType() const { return EquippedWeaponType; }
	FORCEINLINE float GetAimDirection() const { return AimPitch; }
	
	FORCEINLINE ACTFWeapon* GetWeapon() const { return Weapon; }


	FORCEINLINE int32 GetTeamID() const { return TeamID; }
};