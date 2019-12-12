// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPawnMovementComponent.h"

#include "Kismet/KismetMathLibrary.h"

UMyPawnMovementComponent::UMyPawnMovementComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
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
	//�W�����v�������̏����x
	//�����̊֌W�Œ萔�ɂ��Ă��܂����A���̎�̒l�͒��������l�ɂȂ�n�Y�Ȃ̂ŊO���ݒ�ł���悤�ɂ��܂��B
	const float JUMP_INIT_SPEED = 236.0f;

	UpNowSpeed = JUMP_INIT_SPEED;
}

void UMyPawnMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ShouldSkipUpdate(DeltaTime)) {
		return;
	}

	//Update velocity
	{
		//���������ō����A���x�Ɋւ���p�����[�^�B�ȗ����Ă��܂�������������ƂȂ��ǂ��Ǝv���܂��B
		//�����̊֌W�Œ萔�ɂ��Ă��܂����A���̎�̒l�͒��������l�ɂȂ�n�Y�Ȃ̂ŊO���ݒ�ł���悤�ɂ��܂��B
		const float MAX_MOVE_ACCELERATION = 2360.0f;
		const float MAX_MOVE_DECELERATION = 2360.0f;
		const float MAX_MOVE_SPEED = 584.0f;

		//�����_�F�������̑��x�v�Z
		auto calcAccSpeed = [=](float inputAmount, float nowSpeed) {
			const float maxSpeed = FMath::Abs(MAX_MOVE_SPEED * inputAmount);
			nowSpeed += MAX_MOVE_ACCELERATION * DeltaTime * inputAmount;
			return FMath::Clamp(nowSpeed, -maxSpeed, maxSpeed);
		};
		//�����_�F�������̑��x�v�Z
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
		//�O������̑��x�v�Z
		if (0.0f < FMath::Abs(MoveInputAmount.Y)) {
			ForwardNowSpeed = calcAccSpeed(MoveInputAmount.Y, ForwardNowSpeed);
		}
		else {
			ForwardNowSpeed = calcDecSpeed(ForwardNowSpeed);
		}
		//���E�����̑��x�v�Z
		if (0.0f < FMath::Abs(MoveInputAmount.X)) {
			RightNowSpeed = calcAccSpeed(MoveInputAmount.X, RightNowSpeed);
		}
		else {
			RightNowSpeed = calcDecSpeed(RightNowSpeed);
		}
		//�ړ����x�̍X�V
		{
			const auto rotation = UpdatedComponent->GetComponentRotation();

			const auto forwardVector = UKismetMathLibrary::GetForwardVector(rotation);
			const auto rightVector = UKismetMathLibrary::GetRightVector(rotation);

			Velocity = (forwardVector * ForwardNowSpeed) + (rightVector * RightNowSpeed);
		}
	}

	//Update Gravity
	{
		UpNowSpeed -= GetGravityZ() * DeltaTime;
	}

	//Update Movement
	{
		FVector moveVec = Velocity * DeltaTime;
		if (!moveVec.IsNearlyZero())
		{
			MoveUpdatedComponent(moveVec, UpdatedComponent->GetComponentQuat(), true);
		}

		UpdateComponentVelocity();
	}
}
