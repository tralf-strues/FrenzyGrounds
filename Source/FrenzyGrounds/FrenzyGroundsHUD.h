// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FrenzyGroundsHUD.generated.h"

UCLASS()
class AFrenzyGroundsHUD : public AHUD
{
	GENERATED_BODY()

public:
	AFrenzyGroundsHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

