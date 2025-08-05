#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GridPuzzleManagerComponent.generated.h"

class AGridWallGCActor;
class AGridFloorGCActor;

///////////////////////////////////////////////////////////
// 셀 타입 및 구조체 정의
///////////////////////////////////////////////////////////

UENUM(BlueprintType)
enum class EGridCellType : uint8
{
    Empty,
    Floor
};

USTRUCT(BlueprintType)
struct FGridCellData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid|Cell")
    EGridCellType CellType = EGridCellType::Floor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid|Cell")
    int32 Health = 3;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Grid|Cell")
    bool bDestroyed = false;

    FVector WorldLocation;
};

USTRUCT(BlueprintType)
struct FGridEdge
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid|Edge")
    FIntPoint A;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid|Edge")
    FIntPoint B;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid|Edge")
    int32 Health = 3;

    bool operator==(const FGridEdge& Other) const
    {
        return A == Other.A && B == Other.B;
    }
};

FORCEINLINE uint32 GetTypeHash(const FGridEdge& Edge)
{
    return HashCombine(GetTypeHash(Edge.A), GetTypeHash(Edge.B));
}

DECLARE_LOG_CATEGORY_EXTERN(LogAS_GridPuzzle, Log, All);

///////////////////////////////////////////////////////////
// UGridPuzzleManagerComponent
///////////////////////////////////////////////////////////

UCLASS(ClassGroup=(Puzzle), meta=(BlueprintSpawnableComponent))
class ARCANESOULS_API UGridPuzzleManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UGridPuzzleManagerComponent();
    virtual void BeginPlay() override;

    // 그리드 초기화 및 리빌드
    UFUNCTION(CallInEditor, Category="Grid|Setup")
    void InitializeGrid();

    UFUNCTION(CallInEditor, Category="Grid|Setup")
    void RebuildMapping();

    UFUNCTION(CallInEditor, Category="Grid|Setup")
    void SpawnMissingActors();

    UFUNCTION(CallInEditor, Category="Grid|Setup")
    void RecreateAllActors();

    void SyncHealthFromPlacedActors();

    // 스펠 처리
    void UseFireSpell(const FIntPoint& PlayerCell, const FIntPoint& Dir);
    void UseIceSpell (const FIntPoint& TargetCell);
    void ApplyTraceFire(const FHitResult& Hit);

    // 좌표 변환 및 유효성 검사
    FIntPoint WorldToGrid(const FVector& WorldPos) const;
    FVector   GridToWorld(const FIntPoint& Coord) const;
    bool      IsInBounds(const FIntPoint& Coord) const;
    bool      IsFloor   (const FIntPoint& Coord) const;

    // 디버그
    void PrintGridToWorldCheck();
    void PrintWallKeyCheck();

    // 액터 참조
    AActor*             FindFloorActor(const FIntPoint& Coord) const;
    AGridFloorGCActor*  FindFloorActorByGridCoord(const FIntPoint& Coord) const;

protected:
    // 에디터 설정값
    UPROPERTY(EditAnywhere, Category="Grid|Config")
    int32 Rows = 10;

    UPROPERTY(EditAnywhere, Category="Grid|Config")
    int32 Cols = 10;

    UPROPERTY(EditAnywhere, Category="Grid|Config")
    float CellSize = 300.f;

    UPROPERTY(EditAnywhere, Category="Grid|Config")
    FVector2D GridOrigin = FVector2D::ZeroVector;

    // 프리팹 클래스
    UPROPERTY(EditDefaultsOnly, Category="Grid|Prefabs")
    TSubclassOf<AGridFloorGCActor> FloorClass;

    UPROPERTY(EditDefaultsOnly, Category="Grid|Prefabs")
    TSubclassOf<AGridWallGCActor> WallClass;

    // 런타임 데이터: 셀/엣지 정보
    UPROPERTY(VisibleAnywhere, Category="Grid|Runtime")
    TMap<FIntPoint, FGridCellData> GridMap;

    UPROPERTY(VisibleAnywhere, Category="Grid|Runtime")
    TSet<FGridEdge> EdgeSet;

    // 런타임 액터 매핑
    UPROPERTY()
    TMap<AGridWallGCActor*, FGridEdge> WallToKey;

    UPROPERTY()
    TMap<FIntPoint, AGridFloorGCActor*> FloorActors;

    UPROPERTY()
    TMap<FGridEdge, AGridWallGCActor*> WallActors;
    
protected:
    // 내부 유틸리티 함수
    FGridCellData* GetCellData(const FIntPoint& Coord);
    void ApplyDamageAround(const FIntPoint& Center);
    bool CanMove(const FIntPoint& From, const FIntPoint& Dir) const;

    void SpawnEdges();
    AGridFloorGCActor* SpawnFloor(const FIntPoint& Coord, int32 Health);
    AGridWallGCActor*  SpawnWall(const FGridEdge& E);

    void RefreshFloorVisual(const FIntPoint& Coord);
    void RefreshWallVisual (const FGridEdge& Edge);
};
