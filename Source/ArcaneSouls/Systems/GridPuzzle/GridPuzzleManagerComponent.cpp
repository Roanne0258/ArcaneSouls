// GridPuzzleManagerComponent.cpp – Edge‑기반 벽 배치 버전 (Fix: FIntPoint 비교)
#include "GridPuzzleManagerComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

//─────────────────────────────────────────────
// ⬇︎ 방향 상수
static const FIntPoint DirRight(1, 0);
static const FIntPoint DirUp   (0, 1);
static const FIntPoint DirLeft (-1,0);
static const FIntPoint DirDown (0,-1);

/** A가 B보다 작은지를 정렬용으로 판단 (X → Y 순)
 *  FIntPoint에는 operator<가 없으므로 수동 비교 */
static bool IsPointLess(const FIntPoint& A, const FIntPoint& B)
{
    return (A.X < B.X) || (A.X == B.X && A.Y < B.Y);
}

/** 두 셀 좌표를 받아 정렬된 EdgeKey 생성 */
static FGridEdge MakeEdgeKey(const FIntPoint& P1, const FIntPoint& P2, int32 DefaultHealth = 3)
{
    return IsPointLess(P1, P2)
        ? FGridEdge{P1, P2, DefaultHealth}
        : FGridEdge{P2, P1, DefaultHealth};
}

//─────────────────────────────────────────────
UGridPuzzleManagerComponent::UGridPuzzleManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

//─────────────────────────────────────────────
void UGridPuzzleManagerComponent::InitializeGrid()
{
    /* 1) 셀 초기화 */
    GridMap.Empty(Rows * Cols);
    for (int32 Y = 0; Y < Rows; ++Y)
    {
        for (int32 X = 0; X < Cols; ++X)
        {
            const FIntPoint Coord(X, Y);
            FGridCellData   Cell;
            Cell.CellType      = EGridCellType::Floor;
            Cell.Health        = 3;
            Cell.WorldLocation = GridToWorld(Coord);
            GridMap.Add(Coord, Cell);
        }
    }

    /* 2) EdgeSet 초기화 */
    EdgeSet.Empty();
    for (int32 Y = 0; Y < Rows; ++Y)
    {
        for (int32 X = 0; X < Cols; ++X)
        {
            const FIntPoint C(X, Y);

            // 내부 Edge (오른쪽/위쪽)
            if (X < Cols - 1)   EdgeSet.Add(MakeEdgeKey(C, C + DirRight));
            if (Y < Rows - 1)   EdgeSet.Add(MakeEdgeKey(C, C + DirUp));

            // 외곽 Edge (바깥 방향)
            if (X == 0)         EdgeSet.Add(MakeEdgeKey(C, C + DirLeft));
            if (X == Cols - 1)  EdgeSet.Add(MakeEdgeKey(C, C + DirRight));
            if (Y == 0)         EdgeSet.Add(MakeEdgeKey(C, C + DirDown));
            if (Y == Rows - 1)  EdgeSet.Add(MakeEdgeKey(C, C + DirUp));
        }
    }

    /* 3) 액터 스폰 */
    SpawnFloors();
    SpawnEdges();
}

//─────────────────────────────────────────────
FVector UGridPuzzleManagerComponent::GridToWorld(const FIntPoint& Coord) const
{
    return FVector(Coord.X * CellSize, Coord.Y * CellSize, 0.f);
}

FIntPoint UGridPuzzleManagerComponent::WorldToGrid(const FVector& Pos) const
{
    return FIntPoint(
        FMath::RoundToInt(Pos.X / CellSize),
        FMath::RoundToInt(Pos.Y / CellSize));
}

//─────────────────────────────────────────────
FGridCellData* UGridPuzzleManagerComponent::GetCellData(const FIntPoint& Coord)
{
    return GridMap.Find(Coord);
}

bool UGridPuzzleManagerComponent::IsInBounds(const FIntPoint& Coord) const
{
    return Coord.X >= 0 && Coord.X < Cols && Coord.Y >= 0 && Coord.Y < Rows;
}

//─────────────────────────────────────────────
void UGridPuzzleManagerComponent::SpawnFloors()
{
    if (!FloorClass) return;
    UWorld* World = GetWorld();
    if (!World) return;

    for (const auto& Elem : GridMap)
    {
        World->SpawnActor<AActor>(FloorClass, Elem.Value.WorldLocation, FRotator::ZeroRotator);
#if WITH_EDITOR
        DrawDebugBox(World, Elem.Value.WorldLocation, FVector(CellSize*0.5f), FColor::Black, true, 10.f);
#endif
    }
}

void UGridPuzzleManagerComponent::SpawnEdges()
{
    if (!WallClass) return;
    UWorld* World = GetWorld();
    if (!World) return;

    for (const FGridEdge& E : EdgeSet)
    {
        const FVector A = GridToWorld(E.A);
        const FVector B = GridToWorld(E.B);
        const FVector Mid = (A + B) * 0.5f;
        const FIntPoint Dir = E.B - E.A;
        const float Yaw = (Dir.X != 0) ? 0.f : 90.f; // 수평 0°, 수직 90°

        World->SpawnActor<AActor>(WallClass, Mid, FRotator(0.f, Yaw, 0.f));
#if WITH_EDITOR
        DrawDebugLine(World, A, B, FColor::Red, true, 10.f, 0, 10.f);
#endif
    }
}
