
#include "ParticleWebBrowser.h"
#include "SWebBrowser.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Async/TaskGraphInterfaces.h"
#include "UObject/ConstructorHelpers.h"
#include "IWebBrowserCookieManager.h"
#include "IWebBrowserSingleton.h"
#include "WebBrowserModule.h"

#if WITH_EDITOR
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialExpressionMaterialFunctionCall.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "Materials/MaterialExpressionTextureSampleParameter2D.h"
#include "Materials/MaterialFunction.h"
#include "Factories/MaterialFactoryNew.h"
#include "AssetRegistryModule.h"
#include "PackageHelperFunctions.h"
#endif

#define LOCTEXT_NAMESPACE "ParticleWebBrowser"

////////////////////////////////////////////////////
// UParticleWebBrowser

UParticleWebBrowser::UParticleWebBrowser(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsVariable = true;
	bOpenDevTools = false;
	bJustTest = false;
}

void UParticleWebBrowser::LoadURL(FString NewURL)
{
	if (WebBrowserWidget.IsValid())
	{
		return WebBrowserWidget->LoadURL(NewURL);
	}
}

void UParticleWebBrowser::LoadString(FString Contents, FString DummyURL)
{
	if (WebBrowserWidget.IsValid())
	{
		return WebBrowserWidget->LoadString(Contents, DummyURL);
	}
}

void UParticleWebBrowser::ExecuteJavascript(const FString& ScriptText)
{
	if (WebBrowserWidget.IsValid())
	{
		return WebBrowserWidget->ExecuteJavascript(ScriptText);
	}
}

void UParticleWebBrowser::BindUObject(const FString& Name, UObject* Object, bool bIsPermanent)
{
	if (WebBrowserWidget.IsValid())
	{
		return WebBrowserWidget->BindUObject(Name, Object, bIsPermanent);
	}
}
void UParticleWebBrowser::UnbindUObject(const FString& Name, UObject* Object, bool bIsPermanent)
{
	if (WebBrowserWidget.IsValid())
	{
		return WebBrowserWidget->UnbindUObject(Name, Object, bIsPermanent);
	}
}

FText UParticleWebBrowser::GetTitleText() const
{
	if (WebBrowserWidget.IsValid())
	{
		return WebBrowserWidget->GetTitleText();
	}

	return FText::GetEmpty();
}

FString UParticleWebBrowser::GetUrl() const
{
	if (WebBrowserWidget.IsValid())
	{
		return WebBrowserWidget->GetUrl();
	}

	return FString();
}

void UParticleWebBrowser::ClearCache() const
{
	IWebBrowserSingleton* WebBrowserSingleton = IWebBrowserModule::Get().GetSingleton();
	if (WebBrowserSingleton)
	{
		TSharedPtr<IWebBrowserCookieManager> CookieManager = WebBrowserSingleton->GetCookieManager();
		if (CookieManager.IsValid())
		{
			CookieManager->DeleteCookies();
		}
	}
}

void UParticleWebBrowser::GoBack() const
{
	if (WebBrowserWidget.IsValid() && WebBrowserWidget->CanGoBack())
	{
		return WebBrowserWidget->GoBack();
	}
}

void UParticleWebBrowser::GoForward() const
{
	if (WebBrowserWidget.IsValid() && WebBrowserWidget->CanGoForward())
	{
		return WebBrowserWidget->GoForward();
	}
}

void UParticleWebBrowser::ClosePopupBrowserWindow() const
{
	for (auto& Elem : BrowserWindowWidgets)
	{
		TSharedRef<SWindow> InWindowToDestroy = Elem.Value.Pin().ToSharedRef();

		TSharedPtr<IWebBrowserWindow> BrowserWindow = Elem.Key.Pin();
		if (BrowserWindow.IsValid())
		{
			if (BrowserWindow->IsClosing())
			{
				FSlateApplicationBase::Get().RequestDestroyWindow(InWindowToDestroy);
			}
			else
			{
				// Notify the browser window that we would like to close it.  On the CEF side, this will 
				//  result in a call to FWebBrowserHandler::DoClose only if the JavaScript onbeforeunload
				//  event handler allows it.
				BrowserWindow->CloseBrowser(false);
			}
		}
		//UE_LOG(LogParticleWebBrowser, Log, TEXT("HandleBrowserCreateWindow"));
	}
}

void UParticleWebBrowser::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	WebBrowserWidget.Reset();
}

