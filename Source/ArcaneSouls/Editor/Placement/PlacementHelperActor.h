#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlacementHelperActor.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogASPlacement, Log, All);

/**
 *  Editor-only helper actor
 *  - AlignToGround : 지면에 Z 위치 맞춤 (피벗 or 메시 하단)
 *  - SnapToGrid    : 지정 그리드 간격으로 위치 스냅
 *
 *  Header order :  public vars → public funcs → protected vars → protected funcs
 */
UCLASS()
class ARCANESOULS_API APlacementHelperActor : public AActor
{
	GENERATED_BODY()

	/* ──────────────── Public Vars ──────────────── */
public:
	/** 배치 대상 메시 (Instance Editable) */
	UPROPERTY(EditInstanceOnly, Category="Placement")
	UStaticMeshComponent* MeshComp = nullptr;

	/** 이동·변경 시 자동 지면 정렬 여부 */
	UPROPERTY(EditAnywhere, Category="Placement")
	bool bAutoAlign = true;
	
	/** Slope 정렬 시 Yaw 도 노멀로 맞출지 여부 (기본 true) */
	UPROPERTY(EditAnywhere, Category="Placement|Slope")
	bool bAlignYawToSlope = true;

	UPROPERTY(EditAnywhere, Category="Placement|Reset")
	FVector ResetLocationOrigin = FVector::ZeroVector;   // 인스턴스별 초기화 좌표

	UPROPERTY(EditAnywhere, Category="Placement|Reset",
			  meta=(ToolTip="Yaw·Pitch·Roll 전부 0°로 하려면 (0,0,0)"))
	FRotator ResetRotationValue = FRotator::ZeroRotator; // 인스턴스별 초기화 회전

	/**
	 * RandomYaw 간격(도)
	 * -  0   : 완전 랜덤
	 * - 15   : 0·15·30… 15° 단위
	 * - 180  : 0 또는 180 (최소 좌·우 전용)
	 */
	UPROPERTY(EditAnywhere, Category="Placement|RandomYaw",
			  meta=(ClampMin="0.0", ClampMax="180.0"))
	float RandomYawStep = 15.f;

	UPROPERTY(EditAnywhere, Category="Placement|RandomLoc",
		  meta=(ClampMin="1.0", ToolTip="무작위 이동 반경(cm)"))
	float RandomRadius = 200.f;

	/** 머티리얼 순환용 배열(첫 번째 슬롯 전용) ★ NEW */
	UPROPERTY(EditInstanceOnly, Category="MaterialCycler",
			  meta=(ToolTip="버튼 클릭 시 차례로 적용할 머티리얼 목록"))
	TArray<UMaterialInterface*> MaterialList;

	/** 충돌 순환 모드: No → Overlap → Block ★ NEW */
	UPROPERTY(EditAnywhere, Category="CollisionToggle",
			  meta=(ToolTip="ToggleCollision() 버튼 순환 순서"))
	TArray<TEnumAsByte<ECollisionEnabled::Type>> CollisionCycle
		= { ECollisionEnabled::NoCollision,
			ECollisionEnabled::QueryOnly,
			ECollisionEnabled::QueryAndPhysics };
	/* ──────────────── Public Funcs ─────────────── */
public:
	/** 기본 생성자 */
	APlacementHelperActor();

	/** 위치를 ResetLocationOrigin 으로 이동 */
	UFUNCTION(CallInEditor, Category="Placement|Reset")
	void ResetLocation();

	/** 회전을 ResetRotationValue 로 설정 */
	UFUNCTION(CallInEditor, Category="Placement|Reset")
	void ResetRotation();
	
	/** 중심은 유지하고 XY만 무작위 이동(반경 RandomRadius) */
	UFUNCTION(CallInEditor, Category="Placement")
	void RandomLocationInRadius();

	/** 지면 정렬 버튼 */
	UFUNCTION(CallInEditor, Category="Placement")
	void AlignToGround();

	/* 정렬: 메시 하단 노멀(Pitch·Roll) 맞추기 */
	UFUNCTION(CallInEditor, Category="Placement")
	void AlignSlope();     // ★ NEW
	/* 랜덤 회전: 반복 티 제거 */
	UFUNCTION(CallInEditor, Category="Placement")
	void RandomYaw();      // ★ NEW

	/** 머티리얼을 배열 순서로 순환 적용 ★ NEW */
	UFUNCTION(CallInEditor, Category="MaterialCycler")
	void CycleMaterial();

	/** CollisionEnabled 를 Cycle 순서대로 토글 ★ NEW */
	UFUNCTION(CallInEditor, Category="CollisionToggle")
	void ToggleCollision();

	/* ─────────────── Protected Funcs ───────────── */
protected:
	/** 에디터에서 Transform 수정 시 자동 호출 */
	virtual void OnConstruction(const FTransform& Transform) override;

	/* ──────────────── Private Vars ─────────────── */
private:
	/** true = 메시 하단 기준, false = 피벗 기준 */
	bool bUseMeshBottom = true;
	int32 CurrentMatIdx   = -1;   // ★ NEW
	int32 CurrentCollIdx  = -1;   // ★ NEW
};
