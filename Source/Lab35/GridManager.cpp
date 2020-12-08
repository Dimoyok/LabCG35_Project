
#include "GridManager.h"
#include "Traveler.h"

AGridManager::AGridManager()
{
	PrimaryActorTick.bCanEverTick = true;

	NodeRX = 128.0;
	NodeRY = 128.0;
	NodeRZ = 128.0;
	GridW = 5;
	GridH = 5;
	TravelersCount = 5;
	AddEdges = 15;
}

void AGridManager::SetSettings()
{
	FString dir = FPaths::ConvertRelativePathToFull("../../../");
	FXmlFile* file = new FXmlFile();
	file->LoadFile(dir + "settings.xml");
	if (file != nullptr)
	{
		FXmlNode* RootNode = file->GetRootNode();
		if (RootNode != nullptr)
		{
			FXmlNode* graphPointsCountX = RootNode->FindChildNode("graphPointsCountX");
			FXmlNode* graphPointsCountY = RootNode->FindChildNode("graphPointsCountY");
			FXmlNode* objectsCount = RootNode->FindChildNode("objectsCount");
			FXmlNode* offsetX = RootNode->FindChildNode("offsetX");
			FXmlNode* offsetY = RootNode->FindChildNode("offsetY");
			FXmlNode* offsetZ = RootNode->FindChildNode("offsetZ");
			FXmlNode* objectVelocity = RootNode->FindChildNode("objectVelocity");

			if (graphPointsCountX != nullptr)
				GridW = FCString::Atoi(*graphPointsCountX->GetContent());

			if (graphPointsCountY != nullptr)
				GridH = FCString::Atoi(*graphPointsCountY->GetContent());

			if (objectsCount != nullptr)
				TravelersCount = FCString::Atoi(*objectsCount->GetContent());

			if (objectVelocity != nullptr)
				TravelersSpeed = FCString::Atof(*objectVelocity->GetContent());

			if (offsetX != nullptr)
				NodeRX = FCString::Atof(*offsetX->GetContent());
			if (offsetY != nullptr)
				NodeRY = FCString::Atof(*offsetY->GetContent());
			if (offsetZ != nullptr)
				NodeRZ = FCString::Atof(*offsetZ->GetContent());

			int MaxTravelers = GridW * GridH;
			if (TravelersCount > MaxTravelers)
			{
				TravelersCount = MaxTravelers;
			}
		}
	}
}

FColor AGridManager::GetContrastColor(int h) {
	int s = 100, v = 100;

	int Hi = (h / 60) % 6;
	int vmin = (100 - s) * v / 100;
	int a = (v - vmin) * (h % 60) / 60;
	int vinc = vmin + a;
	int vdec = v - a;

	if (Hi == 0) return FColor(v, vinc, vmin);
	if (Hi == 1) return FColor(vdec, v, vmin);
	if (Hi == 2) return FColor(vmin, v, vinc);
	if (Hi == 3) return FColor(vmin, vdec, v);
	if (Hi == 4) return FColor(vinc, vmin, v);
	if (Hi == 5) return FColor(v, vmin, vdec);

	return FColor(0, 0, 0);
}

int AGridManager::GetNeighbours(int CurrentNode, TArray<bool>& NodesMarks)
{
	TArray<int> Neighbours;
	int NeighbourNode;
	for (int y = -1; y <= 1; y++)
		for (int x = -1; x <= 1; x++) {

			int cx = CurrentNode % GridW, cy = CurrentNode / GridW;
			if (cx + x < 0 || cy + y < 0 || cx + x >= GridW || cy + y >= GridH) continue; // Out of bounds

			NeighbourNode = (cx + x) + (cy + y) * GridW;
			if (GMEdges[CurrentNode][NeighbourNode] || GMEdges[NeighbourNode][CurrentNode]) continue;

			if (x != 0 && y != 0) {

				if (GMEdges[cx + (cy + y) * GridW][(cx + x) + cy * GridW] ||
					GMEdges[(cx + x) + cy * GridW][cx + (cy + y) * GridW]) continue; // Intersect existing edge
			}

			if (NodesMarks[NeighbourNode]) continue; // Marked node

			Neighbours.Add(NeighbourNode);
		}

	if (Neighbours.Num() == 0) return -1;

	return Neighbours[FMath::RandRange(0, Neighbours.Num() - 1)];
}

int AGridManager::GetNeighbours(int CurrentNode) {
	TArray<bool> NodesMarks;
	NodesMarks.Init(false, GNodes.Num());
	NodesMarks[CurrentNode] = true;

	return this->GetNeighbours(CurrentNode, NodesMarks);
}

void AGridManager::GetRoute(class ATraveler* trav, TArray<FVector>& Route) {
	trav->Route.Empty();
	int StartNode = trav->EndNode;
	int EndNode = StartNode;
	for (int j = 0; j < 100; j++)
		if ((EndNode = FMath::RandRange(0, GNodes.Num() - 1)) != StartNode) break;
	trav->StartNode = StartNode;
	trav->EndNode = EndNode;
	this->Dijkstra(trav->Route, StartNode, EndNode, trav->RouteColor);
}

