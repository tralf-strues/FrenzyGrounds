// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickUpItem.h"
#include "PickUpHealth.generated.h"

UCLASS()
class FRENZYGROUNDS_API APickUpHealth : public APickUpItem
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Heal")
	int32 Heal{ 1 };

protected:
	void ApplyCustom(class AFrenzyGroundsCharacter* character) override;
};
