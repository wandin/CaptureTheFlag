// CTFFlag.cpp

#include "CTFFlag.h"
#include "Components/BoxComponent.h"
#include "CaptureTheFlag/Character/CTFCharacter.h"
#include "Net/UnrealNetwork.h"

ACTFFlag::ACTFFlag()
{
	FlagMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FlagMesh"));
	SetRootComponent(FlagMesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(RootComponent);
	CollisionBox->SetCollisionProfileName("OverlapAllDynamic");

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ACTFFlag::OnOverlapBegin);
	bReplicates = true;

	HeldBy = nullptr;
}

void ACTFFlag::BeginPlay()
{
	Super::BeginPlay();
}

void ACTFFlag::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACTFFlag::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if(OtherActor == this) return;
	ACTFCharacter* Character = Cast<ACTFCharacter>(OtherActor);
 	if(Character && !HeldBy && Character->bIsAlive)
	{
		HandleCapture(Character);
	}
}

void ACTFFlag::HandleCapture(ACTFCharacter* CapturingCharacter)
{
	if (!CapturingCharacter || !CapturingCharacter->bIsAlive) return;

	HeldBy = CapturingCharacter;
	HeldBy->HeldFlag = this;
	SetOwner(HeldBy);
	OnRep_HeldBy();
}

void ACTFFlag::DropFlag()
{
	if(HeldBy)
	{
		HandleDrop();
	}
}

void ACTFFlag::HandleDrop()
{
	HeldBy = nullptr;
	// Detach flag from character
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	// Ajustar as configurações de colisão
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Overlap);
	CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	OnRep_HeldBy();
}

void ACTFFlag::OnRep_HeldBy()
{
	if(HeldBy)
	{
		// Attach flag to character
		AttachToComponent(HeldBy->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "FlagSocket");
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		HeldBy->HeldFlag = this;
	}
	else
	{
		// Detach flag from character
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		// Ajustar as configurações de colisão
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		CollisionBox->SetCollisionResponseToAllChannels(ECR_Overlap);
		CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	}
}

void ACTFFlag::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACTFFlag, HeldBy);
}
