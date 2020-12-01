#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "FlyCam.generated.h"

UCLASS()
class LAB35_API AFlyCam : public APawn
{
	GENERATED_BODY()

public:
	AFlyCam();

	UPROPERTY()
		UCameraComponent* Camera;

	void MoveVer(float Value);
	void MoveHor(float Value);
	void RotPitch(float Value);
	void RotYaw(float Value);

	void ExitGame();


	UPROPERTY(EditAnywhere)
		float MoveSpeed = 1000;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	float dTime;
};
