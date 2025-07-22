// GridPuzzleManagerComponent.cpp – Grid-Logic ↔ Actor 매핑 & Trace-Fire
#include "GridPuzzleManagerComponent.h"
#include "GridCellBaseActor.h"
#include "GridFloorGCActor.h"
#include "GridWallGCActor.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

DEFINE_LOG_CATEGORY(LogAS_GridPuzzle);

//─────────────────────────────────────────────
// 방향 상수
static const FIntPoint DirRight(1,0), DirUp(0,1);
static const FIntPoint AroundOffsets[5]={{0,0},{1,0},{-1,0},{0,1},{0,-1}};

static bool IsPointLess(const FIntPoint& A,const FIntPoint& B)
{ return (A.X<B.X) || (A.X==B.X && A.Y<B.Y); }

static FGridEdge MakeEdgeKey(const FIntPoint& P1,const FIntPoint& P2,int32 Default=3)
{ return IsPointLess(P1,P2) ? FGridEdge{P1,P2,Default} : FGridEdge{P2,P1,Default}; }

//─────────────────────────────────────────────
UGridPuzzleManagerComponent::UGridPuzzleManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UGridPuzzleManagerComponent::BeginPlay()
{
    Super::BeginPlay();

    GridOrigin = FVector2D(GetOwner()->GetActorLocation());

    InitializeGrid();
    RebuildMapping();
    SyncHealthFromPlacedActors();
}


//─────────────────────────────────────────────
// 그리드 초기화(로직 전용)
void UGridPuzzleManagerComponent::InitializeGrid()
{
    GridMap.Empty(Rows*Cols);
    EdgeSet.Empty();

    for (int32 Y=0; Y<Rows; ++Y)
        for (int32 X=0; X<Cols; ++X)
        {
            const FIntPoint C(X,Y);
            GridMap.Add(C,{EGridCellType::Floor,3,false,GridToWorld(C)});

            if (X<Cols-1) EdgeSet.Add(MakeEdgeKey(C,C+DirRight));
            if (Y<Rows-1) EdgeSet.Add(MakeEdgeKey(C,C+DirUp));
        }
}

//─────────────────────────────────────────────
// 좌표 변환(원점 보정 포함)
FIntPoint UGridPuzzleManagerComponent::WorldToGrid(const FVector& P) const
{
    const FVector Local = P - FVector(GridOrigin, 0.f);
    const int32 X = FMath::FloorToInt(Local.X / CellSize + 0.5f);
    const int32 Y = FMath::FloorToInt(Local.Y / CellSize + 0.5f);
    return FIntPoint(X, Y);
}

FVector UGridPuzzleManagerComponent::GridToWorld(const FIntPoint& C) const
{
    return FVector(GridOrigin, 0.f) + FVector(C.X * CellSize, C.Y * CellSize, 0.f);
}

//─────────────────────────────────────────────
// Health 동기화
void UGridPuzzleManagerComponent::SyncHealthFromPlacedActors()
{
    UWorld* W = GetWorld(); if (!W) return;

    TArray<AActor*> Walls;
    UGameplayStatics::GetAllActorsOfClass(W, AGridWallGCActor::StaticClass(), Walls);
    for (auto* A : Walls)
    {
        auto* Wall = Cast<AGridWallGCActor>(A);
        const FVector Mid = Wall->GetActorLocation();
        const bool Horz = FMath::Abs(Wall->GetActorForwardVector().X) > FMath::Abs(Wall->GetActorForwardVector().Y);
        
        FVector Offset = Horz ? FVector(CellSize * 0.5f, 0, 0) : FVector(0, CellSize * 0.5f, 0);
        const FIntPoint GridA = WorldToGrid(Mid - Offset);
        const FIntPoint GridB = WorldToGrid(Mid + Offset);

        const FGridEdge Key = MakeEdgeKey(GridA, GridB);
        if (FGridEdge* E = EdgeSet.Find(Key))
        {
            E->Health = FMath::Clamp(Wall->Health, 0, 3);
            Wall->RefreshVisual();
        }
    }
}



