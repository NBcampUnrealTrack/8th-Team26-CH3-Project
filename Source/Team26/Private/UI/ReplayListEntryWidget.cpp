#include "UI/ReplayListEntryWidget.h"

#include "Components/TextBlock.h"
#include "Replay/ReplayType.h"

void UReplayListEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UReplayEntryObject* EntryObject = Cast<UReplayEntryObject>(ListItemObject);
	if (!EntryObject)
	{
		if (TitleText)
		{
			TitleText->SetText(FText::GetEmpty());
		}

		return;
	}

	const FReplayFileInfo& Info = EntryObject->Info;

	if (TitleText)
	{
		TitleText->SetText(FText::FromString(Info.DisplayName));
	}

}
