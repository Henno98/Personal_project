// Fill out your copyright notice in the Description page of Project Settings.


#include "TPM_Fighter_GM.h"
#include "PsychicCH.h"
#include "Kismet/GameplayStatics.h"



ATPM_Fighter_GM::ATPM_Fighter_GM()
{
	PrimaryActorTick.bCanEverTick = true;
	static ConstructorHelpers::FClassFinder<ACharacter> PsychichCH(TEXT("/GameSetup/BP_BluePrints/BP_PsychicCH.uasset"));
	if (PsychichCH.Class != NULL)
	{
		DefaultPawnClass = PsychichCH.Class;
	}
}

void ATPM_Fighter_GM::BeginPlay()
{
	Super::BeginPlay();
	/*for(int i = 0;i<3;i++)
	UGameplayStatics::CreatePlayer(GetWorld(), i, true);*/

}


void ATPM_Fighter_GM::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


