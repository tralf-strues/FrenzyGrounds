// Copyright Epic Games, Inc. All Rights Reserved.

#include "FrenzyGroundsGameMode.h"
#include "FrenzyGroundsHUD.h"
#include "FrenzyGroundsCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFrenzyGroundsGameMode::AFrenzyGroundsGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Players/SwatSoldier/BP_SwatSoldier"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFrenzyGroundsHUD::StaticClass();
}
