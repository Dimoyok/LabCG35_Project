// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Math/UnrealMathUtility.h"
#include "Components/LineBatchComponent.h"
#include "Traveler.generated.h"

UCLASS()
class LAB35_API ATraveler : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATraveler();

	TArray<FVector> Route;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* AMesh;
	UPROPERTY(EditAnywhere)
		UMaterial* DoomMaterial;
	UPROPERTY(EditAnywhere)
		UMaterial* SwiborgMaterial;

	UPROPERTY(EditAnywhere)
		float Speed;

	int StartNode;
	int EndNode;
	int CurrentNode;
	FColor RouteColor;
	class AGridManager* GMParent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void ExitGame();

};
