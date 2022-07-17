// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PawnMovementComponent.h"


// Sets default values
APlayerPawn::APlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    // Set this pawn to be controlled by the lowest - numbered player
        AutoPossessPlayer = EAutoReceiveInput::Player0;
        bUseControllerRotationPitch = true;
        bUseControllerRotationYaw = true;
        bUseControllerRotationRoll = true;

    // Create a dummy root component we can attach things to.
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    // Create a camera and a visible object
    UCameraComponent* OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("OurCamera"));
    OurVisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OurVisibleComponent"));
    // Attach our camera and visible object to our root component. Offset and rotate the camera.
    OurCamera->SetupAttachment(RootComponent);
    OurCamera->SetRelativeLocation(FVector(-0, 0.0f, 0));
    OurCamera->SetRelativeRotation(FRotator(-0, 0.0f, 0.0f));
    OurVisibleComponent->SetupAttachment(RootComponent);

    MoveRate = 300;
    Sensitivity = 45;
}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
    // Handle movement based on our "MoveX" and "MoveY" axes
    {
        if (!GetPendingMovementInputVector().IsZero())
        {
            FVector NewLocation = GetActorLocation() + (ConsumeMovementInputVector() * DeltaTime);
           // CurrentVelocity = GetPendingMovementInputVector();
            SetActorLocation(NewLocation);
        }
    }
}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
   
    // Respond every frame to the values of our two movement axes, "MoveX" and "MoveY".
    InputComponent->BindAxis("MoveX", this, &APlayerPawn::Move_XAxis);
    InputComponent->BindAxis("MoveY", this, &APlayerPawn::Move_YAxis);

    PlayerInputComponent->BindAxis("Turn", this, &APlayerPawn::AddControllerYawInput);
    PlayerInputComponent->BindAxis("LookUp", this, &APlayerPawn::AddControllerPitchInput);
}

void APlayerPawn::Move_XAxis(float AxisValue)
{
    // Move at 100 units per second forward or backward
    if ((Controller) && (AxisValue != 0.0f))
    {   
        AddMovementInput(GetActorForwardVector(), AxisValue * MoveRate, false);
    }
}

void APlayerPawn::Move_YAxis(float AxisValue)
{
    // Move at 100 units per second right or left
    if ((Controller) && (AxisValue != 0.0f))
    {
        AddMovementInput(GetActorRightVector(), AxisValue * MoveRate, false);
    }
}

void APlayerPawn::TurnAxis(float AxisValue)
{
    AddControllerYawInput(AxisValue * Sensitivity * GetWorld()->GetDeltaSeconds());
}

void APlayerPawn::LookUpAxis(float AxisValue)
{
    AddControllerPitchInput(AxisValue * Sensitivity * GetWorld()->GetDeltaSeconds());
}

