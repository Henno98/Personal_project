// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TPM_Fighter_GM.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API ATPM_Fighter_GM : public AGameModeBase
{
	GENERATED_BODY()

	ATPM_Fighter_GM();

protected:
	// Called when the game starts or when spawned

	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
