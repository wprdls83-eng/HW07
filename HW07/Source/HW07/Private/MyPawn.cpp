#include "MyPawn.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"

AMyPawn::AMyPawn()
{
	PrimaryActorTick.bCanEverTick = false;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapslueComp"));
	RootComponent = CapsuleComp;
	CapsuleComp->SetSimulatePhysics(false);

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Sans"));
	MeshComp->SetupAttachment(RootComponent);
	CapsuleComp->SetSimulatePhysics(false);

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.0f;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	MoveSpeed = 600.0f;
	RotationSpeed = 100.0f;
}

void AMyPawn::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerContrioller = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LocalPlayer = PlayerContrioller->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* SubSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				SubSystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}

void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyPawn::Move);

		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyPawn::Look);
	}
}

void AMyPawn::Move(const FInputActionValue& value)
{
	FVector2D MoveValue = value.Get<FVector2D>();

	if (Controller)
	{
		FVector Forward = GetActorForwardVector();
		FVector Right = GetActorRightVector();

		Forward.Z = 0.0f;
		Right.Z = 0.0f;

		FVector MoveDirection = Forward * MoveValue.Y + Right * MoveValue.X;

		AddActorLocalOffset(MoveDirection * MoveSpeed * GetWorld()->GetDeltaSeconds(), true);
	}

}
void AMyPawn::Look(const FInputActionValue& value)
{
	FVector2D LookValue = value.Get<FVector2D>();

	float DeltaTime = GetWorld()->GetDeltaSeconds();

	float YawAmount = LookValue.X * RotationSpeed * DeltaTime;
	float PitchAmount = LookValue.Y * RotationSpeed * DeltaTime;

	AddActorLocalRotation(FRotator(0.0f, YawAmount, 0.0f));

	SpringArmComp->AddLocalRotation(FRotator(PitchAmount, 0.0f, 0.0f));

}

