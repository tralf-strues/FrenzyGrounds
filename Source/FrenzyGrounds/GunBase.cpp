// Fill out your copyright notice in the Description page of Project Settings.


#include "GunBase.h"
#include "Components/ArrowComponent.h"
#include "Components/DecalComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "FrenzyGroundsCharacter.h"

// Sets default values
AGunBase::AGunBase()
{
    PrimaryActorTick.bCanEverTick = true;

    bReplicates = true;

    MeshGun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
    SetRootComponent(MeshGun);
    
    Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
    Arrow->SetupAttachment(MeshGun);

    GunSight = CreateDefaultSubobject<UCameraComponent>(TEXT("GunSight"));
    GunSight->SetupAttachment(MeshGun);
}

USkeletalMeshComponent* AGunBase::GetMesh()
{
    return MeshGun;
}

int32 AGunBase::GetAmmoCurrent() const
{
    return AmmoCurrent;
}

void AGunBase::SetAmmoCurrent(int32 NewAmmoCurrent)
{
    AmmoCurrent = NewAmmoCurrent;
}

int32 AGunBase::GetAmmoMax() const
{
    return AmmoMax;
}

void AGunBase::SetAmmoMax(int32 NewAmmoMax)
{
    AmmoMax = NewAmmoMax;
}

void AGunBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AGunBase, AmmoCurrent);
    DOREPLIFETIME(AGunBase, AmmoMax);
    DOREPLIFETIME(AGunBase, IsAiming);
}

void AGunBase::BeginPlay()
{
    Super::BeginPlay();
}

void AGunBase::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (IsAiming)
    {
        auto Character = dynamic_cast<AFrenzyGroundsCharacter*>(GetOwner());
        auto Controller = dynamic_cast<APlayerController*>(Character->GetController());

        if (Controller)
        {
            Character->GetFirstPersonCameraComponent()->SetWorldRotation(Controller->GetControlRotation());
        }
    }
}

void AGunBase::Shoot()
{
    APlayerController* Controller = dynamic_cast<APlayerController*>(dynamic_cast<AFrenzyGroundsCharacter*>(GetOwner())->GetController());

    FVector Direction{ 0.0f };
    if (!IsAiming)
    {
        int32 ViewportSizeX = 0;
        int32 ViewportSizeY = 0;
        Controller->GetViewportSize(ViewportSizeX, ViewportSizeY);

        FVector WorldLocation{};
        Controller->DeprojectScreenPositionToWorld(ViewportSizeX / 2, ViewportSizeY / 2 + 10.0f, WorldLocation, Direction);
    }
    else
    {
        Direction = Arrow->GetComponentRotation().Vector();
    }


    Server_Shoot(Controller, Arrow->GetComponentLocation(), Direction);
}

void AGunBase::StartShooting()
{
    IsShooting = true;
}

void AGunBase::StopShooting()
{
    IsShooting = false;
}

void AGunBase::OnShootVisualEffects()
{
    if (ShootAnimation)
    {
        MeshGun->PlayAnimation(ShootAnimation, false);
    }

    if (SB_Shoot)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), SB_Shoot, Arrow->GetComponentLocation());
    }

    if (PS_Shot)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PS_Shot, Arrow->GetComponentLocation(), Arrow->GetComponentRotation(), FVector(0.35f));
    }

    if (PS_ShotAdditional)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PS_ShotAdditional, Arrow->GetComponentLocation(), Arrow->GetComponentRotation(), FVector(0.2f));
    }
}

void AGunBase::StartAiming()
{
    IsAiming = true;

    //APlayerController* Controller = dynamic_cast<APlayerController*>(dynamic_cast<AFrenzyGroundsCharacter*>(GetOwner())->GetController());
    //if (Controller->IsLocalPlayerController())
    //{
    //    Controller->SetViewTargetWithBlend(this, 0.2f);
    //}
}

void AGunBase::StopAiming()
{
    IsAiming = false;

    //APlayerController* Controller = dynamic_cast<APlayerController*>(dynamic_cast<AFrenzyGroundsCharacter*>(GetOwner())->GetController());
    //if (Controller->IsLocalPlayerController())
    //{
    //    Controller->SetViewTargetWithBlend(GetOwner(), 0.2f);
    //}
}

void AGunBase::Server_Shoot_Implementation(APlayerController* PlayerController, FVector Origin, FVector Direction)
{
    if (AmmoCurrent <= 0)
    {
        return;
    }

    --AmmoCurrent;

    FHitResult HitResult;

    FCollisionQueryParams CollisionQueryParams;
    CollisionQueryParams.AddIgnoredActor(this);
    CollisionQueryParams.AddIgnoredActor(GetOwner());
    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Origin, Origin + ShootRange * Direction, ECC_Camera);
    
    MC_Shoot(HitResult, Direction);

    if (bHit)
    {
        FPointDamageEvent DamageEvent(Damage, HitResult, Direction, nullptr);
        HitResult.GetActor()->TakeDamage(Damage, DamageEvent, PlayerController, this);
    }
}

void AGunBase::MC_Shoot_Implementation(const FHitResult& HitResult, FVector Direction)
{
    if (HitResult.bBlockingHit && !HitResult.Actor->ActorHasTag("Player"))
    {
        if (M_DecalMaterial)
        {
            UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), M_DecalMaterial, FVector(3.0f), HitResult.Location, HitResult.ImpactNormal.Rotation(), 45.0f);
            Decal->SetFadeScreenSize(0.0f);

            FAttachmentTransformRules AttachmentRule = FAttachmentTransformRules::KeepWorldTransform;
            AttachmentRule.bWeldSimulatedBodies = true;
            Decal->AttachToComponent(HitResult.Component.Get(), AttachmentRule);
        }

        if (PS_Impact)
        {
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PS_Impact, HitResult.Location, HitResult.ImpactNormal.Rotation(), FVector(0.25f));
        }
    }
    else
    {
        //UKismetSystemLibrary::DrawDebugLine(GetWorld(), HitResult.TraceStart, HitResult.TraceStart + ShootRange * (HitResult.TraceEnd - HitResult.TraceStart), FLinearColor::Red, 3.0f, 2.0f);
    }

    dynamic_cast<AFrenzyGroundsCharacter*>(GetOwner())->OnShoot();
}

void AGunBase::Reload()
{
    if (AmmoCurrent == AmmoClipSize)
    {
        return;
    }

    Server_Reload();
}

void AGunBase::Server_Reload_Implementation()
{
    int32 BulletDifference = AmmoClipSize - AmmoCurrent;
    if (BulletDifference < AmmoMax)
    {
        AmmoCurrent = AmmoClipSize;
        AmmoMax -= BulletDifference;
    }
    else
    {
        AmmoCurrent += AmmoMax;
        AmmoMax = 0;
    }

    MC_Reload();
}

void AGunBase::MC_Reload_Implementation()
{
    if (ReloadAnimation)
    {
        MeshGun->PlayAnimation(ReloadAnimation, false);
    }

    if (SB_Reload)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), SB_Reload, MeshGun->GetComponentLocation());
    }
}

