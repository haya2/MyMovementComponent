// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPawnMovementComponent.h"

#include "Kismet/KismetMathLibrary.h"

UMyPawnMovementComponent::UMyPawnMovementComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
	,ForwardNowSpeed(0.0f)
	,RightNowSpeed(0.0f)
	,UpNowSpeed(0.0f)
	,IsFalling(false)
{
}

void UMyPawnMovementComponent::SetInputAmount_MoveForward(float InputAmount)
{
	MoveInputAmount.Y = InputAmount;
}

void UMyPawnMovementComponent::SetInputAmount_MoveRight(float InputAmount)
{
	MoveInputAmount.X = InputAmount;
}

void UMyPawnMovementComponent::Jump()
{
	//ジャンプした時の初速度
	//説明の関係で定数にしていますが、この手の値は調整される値になるハズなので外部設定できるようにします。
	const float INIT_JUMP_SPEED = 930.0f;

	UpNowSpeed = INIT_JUMP_SPEED;

	IsFalling = true;
}

void UMyPawnMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ShouldSkipUpdate(DeltaTime)) {
		return;
	}

	//Update velocity
	{
		//加速減速最高速、速度に関するパラメータ。省略していますが初速もあるとなお良いと思います。
		//説明の関係で定数にしていますが、この手の値は調整される値になるハズなので外部設定できるようにします。
		const float MAX_MOVE_ACCELERATION = 2360.0f;
		const float MAX_MOVE_DECELERATION = 2360.0f;
		const float MAX_MOVE_SPEED = 584.0f;

		//ラムダ：加速時の速度計算
		auto calcAccSpeed = [=](float inputAmount, float nowSpeed) {
			const float maxSpeed = FMath::Abs(MAX_MOVE_SPEED * inputAmount);
			nowSpeed += MAX_MOVE_ACCELERATION * DeltaTime * inputAmount;
			return FMath::Clamp(nowSpeed, -maxSpeed, maxSpeed);
		};
		//ラムダ：減速時の速度計算
		auto calcDecSpeed = [=](float nowSpeed) {
			if (0.0f < nowSpeed) {
				float calcSpeed = nowSpeed - (MAX_MOVE_DECELERATION * DeltaTime);
				return FMath::Clamp(calcSpeed, 0.0f, nowSpeed);
			}
			else if (0.0f > nowSpeed) {
				float calcSpeed = nowSpeed + (MAX_MOVE_DECELERATION * DeltaTime);
				return FMath::Clamp(calcSpeed, nowSpeed, 0.0f);
			}
			return nowSpeed;
		};
		//前後方向の速度計算
		if (0.0f < FMath::Abs(MoveInputAmount.Y)) {
			ForwardNowSpeed = calcAccSpeed(MoveInputAmount.Y, ForwardNowSpeed);
		}
		else {
			ForwardNowSpeed = calcDecSpeed(ForwardNowSpeed);
		}
		//左右方向の速度計算
		if (0.0f < FMath::Abs(MoveInputAmount.X)) {
			RightNowSpeed = calcAccSpeed(MoveInputAmount.X, RightNowSpeed);
		}
		else {
			RightNowSpeed = calcDecSpeed(RightNowSpeed);
		}
		//移動速度の更新
		{
			const auto rotation = UpdatedComponent->GetComponentRotation();

			const auto forwardVector = UKismetMathLibrary::GetForwardVector(rotation);
			const auto rightVector = UKismetMathLibrary::GetRightVector(rotation);

			Velocity = (forwardVector * ForwardNowSpeed) + (rightVector * RightNowSpeed);
		}
	}

	//Update Gravity
	if (IsFalling)
	{
		//落下中の最高速度
		//説明の関係で定数にしていますが、この手の値は調整される値になるハズなので外部設定できるようにします。
		const float MAX_FALL_SPEED = 5000.0f;
		const float GRAVITY_SCALE = 2.0f;

		UpNowSpeed += GetGravityZ() * GRAVITY_SCALE * DeltaTime;
		UpNowSpeed = FMath::Clamp(UpNowSpeed, -MAX_FALL_SPEED, MAX_FALL_SPEED);

		{
			const auto rotation = UpdatedComponent->GetComponentRotation();

			const auto upVector = UKismetMathLibrary::GetUpVector(rotation);

			Velocity += (upVector * UpNowSpeed);
		}
	}

	//Update Movement
	{
		FVector moveVec = Velocity * DeltaTime;
		if (!moveVec.IsNearlyZero())
		{
			FHitResult hitResult;
			MoveUpdatedComponent(moveVec, UpdatedComponent->GetComponentRotation(), true, &hitResult);

			if (hitResult.bBlockingHit)
			{
				//
				FHitResult hitSlide = hitResult;
				SlideAlongSurface(moveVec, 1.0f - hitResult.Time, hitResult.Normal, hitSlide, true);

				//落下中の着地判定
				if (IsFalling)
				{
					const auto hitComponent = hitResult.GetComponent();
					if (hitComponent)
					{
						if (hitComponent->CanCharacterStepUp(nullptr))
						{
							IsFalling = false;

							UpNowSpeed = 0.0f;
						}
					}
				}
			}
		}

		UpdateComponentVelocity();
	}
}