//─────────────────────────────────────────────
// 스폰(디자이너용) – Editor 빌드에서만 활성
#if WITH_EDITOR
void UGridPuzzleManagerComponent::SpawnFloors()
{
    if (!FloorClass) return;
    UWorld* W = GetWorld(); if (!W) return;

    for (const auto& Elem : GridMap)
    {
        auto* F = W->SpawnActor<AGridFloorGCActor>(FloorClass,Elem.Value.WorldLocation,FRotator::ZeroRotator);
        F->Health = Elem.Value.Health;
        F->RefreshVisual();
        FloorActors.Add(Elem.Key,F);
        DrawDebugBox(W,Elem.Value.WorldLocation,FVector(CellSize*0.5f),FColor::Black,false,10.f);
    }
}
void UGridPuzzleManagerComponent::SpawnEdges()
{
    if (!WallClass) return;
    UWorld* W = GetWorld(); if (!W) return;

    for (const FGridEdge& E : EdgeSet)
    {
        const FVector Mid=(GridToWorld(E.A)+GridToWorld(E.B))*0.5f;
        const float Yaw =(E.A.X!=E.B.X)?0.f:90.f;

        auto* WActor=W->SpawnActor<AGridWallGCActor>(WallClass,Mid,FRotator(0,Yaw,0));
        WActor->Health=E.Health;
        WActor->RefreshVisual();
        WallActors.Add(E,WActor);
        WallToKey.Add(WActor,E);

        DrawDebugLine(W,GridToWorld(E.A),GridToWorld(E.B),FColor::Red,false,10.f,0,10.f);
    }
}
#endif // WITH_EDITOR

//─────────────────────────────────────────────
// RebuildMapping – 월드 액터 → 논리 매핑
void UGridPuzzleManagerComponent::RebuildMapping()
{
    FloorActors.Empty(); WallActors.Empty(); WallToKey.Empty();

    UWorld* W = GetWorld(); if (!W) return;

    // Floor 처리 (중복 검사 추가)
    TArray<AActor*> Floors;
    UGameplayStatics::GetAllActorsOfClass(W, AGridFloorGCActor::StaticClass(), Floors);
    for (auto* A : Floors)
    {
        auto* Floor = Cast<AGridFloorGCActor>(A);
        const FIntPoint GridPos = WorldToGrid(Floor->GetActorLocation());

        if (FloorActors.Contains(GridPos))
        {
            UE_LOG(LogAS_GridPuzzle, Warning, TEXT("Duplicate Floor at %s"), *GridPos.ToString());
            Floor->Destroy(); // 중복된 Floor 제거
            continue;
        }

        FloorActors.Add(GridPos, Floor);
        if (FGridCellData* Cell = GridMap.Find(GridPos))
            Cell->WorldLocation = Floor->GetActorLocation();
    }

    // Wall 처리 (기존 유지, 정확한 Offset 적용)
    TArray<AActor*> Walls;
    UGameplayStatics::GetAllActorsOfClass(W, AGridWallGCActor::StaticClass(), Walls);
    for (auto* A : Walls)
    {
        auto* Wall = Cast<AGridWallGCActor>(A);
        const FVector Mid = Wall->GetActorLocation();
        const bool Horz = FMath::Abs(Wall->GetActorForwardVector().X) > FMath::Abs(Wall->GetActorForwardVector().Y);
        
        const FVector Offset = Horz ? FVector(CellSize * 0.5f, 0, 0) : FVector(0, CellSize * 0.5f, 0);
        const FIntPoint GridA = WorldToGrid(Mid - Offset);
        const FIntPoint GridB = WorldToGrid(Mid + Offset);

        const FGridEdge Key = MakeEdgeKey(GridA, GridB);
        if (WallActors.Contains(Key))
        {
            UE_LOG(LogAS_GridPuzzle, Warning, TEXT("Duplicate Wall at Edge: %s-%s"), *GridA.ToString(), *GridB.ToString());
            Wall->Destroy(); // 중복 벽 제거
            continue;
        }

        WallActors.Add(Key, Wall);
        WallToKey.Add(Wall, Key);
    }

    UE_LOG(LogAS_GridPuzzle, Log, TEXT("[Rebuild] Floors=%d Walls=%d"), FloorActors.Num(), WallActors.Num());
}



