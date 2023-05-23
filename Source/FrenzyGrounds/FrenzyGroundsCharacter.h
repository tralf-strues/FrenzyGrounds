// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GunInfo.h"
#include "FrenzyGroundsCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UAnimMontage;
class USoundBase;
class USpringArmComponent;

class AGunBase;
class APickUpItem;

UCLASS(config=Game)
class AFrenzyGroundsCharacter : public ACharacter
{
	GENERATED_BODY()

	/* First Person */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* AimingCameraComponent;

	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRepEquipGun, Category = "Shooting", meta = (AllowPrivateAccess = "true"))
	AGunBase* Gun;

	/* Third Person */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh3P;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh3P_Shadow;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh3P_Legs;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* DeathCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Shooting", meta = (AllowPrivateAccess = "true"))
	FRotator AimRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRepEquipGun3P, Category = "Shooting", meta = (AllowPrivateAccess = "true"))
	AGunBase* GunCosmetic3P;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRepEquipGunShadow, Category = "Shooting", meta = (AllowPrivateAccess = "true"))
	AGunBase* GunCosmetic3P_Shadow;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = Character, meta = (AllowPrivateAccess = "true"))
	int32 CurrentGunIdx = -1;

public:
	AFrenzyGroundsCharacter();

protected:
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds);

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooting")
	UAnimMontage* ShootAnimation1P;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooting")
	UAnimMontage* ShootAnimation3P;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = Character, meta = (AllowPrivateAccess = "true"))
	float Health = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = Character, meta = (AllowPrivateAccess = "true"))
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Shooting")
	TArray<FGunInfo> AvailiableGuns;

protected:
	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	float TakeDamage(const float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void StartShooting();
	void StopShooting();

	void StartAiming();
	void StopAiming();

	void Reload();

	void SwitchGun();

	void EquipGun(AActor* GunOwner, int32 NewGunIdx);

	UFUNCTION(Server, Reliable)
	void Server_EquipGun(AActor* GunOwner, int32 NewGunIdx);

	UFUNCTION()
	void OnRepEquipGun();

	UFUNCTION()
	void OnRepEquipGun3P();

	UFUNCTION()
	void OnRepEquipGunShadow();

	UFUNCTION(Server, Reliable)
	void Server_OnDeath();

	UFUNCTION(NetMulticast, Reliable)
	void MC_OnDeath();

	UFUNCTION(Client, Reliable)
	void Client_OnDeath();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION(Server, Reliable)
	void Server_ApplyPickUpItem(APickUpItem* PickUpItem);

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

public:
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	
	int32 GetCurrentGunIdx() const { return CurrentGunIdx; }

	AGunBase* GetGun() { return Gun; }
	AGunBase* GetGunCosmetic3P() { return GunCosmetic3P; }
	AGunBase* GetGunCosmetic3P_Shadow() { return GunCosmetic3P_Shadow; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	void OnShoot();
};

