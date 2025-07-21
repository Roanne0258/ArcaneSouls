// GridPuzzleManagerComponent.cpp – Designer‑editable health sync (BeginPlay)
#include "GridPuzzleManagerComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "GridFloorGCActor.h"
#include "GridWallGCActor.h"
#include "Kismet/GameplayStatics.h"

//─────────────────────────────────────────────
// 방향 상수
static const FIntPoint DirRight(1,0), DirUp(0,1), DirLeft(-1,0), DirDown(0,-1);
static const FIntPoint AroundOffsets[5]={{0,0},{1,0},{-1,0},{0,1},{0,-1}};

/* FIntPoint 비교 (A < B) */
static bool IsPointLess(const FIntPoint& A,const FIntPoint& B)
{ return (A.X<B.X) || (A.X==B.X && A.Y<B.Y); }

/* 정렬된 EdgeKey 생성 */
static FGridEdge MakeEdgeKey(const FIntPoint& P1,const FIntPoint& P2,int32 Default=3)
{ return IsPointLess(P1,P2) ? FGridEdge{P1,P2,Default} : FGridEdge{P2,P1,Default}; }

//─────────────────────────────────────────────
UGridPuzzleManagerComponent::UGridPuzzleManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

//─────────────────────────────────────────────
void UGridPuzzleManagerComponent::BeginPlay()
{
    Super::BeginPlay();

    /* ① 에디터에서 디자이너가 조정한 Health 를 런타임 데이터에 반영 */
    SyncHealthFromPlacedActors();
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
            if (X < Cols - 1)   EdgeSet.Add(MakeEdgeKey(C, C + DirRight));
            if (Y < Rows - 1)   EdgeSet.Add(MakeEdgeKey(C, C + DirUp));
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
// 좌표 변환
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
// 헬퍼
FGridCellData* UGridPuzzleManagerComponent::GetCellData(const FIntPoint& Coord)
{
    return GridMap.Find(Coord);
}

bool UGridPuzzleManagerComponent::IsInBounds(const FIntPoint& Coord) const
{
    return Coord.X >= 0 && Coord.X < Cols && Coord.Y >= 0 && Coord.Y < Rows;
}

bool UGridPuzzleManagerComponent::IsFloor(const FIntPoint& Coord) const
{
    if (const FGridCellData* Cell = GridMap.Find(Coord))
        return Cell->CellType == EGridCellType::Floor;
    return false;
}

//─────────────────────────────────────────────
// 건강 동기화 : BeginPlay 에 호출
void UGridPuzzleManagerComponent::SyncHealthFromPlacedActors()
{
    UWorld* World = GetWorld();
    if (!World) return;

    /* 바닥 셀 */
    TArray<AActor*> Floors;
    UGameplayStatics::GetAllActorsOfClass(World, AGridFloorGCActor::StaticClass(), Floors);
    for (AActor* Actor : Floors)
    {
        AGridFloorGCActor* Floor = Cast<AGridFloorGCActor>(Actor);
        if (!Floor) continue;

        const FIntPoint C = WorldToGrid(Floor->GetActorLocation());
        if (FGridCellData* Cell = GridMap.Find(C))
            Cell->Health = FMath::Clamp(Floor->Health, 0, 3);
    }

    /* 벽(Edge) */
    TArray<AActor*> Walls;
    UGameplayStatics::GetAllActorsOfClass(World, AGridWallGCActor::StaticClass(), Walls);
    for (AActor* Actor : Walls)
    {
        AGridWallGCActor* Wall = Cast<AGridWallGCActor>(Actor);
        if (!Wall) continue;

        // 두 인접 셀 계산 (Mid ± HalfCell in local yaw)
        const FVector Mid = Wall->GetActorLocation();
        const float   Half = CellSize * 0.5f;
        const bool    bHorizontal = FMath::IsNearlyZero(FMath::Fmod(Wall->GetActorRotation().Yaw, 180.f));

        FVector Offset = bHorizontal ? FVector(Half, 0, 0) : FVector(0, Half, 0);
        const FIntPoint A = WorldToGrid(Mid - Offset);
        const FIntPoint B = WorldToGrid(Mid + Offset);

        const FGridEdge Key = MakeEdgeKey(A, B);
        if (FGridEdge* Edge = EdgeSet.Find(Key))
            Edge->Health = FMath::Clamp(Wall->Health, 0, 3);
    }
}

//─────────────────────────────────────────────
// 스폰 함수들
void UGridPuzzleManagerComponent::SpawnFloors()
{
    if (!FloorClass) return;
    UWorld* World = GetWorld();
    if (!World) return;

    for (const auto& Elem : GridMap)
    {
        AGridFloorGCActor* Floor = World->SpawnActor<AGridFloorGCActor>(FloorClass, Elem.Value.WorldLocation, FRotator::ZeroRotator);
        Floor->Health = Elem.Value.Health;
#if WITH_EDITOR
        DrawDebugBox(World, Elem.Value.WorldLocation, FVector(CellSize*0.5f), FColor::Black, false, 10.f);
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
        const float Yaw = (Dir.X != 0) ? 0.f : 90.f;

        AGridWallGCActor* Wall = World->SpawnActor<AGridWallGCActor>(WallClass, Mid, FRotator(0.f, Yaw, 0.f));
        Wall->Health = E.Health;
#if WITH_EDITOR
        DrawDebugLine(World, A, B, FColor::Red, false, 10.f, 0, 10.f);
#endif
    }
}

/*────────────────────────────────────────────
 *  Spell Logic
 *──────────────────────────────────────────*/
void UGridPuzzleManagerComponent::UseFireSpell(const FIntPoint& Cell,const FIntPoint& Dir)
{
    /* 1) 자기 발판 –1 */
    if (FGridCellData* Self = GridMap.Find(Cell))
        Self->Health = FMath::Max(0, Self->Health - 1);

    /* 2) 바라보는 벽 –1 */
    const FIntPoint Other = Cell + Dir;
    if (FGridEdge* Edge = EdgeSet.Find(MakeEdgeKey(Cell, Other)))
        Edge->Health = FMath::Max(0, Edge->Health - 1);

    // TODO: 파괴 FX
}

void UGridPuzzleManagerComponent::UseIceSpell(const FIntPoint& Target)
{
    if (FGridCellData* Cell = GridMap.Find(Target))
    {
        if (Cell->CellType == EGridCellType::Floor && Cell->Health < 3)
            ++Cell->Health;
    }
}

void UGridPuzzleManagerComponent::ApplyDamageAround(const FIntPoint& Center)
{
    for (const FIntPoint& Off : AroundOffsets)
    {
        if (FGridCellData* Cell = GridMap.Find(Center+Off))
        {
            if (!Cell->bDestroyed && Cell->Health>0)
            {
                if (--Cell->Health==0) Cell->bDestroyed=true; // TODO: 파괴 비주얼
            }
        }
    }
}

bool UGridPuzzleManagerComponent::CanMove(const FIntPoint& From,const FIntPoint& Dir) const
{
    const FIntPoint To = From+Dir;
    // 1) 셀 존재 & 파괴 여부
    const FGridCellData* Dest = GridMap.Find(To);
    if (!Dest || Dest->bDestroyed || Dest->Health<=0) return false;

    // 2) 벽 체크
    const FGridEdge Key = MakeEdgeKey(From,To);
    if (const FGridEdge* Edge = EdgeSet.Find(Key))
    {
        return Edge->Health<=0; // 파괴되었거나 health 0 이면 통과 가능
    }
    return true; // 벽이 없으면 통과 가능
}