TSharedRef<SWidget> UParticleWebBrowser::RebuildWidget()
{
	if (IsDesignTime())
	{
		return SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("Qumeta Web Browser", "Qumeta Web Browser"))
			];
	}
	else
	{
		WebBrowserWidget = SNew(SWebBrowser)
			.InitialURL(InitialURL)
			.ShowControls(false)
			.SupportsTransparency(bSupportsTransparency)
			// default frame 24
			//.BrowserFrameRate(60)
			.OnUrlChanged(BIND_UOBJECT_DELEGATE(FOnTextChanged, HandleOnUrlChanged))
			.OnBeforePopup(BIND_UOBJECT_DELEGATE(FOnBeforePopupDelegate, HandleOnBeforePopup))
			.OnCreateWindow(BIND_UOBJECT_DELEGATE(FOnCreateWindowDelegate, HandleOnCreateWindow))
			.OnLoadCompleted(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnLoadCompleted))
			.OnLoadError(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnLoadError));

		return WebBrowserWidget.ToSharedRef();
	}
}

void UParticleWebBrowser::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (WebBrowserWidget.IsValid())
	{

	}
}

void UParticleWebBrowser::HandleOnUrlChanged(const FText& InText)
{
	OnUrlChanged.Broadcast(InText);
}

void UParticleWebBrowser::HandleOnLoadCompleted()
{
	//FString Script = GetInitializeScript();
	//ExecuteJavascript(Script);

	OnLoadCompleted.Broadcast();
}

void UParticleWebBrowser::HandleOnLoadError()
{
	OnLoadError.Broadcast();
}

bool UParticleWebBrowser::HandleOnBeforePopup(FString URL, FString Frame)
{
	// Chrome debug tools are allowed to open a popup window.
	if (URL.Contains(TEXT("chrome-devtools")))
	{
		bOpenDevTools = true;
		return false;
	}

	//UP_WALLET_POP
	if (Frame.Contains(TEXT("UP_WALLET_POP")))
	{
		bJustTest = true;
		return false;
	}

	if (OnBeforePopup.IsBound())
	{
		if (IsInGameThread())
		{
			OnBeforePopup.Broadcast(URL, Frame);
		}
		else
		{
			// Retry on the GameThread.
			TWeakObjectPtr<UParticleWebBrowser> WeakThis = this;
			FFunctionGraphTask::CreateAndDispatchWhenReady([WeakThis, URL, Frame]()
				{
					if (WeakThis.IsValid())
					{
						WeakThis->HandleOnBeforePopup(URL, Frame);
					}
				}, TStatId(), nullptr, ENamedThreads::GameThread);
		}

		return true;
	}

	return false;
}

