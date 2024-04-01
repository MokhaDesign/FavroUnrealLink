// Fill out your copyright notice in the Description page of Project Settings.


#include "Slate/Tabs/FavroLinkSettings.h"

#include "F2UESettings.h"
#include "F2UEStyle.h"
#include "ISinglePropertyView.h"
#include "SlateOptMacros.h"
#include "Models/FavroLinkModel.h"
#include "Widgets/Layout/SSeparator.h"
#include "Styling/StyleColors.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SFavroLinkSettings::Construct(const FArguments& InArgs, const TSharedRef<FFavroLinkModel>& InModel)
{
	Model = InModel;
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	const TObjectPtr<UF2UESettings> Settings = GetMutableDefault<UF2UESettings>();
	const TSharedPtr<SVerticalBox> VerticalBox = SNew(SVerticalBox);

	FSinglePropertyParams InitParams;
	InitParams.Font = FAppStyle::GetFontStyle("PropertyWindow.NormalFont");
	InitParams.NamePlacement = EPropertyNamePlacement::Hidden;
	InitParams.NotifyHook = Model.Get();
	const TSharedPtr<ISinglePropertyView> UsernameProp = PropertyModule.CreateSingleProperty(
		Settings, GET_MEMBER_NAME_CHECKED(UF2UESettings, Username), InitParams);
	const TSharedPtr<ISinglePropertyView> ApiTokenProp = PropertyModule.CreateSingleProperty(
		Settings, GET_MEMBER_NAME_CHECKED(UF2UESettings, ApiToken), InitParams);
	const TSharedPtr<ISinglePropertyView> OrgNameProp = PropertyModule.CreateSingleProperty(
		Settings, GET_MEMBER_NAME_CHECKED(UF2UESettings, OrganizationName), InitParams);
	const TSharedPtr<ISinglePropertyView> OrganizationsInterval = PropertyModule.CreateSingleProperty(
		Settings, GET_MEMBER_NAME_CHECKED(UF2UESettings, OrganizationsInvalidateInterval), InitParams);
	const TSharedPtr<ISinglePropertyView> CollectionsInterval = PropertyModule.CreateSingleProperty(
		Settings, GET_MEMBER_NAME_CHECKED(UF2UESettings, CollectionsInvalidateInterval), InitParams);
	const TSharedPtr<ISinglePropertyView> DashboardInterval = PropertyModule.CreateSingleProperty(
		Settings, GET_MEMBER_NAME_CHECKED(UF2UESettings, DashboardInvalidateInterval), InitParams);

	VerticalBox->AddSlot()[MakeCategoryHeader("Authentication")].AutoHeight();

	VerticalBox->AddSlot()[SNew(SSeparator).Thickness(1.f)].AutoHeight();
	VerticalBox->AddSlot()[MakePropertyRow(UsernameProp, true)].AutoHeight();
	VerticalBox->AddSlot()[SNew(SSeparator).Thickness(1.f)].AutoHeight();
	VerticalBox->AddSlot()[MakePropertyRow(ApiTokenProp, true)].AutoHeight();
	VerticalBox->AddSlot()[SNew(SSeparator).Thickness(1.f)].AutoHeight();
	VerticalBox->AddSlot()[MakePropertyRow(OrgNameProp)].AutoHeight();
	VerticalBox->AddSlot()[SNew(SSeparator).Thickness(1.f)].AutoHeight();

	const TSharedRef<SHorizontalBox> AuthButtons = SNew(SHorizontalBox);

	AuthButtons->AddSlot()[MakeButton("Authenticate",
	                                  FOnClicked::CreateRaw(Model.Get(), &FFavroLinkModel::HandleAuthenticate))].
		AutoWidth();
	VerticalBox->AddSlot()[AuthButtons].HAlign(HAlign_Right).Padding(0.f, 0.f, 30.f, 0.f).AutoHeight();

	VerticalBox->AddSlot()[MakeCategoryHeader("Caching")].AutoHeight();
	VerticalBox->AddSlot()[SNew(SSeparator).Thickness(1.f)].AutoHeight();
	VerticalBox->AddSlot()[MakePropertyRow(OrganizationsInterval)].AutoHeight();
	VerticalBox->AddSlot()[SNew(SSeparator).Thickness(1.f)].AutoHeight();
	VerticalBox->AddSlot()[MakePropertyRow(CollectionsInterval)].AutoHeight();
	VerticalBox->AddSlot()[SNew(SSeparator).Thickness(1.f)].AutoHeight();
	VerticalBox->AddSlot()[MakePropertyRow(DashboardInterval)].AutoHeight();
	VerticalBox->AddSlot()[SNew(SSeparator).Thickness(1.f)].AutoHeight();

	const TSharedRef<SHorizontalBox> CacheButtons = SNew(SHorizontalBox);
	CacheButtons->AddSlot()[MakeButton("Force Organizations Refresh",
	                                   FOnClicked::CreateRaw(Model.Get(), &FFavroLinkModel::HandleForceRefresh,
	                                                         false))].
		AutoWidth();
	CacheButtons->AddSlot()[MakeButton("Force Collections Refresh",
	                                   FOnClicked::CreateRaw(Model.Get(), &FFavroLinkModel::HandleForceRefresh, true))].
		AutoWidth();
	VerticalBox->AddSlot()[CacheButtons].HAlign(HAlign_Right).Padding(0.f, 0.f, 30.f, 0.f).AutoHeight();

	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
		[
			VerticalBox.ToSharedRef()
		]
	];
}

