#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GridPuzzleManagerComponent.generated.h"

/*─────────────────────────────────────────────
 *  열거형 및 구조체
 *─────────────────────────────────────────────*/

/** 셀 타입 */
UENUM(BlueprintType)
enum class EGridCellType : uint8
{
    Empty,
    Floor
};

/** 셀 데이터 (바닥) */
USTRUCT(BlueprintType)
struct FGridCellData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid|Cell")
    EGridCellType CellType = EGridCellType::Floor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid|Cell")
    int32 Health = 3;  // 0‑파괴, 최대 3

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Grid|Cell")
    bool bDestroyed = false;

    FVector WorldLocation;  // 스폰 위치 캐시
};

/**
 * 두 셀 사이 경계를 나타내는 벽(Edge)
 * 항상 A < B 로 정렬하여 Key 충돌 방지
 */
USTRUCT(BlueprintType)
struct FGridEdge
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid|Edge")
    FIntPoint A;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid|Edge")
    FIntPoint B;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid|Edge")
    int32 Health = 3;  // 0‑파괴, 최대 3

    bool operator==(const FGridEdge& Other) const
    {
        return A == Other.A && B == Other.B;
    }
};

/** GetTypeHash 오버로드 – TSet / TMap Key 용 */
FORCEINLINE uint32 GetTypeHash(const FGridEdge& Edge)
{
    return HashCombine(GetTypeHash(Edge.A), GetTypeHash(Edge.B));
}

/*─────────────────────────────────────────────
 *  UGridPuzzleManagerComponent
 *─────────────────────────────────────────────*/

UCLASS(ClassGroup=(Puzzle), meta=(BlueprintSpawnableComponent))
class ARCANESOULS_API UGridPuzzleManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    /** 생성자 */
    UGridPuzzleManagerComponent();

    /** 그리드 및 Instanced Mesh 초기화 (에디터 호출용) */
    UFUNCTION(CallInEditor, Category="Grid|Setup")
    void InitializeGrid();

    /** 마법 효과 처리 */
    void UseFireSpell(const FIntPoint& PlayerCell, const FIntPoint& Dir);
    void UseIceSpell(const FIntPoint& TargetCell);

    /** 월드/그리드 좌표 변환 */
    FIntPoint WorldToGrid(const FVector& WorldPos) const;
    FVector   GridToWorld(const FIntPoint& Coord) const;

protected:
    /*─────────────────────
     * Config (디자이너 설정)
     *─────────────────────*/
    UPROPERTY(EditAnywhere, Category="Grid|Config")
    int32 Rows = 10;

    UPROPERTY(EditAnywhere, Category="Grid|Config")
    int32 Cols = 10;

    UPROPERTY(EditAnywhere, Category="Grid|Config")
    float CellSize = 300.f;

    /*─────────────────────
     * 프리팹 설정
     *─────────────────────*/
    UPROPERTY(EditDefaultsOnly, Category="Grid|Prefabs")
    TSubclassOf<AActor> FloorClass;

    UPROPERTY(EditDefaultsOnly, Category="Grid|Prefabs")
    TSubclassOf<AActor> WallClass;

    /*─────────────────────
     * 런타임 데이터
     *─────────────────────*/
    UPROPERTY(VisibleAnywhere, Category="Grid|Runtime")
    TMap<FIntPoint, FGridCellData> GridMap;

    UPROPERTY(VisibleAnywhere, Category="Grid|Runtime")
    TSet<FGridEdge> EdgeSet;

protected:
    /*─────────────────────
     * 내부 헬퍼 함수
     *─────────────────────*/

    /** 좌표 내 유효성 검사 */
    bool IsInBounds(const FIntPoint& Coord) const;

    /** 셀 데이터 가져오기 */
    FGridCellData* GetCellData(const FIntPoint& Coord);

    /** 데미지 처리 */
    void ApplyDamageAround(const FIntPoint& Center);

    /** 바닥/벽 스폰 */
    void SpawnFloors();
    void SpawnEdges();
};
