// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpHealth.h"
#include "FrenzyGroundsCharacter.h"


void APickUpHealth::ApplyCustom(AFrenzyGroundsCharacter* character)
{
    UE_LOG(LogTemp, Warning, TEXT("APickUpHealth::ApplyCustom(Heal = %d)"), Heal);

    character->Health += Heal;
    if (character->Health > character->MaxHealth)
    {
        character->Health = character->MaxHealth;
    }
}
