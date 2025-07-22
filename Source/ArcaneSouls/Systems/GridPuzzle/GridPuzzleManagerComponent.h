#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GridPuzzleManagerComponent.generated.h"

/*─────────────────────────────────────────────
 *  열거형 및 구조체
 *─────────────────────────────────────────────*/

class AGridWallGCActor;
class AGridFloorGCActor;

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
    int32 Health = 3;                // 0-파괴, 최대 3

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Grid|Cell")
    bool bDestroyed = false;

    FVector WorldLocation;           // 스폰 위치 캐시
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
    int32 Health = 3;                // 0-파괴, 최대 3

    bool operator==(const FGridEdge& Other) const
    {
        return A == Other.A && B == Other.B;
    }
};

FORCEINLINE uint32 GetTypeHash(const FGridEdge& Edge)
{
    return HashCombine(GetTypeHash(Edge.A), GetTypeHash(Edge.B));
}

/*─────────────────────────────────────────────
 *  UGridPuzzleManagerComponent
 *─────────────────────────────────────────────*/

DECLARE_LOG_CATEGORY_EXTERN(LogAS_GridPuzzle, Log, All);

UCLASS(ClassGroup=(Puzzle), meta=(BlueprintSpawnableComponent))
class ARCANESOULS_API UGridPuzzleManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UGridPuzzleManagerComponent();
    virtual void BeginPlay() override;

    /* 초기화 • 동기화 */
    UFUNCTION(CallInEditor, Category="Grid|Setup")
    void InitializeGrid();

    /** 월드에 배치된 Floor/Wall 액터를 그리드 데이터에 매핑  */
    UFUNCTION(CallInEditor, Category="Grid|Setup")
    void RebuildMapping();

    UFUNCTION(CallInEditor, Category="Grid|Setup")
    void SpawnMissingActors();
    
    UFUNCTION(CallInEditor, Category="Grid|Setup")
    void RecreateAllActors();
    
    void SyncHealthFromPlacedActors();

    /* 스펠 */
    void UseFireSpell(const FIntPoint& PlayerCell, const FIntPoint& Dir);
    void UseIceSpell (const FIntPoint& TargetCell);
    void ApplyTraceFire(const FHitResult& Hit);

    /* 좌표 변환 */
    FIntPoint WorldToGrid(const FVector& WorldPos) const;
    FVector   GridToWorld(const FIntPoint& Coord) const;
    AActor* FindFloorActor(const FIntPoint& Coord) const;

    /* 유효성 */
    bool IsInBounds(const FIntPoint& Coord) const;
    bool IsFloor   (const FIntPoint& Coord) const;
    void PrintGridToWorldCheck();
    void PrintWallKeyCheck();

protected:
    /*──────── Config ────────*/
    UPROPERTY(EditAnywhere, Category="Grid|Config") int32 Rows     = 10;
    UPROPERTY(EditAnywhere, Category="Grid|Config") int32 Cols     = 10;
    UPROPERTY(EditAnywhere, Category="Grid|Config") float CellSize = 300.f;
    UPROPERTY(EditAnywhere, Category="Grid|Config")
    FVector2D GridOrigin = FVector2D::ZeroVector;

    /*──────── Prefabs ───────*/
    UPROPERTY(EditDefaultsOnly, Category="Grid|Prefabs")
    TSubclassOf<AGridFloorGCActor> FloorClass;
    UPROPERTY(EditDefaultsOnly, Category="Grid|Prefabs")
    TSubclassOf<AGridWallGCActor>  WallClass;

    /*──────── Runtime Data ──*/
    UPROPERTY(VisibleAnywhere, Category="Grid|Runtime")
    TMap<FIntPoint, FGridCellData> GridMap;
    UPROPERTY(VisibleAnywhere, Category="Grid|Runtime")
    TSet<FGridEdge> EdgeSet;
    UPROPERTY() TMap<AGridWallGCActor*, FGridEdge> WallToKey;

    /* 런타임 액터 참조 맵 */
    UPROPERTY() TMap<FIntPoint, AGridFloorGCActor*> FloorActors;
    UPROPERTY() TMap<FGridEdge , AGridWallGCActor*> WallActors;

    /*──────── Helpers ───────*/
    FGridCellData* GetCellData(const FIntPoint& Coord);
    void ApplyDamageAround(const FIntPoint& Center);
    bool CanMove(const FIntPoint& From, const FIntPoint& Dir) const;

    void SpawnFloors();
    void SpawnEdges();

    void RefreshFloorVisual(const FIntPoint& Coord);
    void RefreshWallVisual (const FGridEdge& Edge);
};
