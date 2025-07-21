#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlacementHelperActor.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogASPlacement, Log, All);

/**
 * Editor 전용 배치 도우미 액터
 * - AlignToGround : 지면에 Z 위치 맞춤 (피벗 또는 메시 하단 기준)
 * - SnapToGrid    : 지정된 그리드 간격으로 위치 스냅
 *
 * Header 순서: public vars → public funcs → protected vars → protected funcs
 */
UCLASS()
class ARCANESOULS_API APlacementHelperActor : public AActor
{
	GENERATED_BODY()

	/* ──────────────── Public Vars ──────────────── */
public:

	/** 배치 대상 메시 (인스턴스 전용) */
	UPROPERTY(EditInstanceOnly, Category="Placement")
	UStaticMeshComponent* MeshComp = nullptr;

	/** 이동 또는 변경 시 자동 지면 정렬 여부 */
	UPROPERTY(EditAnywhere, Category="Placement")
	bool bAutoAlign = false;

	/** 슬로프 정렬 시 Yaw 회전도 노멀에 맞출지 여부 */
	UPROPERTY(EditAnywhere, Category="Placement|Slope")
	bool bAlignYawToSlope = true;

	/** 위치 초기화 기준점 */
	UPROPERTY(EditAnywhere, Category="Placement|Reset")
	FVector ResetLocationOrigin = FVector::ZeroVector;

	/** 회전 초기화 값 */
	UPROPERTY(EditAnywhere, Category="Placement|Reset",
			  meta=(ToolTip="Yaw·Pitch·Roll 전부 0°로 하려면 (0,0,0)"))
	FRotator ResetRotationValue = FRotator::ZeroRotator;

	/**
	 * 무작위 Yaw 회전 간격 (°)
	 * - 0    : 완전 랜덤
	 * - 15   : 0, 15, 30… 식으로 회전
	 * - 180  : 0 또는 180 (좌우 전용)
	 */
	UPROPERTY(EditAnywhere, Category="Placement|RandomYaw",
			  meta=(ClampMin="0.0", ClampMax="180.0"))
	float RandomYawStep = 15.f;

	/** 무작위 위치 이동 반경 (cm) */
	UPROPERTY(EditAnywhere, Category="Placement|RandomLoc",
			  meta=(ClampMin="1.0", ToolTip="무작위 이동 반경(cm)"))
	float RandomRadius = 200.f;

	/** 머티리얼 순환 배열 (첫 슬롯 대상) */
	UPROPERTY(EditInstanceOnly, Category="MaterialCycler",
			  meta=(ToolTip="버튼 클릭 시 차례로 적용할 머티리얼 목록"))
	TArray<UMaterialInterface*> MaterialList;

	/** 충돌 방식 순환 배열: No → Overlap → Block */
	UPROPERTY(EditAnywhere, Category="CollisionToggle",
			  meta=(ToolTip="ToggleCollision() 버튼 순환 순서"))
	TArray<TEnumAsByte<ECollisionEnabled::Type>> CollisionCycle =
	{
		ECollisionEnabled::NoCollision,
		ECollisionEnabled::QueryOnly,
		ECollisionEnabled::QueryAndPhysics
	};

	/* ──────────────── Public Funcs ─────────────── */
public:

	/** 기본 생성자 */
	APlacementHelperActor();

	/** 위치를 ResetLocationOrigin 으로 이동 */
	UFUNCTION(CallInEditor, BlueprintCallable, Category="Placement|Reset")
	void ResetLocation();

	/** 회전을 ResetRotationValue 로 설정 */
	UFUNCTION(CallInEditor, BlueprintCallable, Category="Placement|Reset")
	void ResetRotation();

	/** XY 무작위 이동 (중심 유지, 반경 RandomRadius) */
	UFUNCTION(CallInEditor, BlueprintCallable, Category="Placement")
	void RandomLocationInRadius();

	/** 지면(Pivot 또는 Mesh Bottom)으로 스냅 */
	UFUNCTION(CallInEditor, BlueprintCallable, Category="Placement")
	void AlignToGround();

	/** 슬로프 정렬 (Pitch·Roll만 맞춤) */
	UFUNCTION(CallInEditor, BlueprintCallable, Category="Placement")
	void AlignSlope();

	/** 무작위 Yaw 회전 (Step 적용 가능) */
	UFUNCTION(CallInEditor, BlueprintCallable, Category="Placement")
	void RandomYaw();

	/** 머티리얼 리스트 순환 적용 */
	UFUNCTION(CallInEditor, BlueprintCallable, Category="MaterialCycler")
	void CycleMaterial();

	/** CollisionEnabled 상태 순환 */
	UFUNCTION(CallInEditor, BlueprintCallable, Category="CollisionToggle")
	void ToggleCollision();

	/* ───────────── Protected Funcs ────────────── */
protected:

	/** Transform 변경 시 호출됨 (에디터 전용) */
	virtual void OnConstruction(const FTransform& Transform) override;

	/* ──────────────── Private Vars ─────────────── */
private:

	/** true = 메시 하단 기준, false = 피벗 기준 */
	bool bUseMeshBottom = true;

	/** 머티리얼 순서 인덱스 */
	int32 CurrentMatIdx = -1;

	/** 충돌 순서 인덱스 */
	int32 CurrentCollIdx = -1;

private:

};