//─────────────────────────────────────────────
// SpawnMissingActors – 빈 칸 자동 배치
void UGridPuzzleManagerComponent::SpawnMissingActors()
{
    UWorld* W=GetWorld(); if(!W||!FloorClass||!WallClass) return;

    /* Floor */
    for (const auto& Pair:GridMap)
        if (!FloorActors.Contains(Pair.Key))
        {
            auto* F=W->SpawnActor<AGridFloorGCActor>(FloorClass,Pair.Value.WorldLocation,FRotator::ZeroRotator);
            F->Health = Pair.Value.Health;
            F->RefreshVisual();
            FloorActors.Add(Pair.Key,F);
            UE_LOG(LogAS_GridPuzzle, Log, TEXT("[SpawnMissingActors] Spawn Floor at %s"), *Pair.Key.ToString());
        }

    /* Wall */
    for (const FGridEdge& E:EdgeSet)
        if (!WallActors.Contains(E))
        {
            const FVector Mid=(GridToWorld(E.A)+GridToWorld(E.B))*0.5f;
            const float Yaw =(E.A.X!=E.B.X)?0.f:90.f;

            auto* WActor=W->SpawnActor<AGridWallGCActor>(WallClass,Mid,FRotator(0,Yaw,0));
            WActor->Health=E.Health;
            WActor->RefreshVisual();
            WallActors.Add(E,WActor);
            WallToKey .Add(WActor,E);
        }
}

//─────────────────────────────────────────────
// Spell: TraceFire
void UGridPuzzleManagerComponent::ApplyTraceFire(const FHitResult& Hit)
{
    UE_LOG(LogAS_GridPuzzle, Warning, TEXT("TraceFire: HitActor = %s"), *Hit.GetActor()->GetName());

    AGridWallGCActor* Wall = Cast<AGridWallGCActor>(Hit.GetActor());
    if (!Wall)
    {
        UE_LOG(LogAS_GridPuzzle, Warning, TEXT("TraceFire: HitActor is NOT a GridWall"));
        return;
    }

    const FGridEdge* KeyPtr = WallToKey.Find(Wall);
    if (!WallToKey.Find(Wall))
    {
        UE_LOG(LogAS_GridPuzzle, Warning,
           TEXT("TraceFire: WallToKey not found for %s, WorldPos=%s"),
           *Wall->GetName(), *Wall->GetActorLocation().ToString());
    }
    if (!KeyPtr)
    {
        UE_LOG(LogAS_GridPuzzle, Warning, TEXT("TraceFire: WallToKey not found for %s"), *Wall->GetName());
        return;
    }

    const FGridEdge Key = *KeyPtr;
    if (FGridEdge* Edge = EdgeSet.Find(Key))
    {
        Edge->Health = FMath::Max(0, Edge->Health - 1);
        RefreshWallVisual(Key);
        UE_LOG(LogAS_GridPuzzle, Warning,
               TEXT("TraceFire Wall %s-%s → %d"),
               *Key.A.ToString(), *Key.B.ToString(), Edge->Health);
    }

    const bool bHorz = (Key.A.Y == Key.B.Y);
    const FIntPoint Dir = bHorz ? FIntPoint(0, 1) : FIntPoint(1, 0);
    const FIntPoint Center = (Key.A + Key.B) / 2;

    for (const FIntPoint& C : { Center + Dir, Center - Dir })
    {
        if (FGridCellData* Cell = GridMap.Find(C))
        {
            const int32 Old = Cell->Health;
            Cell->Health = FMath::Max(0, Old - 1);
            RefreshFloorVisual(C);
            UE_LOG(LogAS_GridPuzzle, Warning,
                   TEXT("TraceFire Floor %s %d→%d"),
                   *C.ToString(), Old, Cell->Health);
        }
    }
}