TSharedRef<SWidget> SFavroLinkSettings::MakeCategoryHeader(const FString& Label) const
{
	const FTableColumnHeaderStyle* HeaderStyle = &FAppStyle::Get().GetWidgetStyle<FTableColumnHeaderStyle>(
		"TableView.Header.Column");

	return SNew(SBox)
	[
		SNew(SBorder)
		.VAlign(VAlign_Top)
		.HAlign(HAlign_Left)
		.BorderImage(&HeaderStyle->NormalBrush)
		.Padding(FMargin(10, 7, 8, 8))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			  .VAlign(VAlign_Top)
			  .HAlign(HAlign_Left)
			  .AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString(*Label))
				.Font(FAppStyle::Get().GetFontStyle("DetailsView.CategoryFontStyle"))
				.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
			]
		]
	];
}

TSharedRef<SWidget> SFavroLinkSettings::MakePropertyRow(const TSharedPtr<ISinglePropertyView>& WidgetIn,
                                                        const bool bPasswordToggle) const
{
	const TSharedPtr<SHorizontalBox> HorizontalBox = SNew(SHorizontalBox);
	HorizontalBox->AddSlot()[WidgetIn.ToSharedRef()].Padding(0.f, 0.f, bPasswordToggle ? -20.f : 0.f, 0.f);

	if (bPasswordToggle)
	{
		const FOnCheckStateChanged OnCheckStateChangedDelegate = FOnCheckStateChanged::CreateRaw(
			this, &SFavroLinkSettings::SetTextFieldVisibility, WidgetIn->GetPropertyHandle(), WidgetIn);

		HorizontalBox->AddSlot()[SNew(SBorder)
			.BorderImage(FF2UEStyle::Get().GetBrush("Favro.RoundedBackgroundToggle"))
			[
				SNew(SCheckBox)
			   .Style(FAppStyle::Get(), "ToggleButtonCheckbox")
			   .OnCheckStateChanged(OnCheckStateChangedDelegate)
			   .IsChecked(this, &SFavroLinkSettings::IsTextFieldVisible, WidgetIn->GetPropertyHandle())
				[
					SNew(SImage)
					.Image_Lambda([this, WidgetIn]()
					            {
						            return IsTextFieldVisible(WidgetIn->GetPropertyHandle()) == ECheckBoxState::Checked
							                   ? FF2UEStyle::GetBrush("Favro.EyeOn")
							                   : FF2UEStyle::GetBrush("Favro.EyeOff");
					            })
					.ColorAndOpacity_Lambda([this, WidgetIn]()
					            {
						            return IsTextFieldVisible(WidgetIn->GetPropertyHandle()) == ECheckBoxState::Checked
							                   ? FStyleColors::Recessed
							                   : FSlateColor::UseForeground();
					            })
				]
			]
		].AutoWidth().Padding(FMargin(0.f, 0.f, 28.f, 0.f));
	}

	return SNew(SBox)
	[
		SNew(SBorder)
		.VAlign(VAlign_Top)
		.HAlign(HAlign_Fill)
		.BorderImage(FAppStyle::GetBrush("NoBorder"))
		.Padding(FMargin(10, 2, 8, 2))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			  .VAlign(VAlign_Center)
			  .HAlign(HAlign_Fill)
			  .FillWidth(0.5f)
			[
				SNew(STextBlock)
				.Font(FAppStyle::GetFontStyle("PropertyWindow.NormalFont"))
				.Text(WidgetIn->GetPropertyHandle()->GetPropertyDisplayName())
			]

			+ SHorizontalBox::Slot()
			  .VAlign(VAlign_Center)
			  .HAlign(HAlign_Fill)
			  .FillWidth(0.5f)
			[
				HorizontalBox.ToSharedRef()
			]
		]
	];
}

