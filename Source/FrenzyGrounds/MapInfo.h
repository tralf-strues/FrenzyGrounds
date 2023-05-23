// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedStruct.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "MapInfo.generated.h"

USTRUCT(BlueprintType)
struct FMapInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Level;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* PreviewImage;
};
