// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GunBase.generated.h"

class USkeletalMeshComponent;
class UArrowComponent;
class UCameraComponent;
class USoundBase;

UCLASS()
class FRENZYGROUNDS_API AGunBase : public AActor
{
protected:
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
    USkeletalMeshComponent* MeshGun;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gun", meta = (AllowPrivateAccess = "true"))
    UArrowComponent* Arrow;

    UPROPERTY(EditDefaultsOnly, Category = "Gun")
    UCameraComponent* GunSight;

    UPROPERTY(EditDefaultsOnly, Category = "Visuals")
    UAnimationAsset* ShootAnimation;

    UPROPERTY(EditDefaultsOnly, Category = "Visuals")
    UAnimationAsset* ReloadAnimation;

    UPROPERTY(EditDefaultsOnly, Category = "Visuals")
    USoundBase* SB_Shoot;

    UPROPERTY(EditDefaultsOnly, Category = "Visuals")
    USoundBase* SB_Reload;

    UPROPERTY(EditDefaultsOnly, Category = "Visuals")
    UParticleSystem* PS_Shot;

    UPROPERTY(EditDefaultsOnly, Category = "Visuals")
    UParticleSystem* PS_ShotAdditional;

    UPROPERTY(EditDefaultsOnly, Category = "Visuals")
    UParticleSystem* PS_Impact;

    UPROPERTY(EditDefaultsOnly, Category = "Visuals")
    UMaterialInterface* M_DecalMaterial;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gun", meta = (AllowPrivateAccess = "true"))
    FString Name {"Gun"};

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gun")
    bool IsShooting = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Gun")
    bool IsAiming = false;

    UPROPERTY(EditDefaultsOnly, Category = "Gun")
    float ShootRange = 100000.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Gun")
    float Damage = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
    int32 AmmoCurrent = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
    int32 AmmoMax = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
    int32 AmmoClipSize = 0;

public:
    // Sets default values for this actor's properties
    AGunBase();

public:
    USkeletalMeshComponent* GetMesh();

    int32 GetAmmoCurrent() const;
    void SetAmmoCurrent(int32 NewAmmoCurrent);

    int32 GetAmmoMax() const;
    void SetAmmoMax(int32 NewAmmoMax);

    virtual void StartShooting();
    virtual void StopShooting();
    virtual void OnShootVisualEffects();

    virtual void StartAiming();
    virtual void StopAiming();

    virtual void Reload();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

protected:
    virtual void BeginPlay();
    virtual void Tick(float DeltaSeconds);

    virtual void Shoot();

    UFUNCTION(Server, Reliable)
    virtual void Server_Shoot(APlayerController* PlayerController, FVector Origin, FVector Direction);

    UFUNCTION(NetMulticast, Reliable)
    virtual void MC_Shoot(const FHitResult& HitResult, FVector Direction);

    UFUNCTION(Server, Reliable)
    virtual void Server_Reload();

    UFUNCTION(NetMulticast, Reliable)
    virtual void MC_Reload();
};