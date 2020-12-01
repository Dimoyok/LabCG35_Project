#include "FlyCam.h"

AFlyCam::AFlyCam()
{
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(RootComponent);

	this->dTime = 0.0f;
}

void AFlyCam::BeginPlay()
{
	Super::BeginPlay();

	this->SetActorLocation(FVector(-3200, 600, 1500));
	this->SetActorRotation(FRotator(-15, 5, 0));
}

void AFlyCam::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	this->dTime = DeltaTime;
}

void AFlyCam::ExitGame()
{
	FGenericPlatformMisc::RequestExit(true);
}


void AFlyCam::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("VerticalAxis", this, &AFlyCam::MoveVer);
	InputComponent->BindAxis("HorizontalAxis", this, &AFlyCam::MoveHor);

	InputComponent->BindAxis("MouseX", this, &AFlyCam::RotYaw);
	InputComponent->BindAxis("MouseY", this, &AFlyCam::RotPitch);

	InputComponent->BindAction("ExitGame", IE_Pressed, this, &AFlyCam::ExitGame);
}


void AFlyCam::MoveVer(float Value)
{
	FQuat rot = this->GetActorRotation().Quaternion();
	FVector dir = rot * FVector(1, 0, 0) * Value * this->MoveSpeed * this->dTime;
	this->SetActorLocation(this->GetActorLocation() + dir);
}

void AFlyCam::MoveHor(float Value)
{
	FQuat rot = this->GetActorRotation().Quaternion();
	FVector dir = rot * FVector(0, 1, 0) * Value * this->MoveSpeed * this->dTime;
	this->SetActorLocation(this->GetActorLocation() + dir);
}

void AFlyCam::RotPitch(float Value)
{
	FRotator NewRotation = GetActorRotation();
	NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch + Value, -80.0f, 80.0f);
	SetActorRotation(NewRotation);
}


void AFlyCam::RotYaw(float Value)
{
	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += Value;
	SetActorRotation(NewRotation);
}