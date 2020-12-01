#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/LineBatchComponent.h"
#include "Math/UnrealMathUtility.h"
#include "XmlFile.h"
//#include "tinyxml2.h"
#include <stdlib.h>
#include "GridManager.generated.h"

UCLASS()
class LAB35_API AGridManager : public AActor
{
	GENERATED_BODY()

public:

	AGridManager();
	TArray<FVector> GNodes;
	TArray<TArray<bool>> GMEdges;
	TArray<TArray<FColor>> GMEdgeColors;

	int GMedgesCount;

	UPROPERTY(EditAnywhere)
		int GridW;
	UPROPERTY(EditAnywhere)
		int GridH;
	UPROPERTY(EditAnywhere)
		float NodeRX;
	UPROPERTY(EditAnywhere)
		float NodeRY;
	UPROPERTY(EditAnywhere)
		float NodeRZ;
	UPROPERTY(EditAnywhere)
		int AddEdges;
	UPROPERTY(EditAnywhere)
		int TravelersCount;
	UPROPERTY(EditAnywhere)
		float TravelersSpeed;

protected:
	virtual void BeginPlay() override;


public:
	virtual void Tick(float DeltaTime) override;

	void GetRoute(class ATraveler* trav, TArray<FVector>& Route);

private:
	void CreateGrid();
	int GetNeighbours(int CurrentNode, TArray<bool>& NodesMarks);
	int GetNeighbours(int CurrentNode);
	void Dijkstra(TArray<FVector>& Route, int StartNode, int EndNode, FColor EdgeColor);
	FColor GetContrastColor(int h);
	void CreateTravelers();
	void SetSettings();
};