bool UParticleWebBrowser::HandleOnCreateWindow(const TWeakPtr<IWebBrowserWindow>& NewBrowserWindow, const TWeakPtr<IWebBrowserPopupFeatures>& PopupFeatures)
{
	//UE_LOG(LogQumetaSdk, Log, TEXT("HandleBrowserCreateWindow"));

	TSharedPtr<IWebBrowserPopupFeatures> PopupFeaturesSP = PopupFeatures.Pin();
	check(PopupFeatures.IsValid())

		// All allowed popups, with the exception of the dev tools, are spawned as an overlay on top of the login flow browser.
		if (bOpenDevTools || bJustTest)
		{
			if (bOpenDevTools)
				bOpenDevTools = false;

			if (bJustTest)
				bJustTest = false;

			// Dev tools spawn in a new window.
			TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindWidgetWindow(WebBrowserWidget.ToSharedRef());
			if (ParentWindow.IsValid())
			{
				const int PosX = PopupFeaturesSP->IsXSet() ? PopupFeaturesSP->GetX() : 100;
				const int PosY = PopupFeaturesSP->IsYSet() ? PopupFeaturesSP->GetY() : 100;
				const FVector2D BrowserWindowPosition(PosX, PosY);

				const int Width = PopupFeaturesSP->IsWidthSet() ? PopupFeaturesSP->GetWidth() : 800;
				const int Height = PopupFeaturesSP->IsHeightSet() ? PopupFeaturesSP->GetHeight() : 600;
				const FVector2D BrowserWindowSize(Width, Height);

				const ESizingRule SizeingRule = PopupFeaturesSP->IsResizable() ? ESizingRule::UserSized : ESizingRule::FixedSize;

				TSharedPtr<IWebBrowserWindow> NewBrowserWindowSP = NewBrowserWindow.Pin();
				check(NewBrowserWindowSP.IsValid());

				TSharedRef<SWindow> BrowserWindowWidget =
					SNew(SWindow)
					.Title(FText::GetEmpty())
					.ClientSize(BrowserWindowSize)
					.ScreenPosition(BrowserWindowPosition)
					.AutoCenter(EAutoCenter::None)
					.SizingRule(SizeingRule)
					.SupportsMaximize(SizeingRule != ESizingRule::FixedSize)
					.SupportsMinimize(SizeingRule != ESizingRule::FixedSize)
					.HasCloseButton(true)
					.CreateTitleBar(true)
					.IsInitiallyMaximized(PopupFeaturesSP->IsFullscreen())
					.LayoutBorder(FMargin(0));
				// Setup browser widget.
				TSharedPtr<SWebBrowser> BrowserWidget;
				BrowserWindowWidget->SetContent(
					SNew(SBorder)
					.VAlign(VAlign_Fill)
					.HAlign(HAlign_Fill)
					.Padding(0)
					[
						SAssignNew(BrowserWidget, SWebBrowser, NewBrowserWindowSP)
						.ShowControls(PopupFeaturesSP->IsToolBarVisible())
					.ShowAddressBar(PopupFeaturesSP->IsLocationBarVisible())
					.OnCreateWindow(BIND_UOBJECT_DELEGATE(FOnCreateWindowDelegate, HandleOnCreateWindow))
					.OnCloseWindow(BIND_UOBJECT_DELEGATE(FOnCloseWindowDelegate, HandleOnCloseWindow))
					//.OnShowDialog(this, &UParticleWebBrowser::HandleShowDialog)
					]);

				// Setup some OnClose stuff.
				{
					struct FLocal
					{
						static void RequestDestroyWindowOverride(const TSharedRef<SWindow>& Window, TWeakPtr<IWebBrowserWindow> BrowserWindowPtr)
						{
							TSharedPtr<IWebBrowserWindow> BrowserWindow = BrowserWindowPtr.Pin();
							if (BrowserWindow.IsValid())
							{
								if (BrowserWindow->IsClosing())
								{
									FSlateApplicationBase::Get().RequestDestroyWindow(Window);
								}
								else
								{
									// Notify the browser window that we would like to close it.  On the CEF side, this will 
									//  result in a call to FWebBrowserHandler::DoClose only if the JavaScript onbeforeunload
									//  event handler allows it.
									BrowserWindow->CloseBrowser(false);
								}
							}
						}
					};

					BrowserWindowWidget->SetRequestDestroyWindowOverride(FRequestDestroyWindowOverride::CreateStatic(&FLocal::RequestDestroyWindowOverride, TWeakPtr<IWebBrowserWindow>(NewBrowserWindow)));
				}

				FSlateApplication::Get().AddWindow(BrowserWindowWidget);
				BrowserWindowWidget->BringToFront();
				FSlateApplication::Get().SetKeyboardFocus(BrowserWidget, EFocusCause::SetDirectly);

				BrowserWindowWidgets.Add(NewBrowserWindow, BrowserWindowWidget);
				return true;
			}
			else
			{
				TSharedPtr<IWebBrowserWindow> NewBrowserWindowSP = NewBrowserWindow.Pin();
				check(NewBrowserWindowSP.IsValid());

				TSharedRef<SWebBrowserView> NewBrowserToOverlay =
					SNew(SWebBrowserView, NewBrowserWindowSP)
					.ShowErrorMessage(false)
					.SupportsTransparency(true)
					//.OnLoadError(this, &SLoginFlowImpl::HandleOverlayBrowserLoadError, NewBrowserWindow)
					.OnBeforePopup(BIND_UOBJECT_DELEGATE(FOnBeforePopupDelegate, HandleOnBeforePopup))
					.OnCreateWindow(BIND_UOBJECT_DELEGATE(FOnCreateWindowDelegate, HandleOnCreateWindow))
					.OnCloseWindow(BIND_UOBJECT_DELEGATE(FOnCloseWindowDelegate, HandleOnCloseWindow));
				//.OnBeforeNavigation(this, &SLoginFlowImpl::HandleBrowserBeforeBrowse)
				//.OnShowDialog(this, &SLoginFlowImpl::HandleShowDialog);

			//AddWebOverlay(NewBrowserToOverlay);
			//BrowserOverlayWidgets.Add(NewBrowserWindow, NewBrowserToOverlay);

				return true;
			}
		}
		else
		{
			TSharedPtr<IWebBrowserWindow> NewBrowserWindowSP = NewBrowserWindow.Pin();
			check(NewBrowserWindowSP.IsValid());

			TSharedRef<SWebBrowserView> NewBrowserToOverlay =
				SNew(SWebBrowserView, NewBrowserWindowSP)
				.ShowErrorMessage(false)
				.SupportsTransparency(true)
				//.OnLoadError(this, &SLoginFlowImpl::HandleOverlayBrowserLoadError, NewBrowserWindow)
				.OnBeforePopup(BIND_UOBJECT_DELEGATE(FOnBeforePopupDelegate, HandleOnBeforePopup))
				.OnCreateWindow(BIND_UOBJECT_DELEGATE(FOnCreateWindowDelegate, HandleOnCreateWindow))
				.OnCloseWindow(BIND_UOBJECT_DELEGATE(FOnCloseWindowDelegate, HandleOnCloseWindow));
			//.OnBeforeNavigation(this, &SLoginFlowImpl::HandleBrowserBeforeBrowse)
			//.OnShowDialog(this, &SLoginFlowImpl::HandleShowDialog);

			AddWebOverlay(NewBrowserToOverlay);
			BrowserOverlayWidgets.Add(NewBrowserWindow, NewBrowserToOverlay);

			return true;
		}

	return true;
}

