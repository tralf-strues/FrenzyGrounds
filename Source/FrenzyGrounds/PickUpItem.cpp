// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpItem.h"
#include "Net/UnrealNetwork.h"
#include "Components/BoxComponent.h"
#include "Engine/Engine.h"

#include "FrenzyGroundsCharacter.h"

APickUpItem::APickUpItem()
{
	bReplicates = true;

	Tags.Push("PickUpItem");

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;
	CollisionBox->SetIsReplicated(true);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(CollisionBox);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APickUpItem::Apply(AFrenzyGroundsCharacter* character)
{
	UE_LOG(LogTemp, Warning, TEXT("APickUpItem::Apply!, RespawnTime = %f, Active = %f"), RespawnTime, Active);

	if (Active)
	{
		ApplyCustom(character);

		Active = false;
		OnRepActive();

		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &APickUpItem::Respawn, RespawnTime, false, RespawnTime);
	}
}

void APickUpItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APickUpItem, Active);
}

void APickUpItem::OnRepActive()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRepActive! Active = %d"), Active);

	if (ItemMesh)
	{
		ItemMesh->SetHiddenInGame(!Active);
	}
}

void APickUpItem::Respawn()
{
	Active = true;
	OnRepActive();

	GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);
	RespawnTimerHandle.Invalidate();
}

