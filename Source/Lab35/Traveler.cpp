
#include "Traveler.h"
#include "GridManager.h"

ATraveler::ATraveler()
{
	PrimaryActorTick.bCanEverTick = true;

	AMesh = CreateDefaultSubobject<UStaticMeshComponent>("AMesh");

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Engine/BasicShapes/Sphere"));
	if (SphereMeshAsset.Succeeded()) {
		AMesh->SetStaticMesh(SphereMeshAsset.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> AssetSwiborgMaterial(TEXT("/Game/SwiborgMaterial"));
	if (AssetSwiborgMaterial.Succeeded()) {
		SwiborgMaterial = AssetSwiborgMaterial.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> AssetDoomMaterial(TEXT("/Game/DoomMaterial"));
	if (AssetDoomMaterial.Succeeded()) {
		DoomMaterial = AssetDoomMaterial.Object;
	}

	switch (FMath::RandRange(0, 1)) {
	case 0:
		AMesh->SetMaterial(0, DoomMaterial);
		break;
	case 1:
		AMesh->SetMaterial(0, SwiborgMaterial);
		break;
	}

	Speed = 150.0f;
	CurrentNode = 0;
	StartNode = 0;
	EndNode = 0;
}

void ATraveler::BeginPlay()
{
	Super::BeginPlay();

}

void ATraveler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentNode != Route.Num()) {
		FVector dir = Route[CurrentNode] - this->GetActorLocation();
		if (dir.Size() > Speed * DeltaTime) {
			this->SetActorRotation(dir.Rotation() + FRotator(0, -90, 0));
			this->SetActorLocation(this->GetActorLocation() + dir.GetSafeNormal() * Speed * DeltaTime);
		}
		else {
			this->SetActorLocation(Route[CurrentNode]);
			CurrentNode += 1;
		}
	}
	else {
		GMParent->GetRoute(this, Route);
		CurrentNode = 0;
	}

	for (int i = 0; i < Route.Num() - 1; i++) {
		FVector dir = Route[i] - Route[i + 1];
		FRotator rot1(45, 0, 0);
		FRotator rot2(-45, 0, 0);
		FVector ar1 = rot1.RotateVector(dir.GetSafeNormal()) * 50.0f;
		FVector ar2 = rot2.RotateVector(dir.GetSafeNormal()) * 50.0f;
		GetWorld()->LineBatcher->DrawLine(Route[i], Route[i + 1], RouteColor, 0, 15.0f);
		GetWorld()->LineBatcher->DrawLine(Route[i + 1] + ar1, Route[i + 1], RouteColor, 0, 15.0f);
		GetWorld()->LineBatcher->DrawLine(Route[i + 1] + ar2, Route[i + 1], RouteColor, 0, 15.0f);
	}
}

