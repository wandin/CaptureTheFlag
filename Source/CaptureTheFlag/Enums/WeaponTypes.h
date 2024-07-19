#pragma once


UENUM(BlueprintType)
enum class EWeaponTypes : uint8
{
	EWT_Unarmed		UMETA(DisplayName = "Unarmed"),
	EWT_Pistol		UMETA(DisplayName = "Pistol"),
	EWT_Rifle		UMETA(DisplayName = "Rifle"),
	EWT_MAX			UMETA(DisplayName = "DefaultMAX")
};