void UParticleWebBrowser::AddWebOverlay(const TSharedRef< SWidget >& Content)
{
	if (BrowserContainer.IsValid())
	{
		BrowserContainer->AddSlot()
			.Padding(35.0f)
			[
				Content
			];
	}
}

void UParticleWebBrowser::RemoveWebOverlay(const TSharedRef< SWidget >& Content)
{
	if (BrowserContainer->GetNumWidgets() > 1)
	{
		BrowserContainer->RemoveSlot(Content);

	}
}

bool UParticleWebBrowser::HandleOnCloseWindow(const TWeakPtr<IWebBrowserWindow>& BrowserWindowPtr)
{
	//UE_LOG(LogQumetaSdk, Log, TEXT("HandleBrowserCloseWindow"));

	TSharedPtr<IWebBrowserWindow> BrowserWindow = BrowserWindowPtr.Pin();
	if (BrowserWindow.IsValid())
	{
		if (!BrowserWindow->IsClosing())
		{
			// If the browser is not set to close, we tell the browser to close which will call back into this handler function.
			BrowserWindow->CloseBrowser(false);
		}
		else
		{
			// Close any matching overlay
			const TWeakPtr<SWebBrowserView>* FoundBrowserViewWidgetPtr = BrowserOverlayWidgets.Find(BrowserWindow);
			if (FoundBrowserViewWidgetPtr != nullptr)
			{
				TSharedPtr<SWebBrowserView> FoundBrowserViewWidget = FoundBrowserViewWidgetPtr->Pin();
				if (FoundBrowserViewWidget.IsValid())
				{
					RemoveWebOverlay(FoundBrowserViewWidget.ToSharedRef());
				}
				BrowserOverlayWidgets.Remove(BrowserWindow);
				return true;
			}

			// Close any matching window
			const TWeakPtr<SWindow>* FoundBrowserWindowWidgetPtr = BrowserWindowWidgets.Find(BrowserWindow);
			if (FoundBrowserWindowWidgetPtr != nullptr)
			{
				TSharedPtr<SWindow> FoundBrowserWindowWidget = FoundBrowserWindowWidgetPtr->Pin();
				if (FoundBrowserWindowWidget.IsValid())
				{
					FoundBrowserWindowWidget->RequestDestroyWindow();
				}
				BrowserWindowWidgets.Remove(BrowserWindow);
				return true;
			}
		}
	}

	return false;
}

EWebBrowserDialogEventResponse UParticleWebBrowser::HandleShowDialog(const TWeakPtr<IWebBrowserDialog>& DialogPtr)
{
	// @todo: Due to an OS X crash, we continue all dialogs with the default action to prevent them from displaying using native windows.  In the future, we should add custom handlers/ui for these dialogs.
	EWebBrowserDialogEventResponse WebDialogHandling = EWebBrowserDialogEventResponse::Continue;
	return WebDialogHandling;
}

#if WITH_EDITOR

const FText UParticleWebBrowser::GetPaletteCategory()
{
	return LOCTEXT("Qumeta", "Qumeta");
}

#endif

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
