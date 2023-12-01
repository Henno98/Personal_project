// Fill out your copyright notice in the Description page of Project Settings.


#include "Psychic_Blast.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraSystem.h"
#include "PsychicCH.h"

// Sets default values
APsychic_Blast::APsychic_Blast()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	SetRootComponent(Collider);
	

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(GetRootComponent());
	StaticMesh->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
	

	MovementSpeed = 1000.f;
	TimeLived = 0.f;
	LifeSpan = 0.5f;
}

// Called when the game starts or when spawned
void APsychic_Blast::BeginPlay()
{
	Super::BeginPlay();
	StaticMesh->OnComponentBeginOverlap.AddDynamic(this, &APsychic_Blast::OnOverlap);
}

// Called every frame
void APsychic_Blast::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector NewLocation = GetActorLocation();
	
	SetActorLocation(NewLocation);

	TimeLived += DeltaTime;
	if (TimeLived > LifeSpan)
	{
		DestroyBullet();
	}
}

void APsychic_Blast::DestroyBullet()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	this->Destroy();
}

void APsychic_Blast::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<APsychicCH>())
	{ 
		Cast<APsychicCH>(OtherActor)->HitBy_PB();
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Cyan, TEXT("Hits"));
		//Destroys bullet
		DestroyBullet();
	}
}

