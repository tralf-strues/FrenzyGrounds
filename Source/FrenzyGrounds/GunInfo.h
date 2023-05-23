// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedStruct.h"
#include "GunInfo.generated.h"

class AGunBase;

USTRUCT(BlueprintType)
struct FGunInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gun")
	TSubclassOf<class AGunBase> GunBlueprint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo")
	int32 CurrentAmmo = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo")
	int32 MaxAmmo = 0;
};
