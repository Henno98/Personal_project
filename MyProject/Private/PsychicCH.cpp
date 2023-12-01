// Fill out your copyright notice in the Description page of Project Settings.


#include "PsychicCH.h"

#include <algorithm>

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Psychic_Lance.h"
#include "Psychic_Blast.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"

using namespace std;
// Sets default values
APsychicCH::APsychicCH()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//Springarm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 400.f; // Distance from player
	SpringArm->bUsePawnControlRotation = true; // Rotate arm based on controller
	//Camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = true;

	Timer = 30.f;
	
	
}
// Called when the game starts or when spawned
void APsychicCH::BeginPlay()
{
	Super::BeginPlay();
	
	//Controller Setup
	if (UGameplayStatics::GetPlayerController(this, 0) != nullptr) 
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->AddMappingContext(MappingContext, 0);
			}
		}
	}
	
	EnergyCalc();
	HealthCalc();
	StaminaCalc();
	GetCharacterMovement()->AirControl = 1;
	GetCharacterMovement()->MaxWalkSpeed = 2500;
	GetCharacterMovement()->MaxAcceleration = 5000;
	GetCharacterMovement()->GroundFriction = 0.8;
	
	
}
// Called every frame
void APsychicCH::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentHealth <= 0) {
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
	}
	RegenCalc();

}
// Called to bind functionality to input
void APsychicCH::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	APlayerController* PC = Cast<APlayerController>(GetController());

	if (UEnhancedInputComponent* EnhanceInputCom = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		
		EnhanceInputCom->BindAction(DashInput, ETriggerEvent::Triggered, this, &APsychicCH::Movement);
		EnhanceInputCom->BindAction(MovementInput, ETriggerEvent::Triggered, this, &APsychicCH::Dash);
		EnhanceInputCom->BindAction(JumpInput, ETriggerEvent::Completed, this, &APsychicCH::Up);
		

		
		EnhanceInputCom->BindAction(MouseXInput, ETriggerEvent::Triggered, this, &APsychicCH::Mouse);
		
		

		EnhanceInputCom->BindAction(PsychicBlastInput, ETriggerEvent::Triggered, this, &APsychicCH::PsychicBlast);
		EnhanceInputCom->BindAction(PsychicBlastInput, ETriggerEvent::Completed, this, &APsychicCH::PsychicBlast);
		EnhanceInputCom->BindAction(PsychicLanceInput, ETriggerEvent::Completed, this, &APsychicCH::PsychicSpear);

	}
}

void APsychicCH::Mouse(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APsychicCH::Movement(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
	
}

void APsychicCH::HealthCalc()
{
	
	if(Endurance > 0)
	{
		int EnduranceMultiplier = 15;

		Health = Endurance * EnduranceMultiplier;
		CurrentHealth = Health;
		HealthRegen = Endurance / 100;

	}
	
}

void APsychicCH::StaminaCalc()
{
	if(Endurance > 0)
	{
		int EnduranceMultiplier = 100;

		Stamina = Endurance * EnduranceMultiplier;
		CurrentStamina = Stamina;
		StaminaRegen = Endurance / 10;
	}
}

void APsychicCH::EnergyCalc()
{
	int Mainstat = Mystic;
	if (Mainstat > 0)
	{
		Energy = Mainstat * 10;

		CurrentEnergy = Energy;

		EnergyRegen = (Mainstat / 100);

	}
}

void APsychicCH::RegenCalc()
{
	
	
	Timer -= 1;
	if (Timer == 0.f) 
	{
		if (CurrentEnergy < Energy) {
			CurrentEnergy += EnergyRegen;
			
	
		}
		if (CurrentStamina < Stamina) {
			CurrentStamina += StaminaRegen;
			
		}
		if (CurrentHealth < Health) {
			CurrentHealth += HealthRegen;
			
		}
		Timer = 30.f;
	}
	else
	{
		CurrentEnergy = clamp(CurrentEnergy, 0, Energy);
		CurrentStamina = clamp(CurrentStamina, 0, Stamina);
		CurrentHealth = clamp(CurrentHealth, 0, Health);
	}
}

void APsychicCH::PsychicSpear()
{

	if (CurrentEnergy >= 10) {
		//UE_LOG(LogTemp, Warning, TEXT("Shoot"));
		ActorLocation = GetActorLocation();
		NewSpawnLocation = FVector3d(ActorLocation.X , ActorLocation.Y  , ActorLocation.Z + 50);
		APsychic_Lance* Lance = GetWorld()->SpawnActor<APsychic_Lance>(BP_Psychic_Lance,		// What to spawn
			NewSpawnLocation, GetControlRotation());	// Location & Rotation
		CurrentEnergy -= 10;
		
	}
	
}
void APsychicCH::Dash()
{
	if (CurrentStamina > 1000) {
		FVector Speed = GetMovementComponent()->Velocity;
		LaunchCharacter(Speed * 2, false, false);
		CurrentStamina -= 1000;
	}
}
void APsychicCH::Up(const FInputActionValue& input)
{
	if (!GetCharacterMovement()->IsFalling()) {
		LaunchCharacter(FVector(0, 0, 5000), false, false);
		CurrentStamina -= 1000;
	}
}


void APsychicCH::HitBy_PB()
{
	CurrentHealth -= 10;
}

void APsychicCH::HitBy_PL()
{
	CurrentHealth -= 100;
}

void APsychicCH::PsychicBlast()
{
	if (CurrentEnergy >= 1) {
		//UE_LOG(LogTemp, Warning, TEXT("Shoot"));
		ActorLocation = GetActorLocation();
		NewSpawnLocation = FVector3d(ActorLocation.X, ActorLocation.Y, ActorLocation.Z +50);
		GetWorld()->SpawnActor<AActor>(BP_Psychic_Blast,		// What to spawn
			NewSpawnLocation, GetControlRotation());	// Location & Rotation
		CurrentEnergy -= 1;
	}

}
