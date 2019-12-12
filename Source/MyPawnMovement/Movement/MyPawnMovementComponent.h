// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "MyPawnMovementComponent.generated.h"

UCLASS(ClassGroup = Movement, meta = (BlueprintSpawnableComponent))
class MYPAWNMOVEMENT_API UMyPawnMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	//前方向の入力量を設定する
	UFUNCTION(BlueprintCallable)
	void SetInputAmount_MoveForward(float InputAmount);

	//右方向の入力量を設定する
	UFUNCTION(BlueprintCallable)
	void SetInputAmount_MoveRight(float InputAmount);

public:
	//ジャンプする
	UFUNCTION(BlueprintCallable)
	void Jump();

public:
	UMyPawnMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	//速度計算で使う移動の入力量
	FVector	MoveInputAmount;

	//現在の移動速度。前後
	float	ForwardNowSpeed;
	//現在の移動速度。左右
	float	RightNowSpeed;
	//現在の移動速度。上下
	float	UpNowSpeed;
};