void AGridManager::CreateTravelers() {
	TArray<int> NodesEmpty;
	for (int i = 0; i < GNodes.Num(); i++)
		NodesEmpty.Add(i);

	for (int i = 0; i < TravelersCount; i++) {
		int node = FMath::RandRange(0, NodesEmpty.Num() - 1);
		int StartNode = NodesEmpty[node];
		int EndNode = StartNode;
		ATraveler* trav = GetWorld()->SpawnActor<ATraveler>(GNodes[StartNode], FRotator(0, 0, 0));
		trav->GMParent = this;
		trav->StartNode = StartNode;
		trav->EndNode = EndNode;
		trav->RouteColor = this->GetContrastColor(i * 360 / TravelersCount);
		trav->deltaZ = NodeRZ / 100.0f * (i + 1);
		trav->Speed = TravelersSpeed;

		NodesEmpty.RemoveAt(node);
	}
}

void AGridManager::CreateGrid()
{
	for (int y = 0; y < GridH; y++) {
		for (int x = 0; x < GridW; x++) {
			FVector NodeV = FVector(x * NodeRX * 4, y * NodeRY * 4, 0);
			NodeV.X += FMath::RandRange(-NodeRX, NodeRX); // Fluctuation
			NodeV.Y += FMath::RandRange(-NodeRY, NodeRY); // Fluctuation
			NodeV.Z += FMath::RandRange(-NodeRZ, NodeRZ); // Fluctuation
			GNodes.Add(NodeV);
		}
	}

	GMedgesCount = 0;
	GMEdges.Init({}, GridW * GridH);
	for (int x = 0; x < GridW * GridH; ++x)
		GMEdges[x].Init(false, GridW * GridH);

	GMEdgeColors.Init({}, GridW * GridH);
	for (int x = 0; x < GridW * GridH; ++x)
		GMEdgeColors[x].Init(FColor(255, 255, 255), GridW * GridH);

	TArray<int> NodesStack;
	TArray<bool> NodesMarks;

	for (int i = 0; i < GNodes.Num(); i++) {
		NodesMarks.Add(false);
	}

	int CurrentNode = FMath::RandRange(0, GNodes.Num() - 1);
	int NeighbourNode;

	NodesStack.Push(CurrentNode);
	NodesMarks[CurrentNode] = true;

	while (NodesStack.Num() > 0) {
		CurrentNode = NodesStack.Pop();

		NeighbourNode = this->GetNeighbours(CurrentNode, NodesMarks);
		if (NeighbourNode == -1) continue;

		NodesMarks[NeighbourNode] = true;

		GMEdges[CurrentNode][NeighbourNode] = true;
		GMEdges[NeighbourNode][CurrentNode] = true;
		NodesStack.Push(CurrentNode);
		NodesStack.Push(NeighbourNode);
		++GMedgesCount;
	}


	for (int i = 0; i < 20; i++) {
		CurrentNode = FMath::RandRange(0, GNodes.Num() - 1);


		NeighbourNode = this->GetNeighbours(CurrentNode);
		if (NeighbourNode == -1) continue;

		GMEdges[CurrentNode][NeighbourNode] = true;
		GMEdges[NeighbourNode][CurrentNode] = true;
		++GMedgesCount;

	}
}


void AGridManager::Dijkstra(TArray<FVector>& Route, int StartNode, int EndNode, FColor EdgeColor) {
	TArray<int> MinRoutes;
	TArray<int> Parents;
	TArray<int> NodesStack;
	TArray<bool> NodesMarks;
	int CurrentNode;

	MinRoutes.Init(GMedgesCount * 2, GMedgesCount);
	Parents.Init(-1, GMedgesCount);
	NodesMarks.Init(false, GMedgesCount);

	NodesStack.Add(StartNode);
	MinRoutes[StartNode] = 0;

	while (NodesStack.Num() > 0) {
		CurrentNode = NodesStack.Pop();
		NodesMarks[CurrentNode] = true;
		for (int x = 0; x < GridW * GridH; x++) {
			if (!GMEdges[x][CurrentNode]) continue;

			if (MinRoutes[x] > MinRoutes[CurrentNode] + 1) {
				MinRoutes[x] = MinRoutes[CurrentNode] + 1;
				Parents[x] = CurrentNode;
			}

			if (!NodesMarks[x]) {
				NodesStack.Add(x);
			}
		}
	}

	CurrentNode = EndNode;
	Route.Empty();
	while (CurrentNode != StartNode) {
		Route.Insert(GNodes[CurrentNode], 0);
		CurrentNode = Parents[CurrentNode];
	}
	Route.Insert(GNodes[CurrentNode], 0);
}

void AGridManager::BeginPlay()
{
	Super::BeginPlay();
	this->SetSettings();
	this->CreateGrid();
	this->CreateTravelers();
}



void AGridManager::Tick(float DeltaTime)

{
	Super::Tick(DeltaTime);

	for (int i = 0; i < GNodes.Num(); i++) {
		GetWorld()->LineBatcher->DrawPoint(GNodes[i], FColor(0, 0, 255), 15.0f, 0);
	}

	for (int x = 0; x < GridW * GridH; x++) {
		for (int y = 0; y < x; y++) {
			if (GMEdges[x][y]) GetWorld()->LineBatcher->DrawLine(GNodes[x], GNodes[y], GMEdgeColors[x][y], 0, 5.0f);
		}
	}


}

