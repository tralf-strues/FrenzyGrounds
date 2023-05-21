// Fill out your copyright notice in the Description page of Project Settings.


#include "GunAutomatic.h"
#include "TimerManager.h"

void AGunAutomatic::StartShooting()
{
    AGunBase::StartShooting();

    Shoot();
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AGunAutomatic::Shoot, ShootRate, true, ShootRate);
}

void AGunAutomatic::StopShooting()
{
    AGunBase::StopShooting();

    GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
    TimerHandle.Invalidate();
}
