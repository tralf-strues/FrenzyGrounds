// Fill out your copyright notice in the Description page of Project Settings.


#include "GunSemiAutomatic.h"

void AGunSemiAutomatic::StartShooting()
{
    AGunBase::StartShooting();

    Shoot();
}
