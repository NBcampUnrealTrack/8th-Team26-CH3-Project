#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "ReplayListEntryWidget.generated.h"

class UTextBlock;

UCLASS()
class TEAM26_API UReplayListEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TitleText = nullptr;
};