TSharedRef<SWidget> SFavroLinkSettings::MakeButton(const FString& ButtonLabel, FOnClicked OnClickedDelegate) const
{
	return SNew(SBox)
	[
		SNew(SBorder)
	.VAlign(VAlign_Top)
	.HAlign(HAlign_Right)
	.BorderImage(FAppStyle::GetBrush("NoBorder"))
	.Padding(6.f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			  .VAlign(VAlign_Top)
			  .HAlign(HAlign_Right)
			  .AutoWidth()
			[
				SNew(SButton)
				.Text(FText::FromString(ButtonLabel))
				.OnClicked(OnClickedDelegate)
			]
		]
	];
}

void SFavroLinkSettings::SetTextFieldVisibility(ECheckBoxState CheckBoxState,
                                                TSharedPtr<IPropertyHandle, ESPMode::ThreadSafe> PropertyHandle,
                                                TSharedPtr<ISinglePropertyView> WidgetIn) const
{
	if (PropertyHandle->HasMetaData("PasswordField"))
	{
		const bool bIsPassword = CheckBoxState == ECheckBoxState::Unchecked;
		const FString NewVisibility = bIsPassword ? "true" : "false";
		PropertyHandle->GetProperty()->SetMetaData("PasswordField", *NewVisibility);
		SetEditableTextPasswordAttribute(WidgetIn, bIsPassword);
	}
}

void SFavroLinkSettings::SetEditableTextPasswordAttribute(const TSharedPtr<SWidget>& Widget, bool bIsPassword) const
{
	if (!Widget.IsValid())
	{
		return;
	}

	TArray<TSharedPtr<SWidget>> WidgetsToVisit;
	WidgetsToVisit.Add(Widget);

	while (WidgetsToVisit.Num() > 0)
	{
		TSharedPtr<SWidget> CurrentWidget = WidgetsToVisit.Pop();

		if (!CurrentWidget.IsValid())
		{
			continue;
		}

		if (CurrentWidget->GetTypeAsString() == "SEditableTextBox")
		{
			const TSharedPtr<SEditableTextBox> EditableTextBox = StaticCastSharedPtr<SEditableTextBox>(CurrentWidget);
			EditableTextBox->SetIsPassword(bIsPassword);
		}
		else
		{
			FChildren* Children = CurrentWidget->GetChildren();
			for (int32 Index = 0; Index < Children->Num(); ++Index)
			{
				const TSharedPtr<SWidget>& Child = Children->GetChildAt(Index);
				WidgetsToVisit.Add(Child);
			}
		}
	}
}

ECheckBoxState SFavroLinkSettings::IsTextFieldVisible(
	TSharedPtr<IPropertyHandle, ESPMode::ThreadSafe> PropertyHandle) const
{
	if (PropertyHandle->HasMetaData("PasswordField"))
	{
		const ECheckBoxState CheckBoxState = PropertyHandle->GetBoolMetaData("PasswordField")
			                                     ? ECheckBoxState::Unchecked
			                                     : ECheckBoxState::Checked;
		return CheckBoxState;
	}

	return ECheckBoxState::Undetermined;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
