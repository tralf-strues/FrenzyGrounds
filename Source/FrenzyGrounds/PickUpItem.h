// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickUpItem.generated.h"

UCLASS()
class FRENZYGROUNDS_API APickUpItem : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Trigger")
	class UBoxComponent* CollisionBox;

	UPROPERTY(EditDefaultsOnly, Category = "Visuals")
	class UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditDefaultsOnly, ReplicatedUsing=OnRepActive, Category = "State")
	bool Active{ true };

	UPROPERTY(EditDefaultsOnly, Category = "State")
	float RespawnTime{ 5.0f };

private:
	FTimerHandle RespawnTimerHandle;

public:
	APickUpItem();

	void Apply(class AFrenzyGroundsCharacter* character);

protected:
	virtual void ApplyCustom(class AFrenzyGroundsCharacter* character) {}

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRepActive();

private:
	void Respawn();
};
