#include "DataLogger/VehicleGeographicHudWidget.h"
#include "Components/TextBlock.h"

namespace
{
	const FText DefaultGeographicText = NSLOCTEXT(
		"VehicleGeographicHudWidget",
		"DefaultGeographicText",
		"Waiting for geographic location."
	);
}

void UVehicleGeographicHudWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ensureMsgf(LocationText, TEXT("VehicleGeographicHudWidget requires a TextBlock named LocationText in the widget blueprint."));

	SetGeographicText(DefaultGeographicText);
}

void UVehicleGeographicHudWidget::SetGeographicText(const FText& NewText)
{
	if (LocationText)
	{
		LocationText->SetText(NewText);
	}
}