//─────────────────────────────────────────────
// Visual Refresh Helpers
//─────────────────────────────────────────────

void UGridPuzzleManagerComponent::RefreshFloorVisual(const FIntPoint& Coord)
{
    AGridFloorGCActor** FloorPtr = FloorActors.Find(Coord);
    FGridCellData* Cell = GridMap.Find(Coord);
    if (!FloorPtr || !Cell) return;

    AGridFloorGCActor* Floor = *FloorPtr;

    if (Cell->Health <= 0)
    {
        Floor->Destroy();
        FloorActors.Remove(Coord);
        UE_LOG(LogAS_GridPuzzle, Warning, TEXT("Floor destroyed at %s"), *Coord.ToString());
        return;
    }

    Floor->Health = Cell->Health;
    Floor->RefreshVisual();

#if !(UE_BUILD_SHIPPING)
    DrawDebugString(
        GetWorld(),
        Floor->GetActorLocation() + FVector(0, 0, 90),
        FString::Printf(TEXT("Floor %s : %d"), *Coord.ToString(), Cell->Health),
        nullptr, FColor::White, 1.f, false
    );
#endif
}


void UGridPuzzleManagerComponent::RefreshWallVisual(const FGridEdge& E)
{
    AGridWallGCActor** WallPtr = WallActors.Find(E);
    FGridEdge* EdgeData = EdgeSet.Find(E);
    if (!WallPtr || !EdgeData) return;

    AGridWallGCActor* Wall = *WallPtr;

    if (EdgeData->Health <= 0)
    {
        Wall->Destroy();
        WallActors.Remove(E);
        WallToKey.Remove(Wall);
        UE_LOG(LogAS_GridPuzzle, Warning, TEXT("Wall destroyed at Edge %s-%s"), *E.A.ToString(), *E.B.ToString());
        return;
    }

    Wall->Health = EdgeData->Health;
    Wall->RefreshVisual();

#if !(UE_BUILD_SHIPPING)
    DrawDebugString(
        GetWorld(),
        Wall->GetActorLocation() + FVector(0, 0, 120),
        FString::Printf(TEXT("Wall %s-%s : %d"), *E.A.ToString(), *E.B.ToString(), EdgeData->Health),
        nullptr, FColor::Yellow, 1.f, false
    );
#endif
}

//─────────────────────────────────────────────
// GridPuzzleManagerComponent.cpp 추가 정의
//─────────────────────────────────────────────

#include "GridPuzzleManagerComponent.h"

// GridLogic 방식 Fire (캐릭터 CastFire → UseFireSpell 호출용)
void UGridPuzzleManagerComponent::UseFireSpell(
    const FIntPoint& PlayerCell,
    const FIntPoint& Dir)
{
    // 1) 바닥 데미지
    if (FGridCellData* Self = GridMap.Find(PlayerCell))
    {
        const int32 Old = Self->Health;
        Self->Health = FMath::Max(0, Old - 1);
        RefreshFloorVisual(PlayerCell);
        UE_LOG(LogAS_GridPuzzle, Warning,
               TEXT("Fire Floor %s %d→%d"),
               *PlayerCell.ToString(), Old, Self->Health);
    }

    // 2) 벽 데미지
    const FGridEdge Key = MakeEdgeKey(PlayerCell, PlayerCell + Dir);
    if (FGridEdge* Edge = EdgeSet.Find(Key))
    {
        const int32 Old = Edge->Health;
        Edge->Health = FMath::Max(0, Old - 1);
        RefreshWallVisual(Key);
        UE_LOG(LogAS_GridPuzzle, Warning,
               TEXT("Fire Wall %s-%s %d→%d"),
               *Key.A.ToString(), *Key.B.ToString(),
               Old, Edge->Health);
    }
}

