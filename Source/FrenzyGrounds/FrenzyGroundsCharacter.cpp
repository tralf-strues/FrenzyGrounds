// Copyright Epic Games, Inc. All Rights Reserved.

#include "FrenzyGroundsCharacter.h"
#include "FrenzyGroundsProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "Net/UnrealNetwork.h"
#include "GunBase.h"
#include "PickUpItem.h"
#include "Components/ChildActorComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

AFrenzyGroundsCharacter::AFrenzyGroundsCharacter()
{
	bReplicates = true;

	Tags.Push("Player");

	auto CachedCapsuleComponent = GetCapsuleComponent();
	CachedCapsuleComponent->InitCapsuleSize(55.f, 96.0f);

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	/* First Person */
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(CachedCapsuleComponent);
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	AimingCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("AimingCamera"));
	AimingCameraComponent->SetupAttachment(CachedCapsuleComponent);
	AimingCameraComponent->bUsePawnControlRotation = true;
	AimingCameraComponent->bAutoActivate = false;

	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh1P"));
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;

	/* Third Person */
	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh3P"));
	Mesh3P->SetupAttachment(CachedCapsuleComponent);
	Mesh3P->SetOwnerNoSee(true);

	Mesh3P_Shadow = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh3P_Shadow"));
	Mesh3P_Shadow->SetupAttachment(CachedCapsuleComponent);
	Mesh3P_Shadow->SetOnlyOwnerSee(true);
	Mesh3P_Shadow->SetRenderInMainPass(false);

	Mesh3P_Legs = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh3P_Legs"));
	Mesh3P_Legs->SetupAttachment(CachedCapsuleComponent);
	Mesh3P_Legs->SetOnlyOwnerSee(true);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(Mesh3P);
	SpringArm->bUsePawnControlRotation = true;

	DeathCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("DeathCamera"));
	DeathCameraComponent->SetupAttachment(SpringArm);
	DeathCameraComponent->bAutoActivate = false;
}

void AFrenzyGroundsCharacter::BeginPlay()
{
	Super::BeginPlay();

	Mesh3P_Legs->HideBoneByName("spine_01", PBO_None);

	UE_LOG(LogTemp, Warning, TEXT("Taking damage GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic()"));
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AFrenzyGroundsCharacter::OnOverlapBegin);
}

void AFrenzyGroundsCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (HasAuthority())
	{
		AimRotation = GetControlRotation();

		if (Gun == nullptr && AvailiableGuns.Num() > 0)
		{
			EquipGun(this, 0);
		}
	}
}

void AFrenzyGroundsCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFrenzyGroundsCharacter::StartShooting);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AFrenzyGroundsCharacter::StopShooting);

	// Bind aim event
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AFrenzyGroundsCharacter::StartAiming);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AFrenzyGroundsCharacter::StopAiming);

	// Bind reload event
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AFrenzyGroundsCharacter::Reload);

	// Bind switch gun event
	PlayerInputComponent->BindAction("SwitchGun", IE_Released, this, &AFrenzyGroundsCharacter::SwitchGun);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AFrenzyGroundsCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFrenzyGroundsCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AFrenzyGroundsCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AFrenzyGroundsCharacter::LookUpAtRate);
}

void AFrenzyGroundsCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFrenzyGroundsCharacter, Health);
	DOREPLIFETIME(AFrenzyGroundsCharacter, MaxHealth);
	DOREPLIFETIME(AFrenzyGroundsCharacter, AimRotation);
	DOREPLIFETIME(AFrenzyGroundsCharacter, Gun);
	DOREPLIFETIME(AFrenzyGroundsCharacter, GunCosmetic3P);
	DOREPLIFETIME(AFrenzyGroundsCharacter, GunCosmetic3P_Shadow);
	DOREPLIFETIME(AFrenzyGroundsCharacter, CurrentGunIdx);
	DOREPLIFETIME(AFrenzyGroundsCharacter, AvailiableGuns);
}

void AFrenzyGroundsCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AFrenzyGroundsCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AFrenzyGroundsCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AFrenzyGroundsCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

float AFrenzyGroundsCharacter::TakeDamage(const float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Taking damage %f"), DamageAmount);

	if (EventInstigator == GetInstigatorController())
	{
		UE_LOG(LogTemp, Warning, TEXT("Ignoring damage"), DamageAmount);

		return 0.0f;
	}

	Health -= DamageAmount;

	if (Health <= 0.0f)
	{
		Server_OnDeath();
	}

	return DamageAmount;
}

void AFrenzyGroundsCharacter::Server_OnDeath_Implementation()
{
	Client_OnDeath();
	MC_OnDeath();
}

void AFrenzyGroundsCharacter::MC_OnDeath_Implementation()
{
	Mesh3P->SetSimulatePhysics(true);
	Mesh3P->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetCharacterMovement()->DisableMovement();

	Health = 0.0f;

	Gun->SetActorHiddenInGame(true);
	GunCosmetic3P->SetActorHiddenInGame(true);
	GunCosmetic3P_Shadow->SetActorHiddenInGame(true);
}

void AFrenzyGroundsCharacter::Client_OnDeath_Implementation()
{
	Mesh1P->SetOwnerNoSee(true);

	Mesh3P->SetOwnerNoSee(false);
	Mesh3P_Shadow->SetOwnerNoSee(true);
	Mesh3P_Legs->SetOwnerNoSee(true);

	FirstPersonCameraComponent->Deactivate();
	AimingCameraComponent->Deactivate();
	DeathCameraComponent->Activate();

	DisableInput(dynamic_cast<APlayerController*>(GetController()));
}

void AFrenzyGroundsCharacter::StartShooting()
{
	if (Gun)
	{
		Gun->StartShooting();
	}
}

void AFrenzyGroundsCharacter::StopShooting()
{
	if (Gun)
	{
		Gun->StopShooting();
	}
}

void AFrenzyGroundsCharacter::StartAiming()
{
	if (Gun)
	{
		FirstPersonCameraComponent->Deactivate();
		AimingCameraComponent->Activate();

		Gun->StartAiming();
	}
}

void AFrenzyGroundsCharacter::StopAiming()
{
	if (Gun)
	{
		AimingCameraComponent->Deactivate();
		FirstPersonCameraComponent->Activate();

		Gun->StopAiming();
	}
}

void AFrenzyGroundsCharacter::Reload()
{
	if (Gun)
	{
		Gun->Reload();
	}
}

void AFrenzyGroundsCharacter::SwitchGun()
{
	UE_LOG(LogTemp, Warning, TEXT("SwitchGun to %d"), (CurrentGunIdx + 1) % AvailiableGuns.Num());

	if (AvailiableGuns.Num() > 1)
	{
		EquipGun(this, (CurrentGunIdx + 1) % AvailiableGuns.Num());
	}
}

