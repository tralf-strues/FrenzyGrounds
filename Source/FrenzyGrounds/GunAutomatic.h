// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GunBase.h"
#include "GunAutomatic.generated.h"

UCLASS()
class FRENZYGROUNDS_API AGunAutomatic : public AGunBase
{
	GENERATED_BODY()

private:
	FTimerHandle TimerHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gun", meta = (AllowPrivateAccess = "true"))
	float ShootRate = 0.15f;
	
public:
	void StartShooting() override;
	void StopShooting() override;
};
