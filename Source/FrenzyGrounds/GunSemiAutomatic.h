// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GunBase.h"
#include "GunSemiAutomatic.generated.h"

/**
 * 
 */
UCLASS()
class FRENZYGROUNDS_API AGunSemiAutomatic : public AGunBase
{
	GENERATED_BODY()

public:
	void StartShooting() override;
};
