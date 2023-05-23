// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickUpItem.h"
#include "GunInfo.h"
#include "PickUpGun.generated.h"

UCLASS()
class FRENZYGROUNDS_API APickUpGun : public APickUpItem
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Gun")
	FGunInfo GunInfo;

protected:
	void ApplyCustom(class AFrenzyGroundsCharacter* character) override;
};
