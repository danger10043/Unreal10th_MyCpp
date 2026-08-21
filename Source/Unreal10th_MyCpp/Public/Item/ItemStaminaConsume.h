// ItemStaminaConsume.h 수정

#include "CoreMinimal.h"
#include "Item/ItemBase.h"
#include "ItemStaminaConsume.generated.h"

class UItemDataAsset;

UCLASS()
class UNREAL10TH_MYCPP_API AItemStaminaConsume : public AItemBase
{
	GENERATED_BODY()

public:
	virtual void InitializeFromItemData(
		UItemDataAsset* InItemData
	) override;

protected:
	virtual void PickUpItem(AActor* InActor) override;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Item|Stamina",
		meta = (ClampMin = "0.0")
	)
	float StaminaAmount = 30.0f;

};