// GridLogic 방식 Ice (캐릭터 CastIce → UseIceSpell 호출용)
void UGridPuzzleManagerComponent::UseIceSpell(const FIntPoint& TargetCell)
{
    if (FGridCellData* Cell = GridMap.Find(TargetCell))
    {
        if (Cell->CellType == EGridCellType::Floor && Cell->Health < 3)
        {
            const int32 Old = Cell->Health;
            ++Cell->Health;
            RefreshFloorVisual(TargetCell);
            UE_LOG(LogAS_GridPuzzle, Warning,
                   TEXT("Ice Floor %s %d→%d"),
                   *TargetCell.ToString(), Old, Cell->Health);

#if !(UE_BUILD_SHIPPING)
            DrawDebugBox(GetWorld(), GridToWorld(TargetCell),
                FVector(CellSize * 0.4f), FColor::Cyan, false, 1.f, 0, 2.f);
#endif
        }
    }
}


// 캐릭터 CastIce() 내부에서 사용하는 좌표 검사
bool UGridPuzzleManagerComponent::IsInBounds(const FIntPoint& Coord) const
{
    return Coord.X >= 0 && Coord.X < Cols
        && Coord.Y >= 0 && Coord.Y < Rows;
}

bool UGridPuzzleManagerComponent::IsFloor(const FIntPoint& Coord) const
{
    if (const FGridCellData* Cell = GridMap.Find(Coord))
        return Cell->CellType == EGridCellType::Floor;
    return false;
}

// 디버그용 함수 추가
void UGridPuzzleManagerComponent::PrintGridToWorldCheck()
{
    for (const auto& Elem : GridMap)
    {
        const FIntPoint Coord = Elem.Key;
        const FVector P = GridToWorld(Coord);
        const FIntPoint BackCoord = WorldToGrid(P);

        const bool bMatch = Coord == BackCoord;

        UE_LOG(LogAS_GridPuzzle, Warning,
            TEXT("[GridCheck] Grid=(%d,%d) → World=%s → Grid=%s %s"),
            Coord.X, Coord.Y, *P.ToString(), *BackCoord.ToString(),
            bMatch ? TEXT("✓") : TEXT("✗"));
    }
}

void UGridPuzzleManagerComponent::PrintWallKeyCheck()
{
    for (const auto& Pair : WallActors)
    {
        const FGridEdge& Key = Pair.Key;
        const FVector A = GridToWorld(Key.A);
        const FVector B = GridToWorld(Key.B);
        const FVector Mid = (A + B) * 0.5f;

        const FIntPoint ACheck = WorldToGrid(Mid - (B - A) * 0.5f);
        const FIntPoint BCheck = WorldToGrid(Mid + (B - A) * 0.5f);

        const bool bSame =
            (Key.A == ACheck && Key.B == BCheck) ||
            (Key.A == BCheck && Key.B == ACheck);

        UE_LOG(LogAS_GridPuzzle, Warning,
            TEXT("[WallCheck] Edge %s-%s | Mid=%s → A'=%s B'=%s %s"),
            *Key.A.ToString(), *Key.B.ToString(),
            *Mid.ToString(),
            *ACheck.ToString(), *BCheck.ToString(),
            bSame ? TEXT("✓") : TEXT("✗"));
    }
}

#if WITH_EDITOR
// 기존 액터 전부 제거 후 재배치
void UGridPuzzleManagerComponent::RecreateAllActors()
{
    UWorld* W = GetWorld(); if (!W) return;

    for (auto& Pair : FloorActors) Pair.Value->Destroy();
    for (auto& Pair : WallActors) Pair.Value->Destroy();

    FloorActors.Empty();
    WallActors.Empty();
    WallToKey.Empty();

    SpawnFloors();
    SpawnEdges();
}
#endif

//─────────────────────────────────────────────
