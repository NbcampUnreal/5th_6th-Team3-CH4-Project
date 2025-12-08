// TTPlayerCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "TTPlayerCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;

UCLASS()
class TEAM03_API ATTPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATTPlayerCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpringArm")
	TObjectPtr<USpringArmComponent> SpringArm;
	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Camera" )
	TObjectPtr<UCameraComponent> Camera;

#pragma region Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> Move;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Input" )
	TObjectPtr<UInputAction> Look;
	TObjectPtr<UInputAction> Jump;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Input" )
	TObjectPtr<UInputAction> Attack;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Input" )
	TObjectPtr<UInputMappingContext> IMC;

public:
	virtual void SetupPlayerInputComponent ( class UInputComponent* PlayerInputComponent ) override;
	
	virtual void BeginPlay () override;

protected:
	void Move ( const FInputActionValue& Value );
	void Look ( const FInputActionValue& Value );
	void InputAttack();
	
#pragma endregion
};