void AFrenzyGroundsCharacter::EquipGun(AActor* GunOwner, int32 NewGunIdx)
{
	UE_LOG(LogTemp, Warning, TEXT("EquipGun with authority = %d"), HasAuthority());

	if (!HasAuthority())
	{
		Server_EquipGun(GunOwner, NewGunIdx);
	}
	else
	{
		if (CurrentGunIdx != -1)
		{
			UE_LOG(LogTemp, Warning, TEXT("Saving ammo info, current = %d, max = %d"), Gun->GetAmmoCurrent(), Gun->GetAmmoMax());

			AvailiableGuns[CurrentGunIdx].CurrentAmmo = Gun->GetAmmoCurrent();
			AvailiableGuns[CurrentGunIdx].MaxAmmo = Gun->GetAmmoMax();

			Gun->Destroy();
			GunCosmetic3P->Destroy();
			GunCosmetic3P_Shadow->Destroy();
		}

		CurrentGunIdx = NewGunIdx;

		Gun = GetWorld()->SpawnActor<AGunBase>(AvailiableGuns[CurrentGunIdx].GunBlueprint);
		Gun->SetOwner(GunOwner);
		Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("GripPointRifle1P"));

		Gun->SetAmmoCurrent(AvailiableGuns[CurrentGunIdx].CurrentAmmo);
		Gun->SetAmmoMax(AvailiableGuns[CurrentGunIdx].MaxAmmo);

		GunCosmetic3P = GetWorld()->SpawnActor<AGunBase>(AvailiableGuns[CurrentGunIdx].GunBlueprint);
		GunCosmetic3P->SetOwner(GunOwner);
		GunCosmetic3P->AttachToComponent(Mesh3P, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("GripPointRifleHip"));

		GunCosmetic3P->SetAmmoCurrent(AvailiableGuns[CurrentGunIdx].CurrentAmmo);
		GunCosmetic3P->SetAmmoMax(AvailiableGuns[CurrentGunIdx].MaxAmmo);

		GunCosmetic3P_Shadow = GetWorld()->SpawnActor<AGunBase>(AvailiableGuns[CurrentGunIdx].GunBlueprint);
		GunCosmetic3P_Shadow->SetOwner(GunOwner);
		GunCosmetic3P_Shadow->AttachToComponent(Mesh3P_Shadow, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("GripPointRifleHip"));

		OnRepEquipGun();
		OnRepEquipGun3P();
		OnRepEquipGunShadow();
	}
}

void AFrenzyGroundsCharacter::Server_EquipGun_Implementation(AActor* GunOwner, int32 NewGunIdx)
{
	EquipGun(GunOwner, NewGunIdx);
}

void AFrenzyGroundsCharacter::OnRepEquipGun()
{
	if (Gun)
	{
		//Gun->SetCosmeticOnly(false);
		Gun->GetMesh()->SetOnlyOwnerSee(true);
		Gun->GetMesh()->bCastDynamicShadow = false;
		Gun->GetMesh()->CastShadow = false;

		AimingCameraComponent->AttachToComponent(Gun->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("GunSightSocket"));
	}
}

void AFrenzyGroundsCharacter::OnRepEquipGun3P()
{
	if (GunCosmetic3P)
	{
		//GunCosmetic3P->SetCosmeticOnly(false);
		GunCosmetic3P->GetMesh()->SetOwnerNoSee(true);
	}
}

void AFrenzyGroundsCharacter::OnRepEquipGunShadow()
{
	if (GunCosmetic3P_Shadow)
	{
		//GunCosmetic3P_Shadow->SetCosmeticOnly(false);
		GunCosmetic3P_Shadow->GetMesh()->SetOnlyOwnerSee(true);
		GunCosmetic3P_Shadow->GetMesh()->SetRenderInMainPass(false);
	}
}

void AFrenzyGroundsCharacter::OnShoot()
{
	bool IsFirstPerson = IsLocallyControlled();

	UE_LOG(LogTemp, Warning, TEXT("OnShoot, LocallyControlled = %d"), IsFirstPerson);

	if (ShootAnimation1P && IsFirstPerson)
	{
		Mesh1P->GetAnimInstance()->Montage_Play(ShootAnimation1P);

		Gun->OnShootVisualEffects();
	}

	if (ShootAnimation3P && !IsFirstPerson)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnShoot, ShootAnimation3P"));

		Mesh3P->GetAnimInstance()->Montage_Play(ShootAnimation3P);
		Mesh3P_Shadow->GetAnimInstance()->Montage_Play(ShootAnimation3P);

		GunCosmetic3P->OnShootVisualEffects();
	}
}

void AFrenzyGroundsCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("OnOverlapBegin"));

	if (OtherActor->ActorHasTag("PickUpItem"))
	{
		Server_ApplyPickUpItem(dynamic_cast<APickUpItem*>(OtherActor));
	}
}

void AFrenzyGroundsCharacter::Server_ApplyPickUpItem_Implementation(APickUpItem* PickUpItem)
{
	PickUpItem->Apply(this);
}

