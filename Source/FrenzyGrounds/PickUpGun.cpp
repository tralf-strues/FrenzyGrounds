// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpGun.h"
#include "FrenzyGroundsCharacter.h"
#include "GunBase.h"

void APickUpGun::ApplyCustom(AFrenzyGroundsCharacter* character)
{
    UE_LOG(LogTemp, Warning, TEXT("APickUpGun::ApplyCustom(AmmoCurrent = %d, AmmoMax = %d)"), GunInfo.CurrentAmmo, GunInfo.MaxAmmo);

    auto& AvailiableGuns = character->AvailiableGuns;

    int32 GunIdx = -1;
    for (int32 i = 0; i < AvailiableGuns.Num(); ++i)
    {
        if (AvailiableGuns[i].GunBlueprint == GunInfo.GunBlueprint)
        {
            GunIdx = i;
            break;
        }
    }

    if (GunIdx == -1)
    {
        AvailiableGuns.Add(GunInfo);
    }
    else
    {
        if (GunIdx == character->GetCurrentGunIdx())
        {
            character->GetGun()->SetAmmoMax(character->GetGun()->GetAmmoMax() + GunInfo.MaxAmmo);
            character->GetGunCosmetic3P()->SetAmmoMax(character->GetGunCosmetic3P()->GetAmmoMax() + GunInfo.MaxAmmo);
            character->GetGunCosmetic3P_Shadow()->SetAmmoMax(character->GetGunCosmetic3P_Shadow()->GetAmmoMax() + GunInfo.MaxAmmo);
        }
        else
        {
            AvailiableGuns[GunIdx].MaxAmmo += GunInfo.MaxAmmo;
        }
    }
}
