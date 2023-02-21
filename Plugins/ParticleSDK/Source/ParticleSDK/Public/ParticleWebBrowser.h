// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "IWebBrowserWindow.h"
#include "IWebBrowserPopupFeatures.h"
#include "IWebBrowserDialog.h"

#include "ParticleWebBrowser.generated.h"

/**
 *
 */
UCLASS()
class UParticleWebBrowser : public UWidget
{
	GENERATED_UCLASS_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUrlChanged, const FText&, Text);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBeforePopup, FString, URL, FString, Frame);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadCompleted);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadError);
	/**
	 * Load the specified URL
	 *
	 * @param NewURL New URL to load
	 */
	UFUNCTION(BlueprintCallable, Category = "Particle|Web Browser")
		void LoadURL(FString NewURL);

	/**
	 * Load a string as data to create a web page
	 *
	 * @param Contents String to load
	 * @param DummyURL Dummy URL for the page
	 */
	UFUNCTION(BlueprintCallable, Category = "Particle|Web Browser")
		void LoadString(FString Contents, FString DummyURL);

	/**
	* Executes a JavaScript string in the context of the web page
	*
	* @param ScriptText JavaScript string to execute
	*/
	UFUNCTION(BlueprintCallable, Category = "Particle|Web Browser")
		void ExecuteJavascript(const FString& ScriptText);

	/**
	 * Expose a UObject instance to the browser runtime.
	 * Properties and Functions will be accessible from JavaScript side.
	 * As all communication with the rendering procesis asynchronous, return values (both for properties and function results) are wrapped into JS Future objects.
	 *
	 * @param Name The name of the object. The object will show up as window.ue4.{Name} on the javascript side. If there is an existing object of the same name, this object will replace it. If bIsPermanent is false and there is an existing permanent binding, the permanent binding will be restored when the temporary one is removed.
	 * @param Object The object instance.
	 * @param bIsPermanent If true, the object will be visible to all pages loaded through this browser widget, otherwise, it will be deleted when navigating away from the current page. Non-permanent bindings should be registered from inside an OnLoadStarted event handler in order to be available before JS code starts loading.
	*/
	UFUNCTION(BlueprintCallable, Category = "Particle|Web Browser")
		void BindUObject(const FString& Name, UObject* Object, bool bIsPermanent);

	/**
	 * Remove an existing script binding registered by BindUObject.
	 *
	 * @param Name The name of the object to remove.
	 * @param Object The object will only be removed if it is the same object as the one passed in.
	 * @param bIsPermanent Must match the bIsPermanent argument passed to BindUObject.
	 */
	UFUNCTION(BlueprintCallable, Category = "Particle|Web Browser")
		void UnbindUObject(const FString& Name, UObject* Object, bool bIsPermanent);

	/**
	 * Get the current title of the web page
	 */
	UFUNCTION(BlueprintCallable, Category = "Particle|Web Browser")
		FText GetTitleText() const;

	/**
	* Gets the currently loaded URL.
	*
	* @return The URL, or empty string if no document is loaded.
	*/
	UFUNCTION(BlueprintCallable, Category = "Particle|Web Browser")
		FString GetUrl() const;

	/**
	 * Clear browser cache (cookies etc.).
	 *
	 */
	UFUNCTION(BlueprintCallable, Category = "Particle|Web Browser")
		void ClearCache() const;

	/**
	 * Return to previous page.
	 *
	 */
	UFUNCTION(BlueprintCallable, Category = "Particle|Web Browser")
		void GoBack() const;

	/**
	 * Go to next page.
	 *
	 */
	UFUNCTION(BlueprintCallable, Category = "Particle|Web Browser")
		void GoForward() const;

	UFUNCTION(BlueprintCallable, Category = "Particle|Web Browser")
		void ClosePopupBrowserWindow() const;

	/** Called when the Url changes. */
	UPROPERTY(BlueprintAssignable, Category = "Particle|Web Browser|Event")
		FOnUrlChanged OnUrlChanged;

	/** Called when a popup is about to spawn. */
	UPROPERTY(BlueprintAssignable, Category = "Particle|Web Browser|Event")
		FOnBeforePopup OnBeforePopup;

	/** Called when page loaded. */
	UPROPERTY(BlueprintAssignable, Category = "Particle|Web Browser|Event")
		FOnLoadCompleted OnLoadCompleted;

	/** Called when page load error. */
	UPROPERTY(BlueprintAssignable, Category = "Particle|Web Browser|Event")
	FOnLoadError OnLoadError;
	
public:

	//~ Begin UWidget interface
	virtual void SynchronizeProperties() override;
	// End UWidget interface

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

protected:
	/** URL that the browser will initially navigate to. The URL should include the protocol, eg http:// */
	UPROPERTY(EditAnywhere, Category = Appearance)
		FString InitialURL;

	/** Should the browser window support transparency. */
	UPROPERTY(EditAnywhere, Category = Appearance)
		bool bSupportsTransparency;

protected:
	TSharedPtr<class SWebBrowser> WebBrowserWidget;

protected:
	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	// End of UWidget interface

	void HandleOnUrlChanged(const FText& Text);
	bool HandleOnBeforePopup(FString URL, FString Frame);
	bool HandleOnCreateWindow(const TWeakPtr<IWebBrowserWindow>& NewBrowserWindow, const TWeakPtr<IWebBrowserPopupFeatures>& PopupFeatures);
	bool HandleOnCloseWindow(const TWeakPtr<IWebBrowserWindow>& BrowserWindowPtr);
	EWebBrowserDialogEventResponse HandleShowDialog(const TWeakPtr<IWebBrowserDialog>& DialogPtr);
	void HandleOnLoadCompleted();
	void HandleOnLoadError();

private:
	void AddWebOverlay(const TSharedRef< SWidget >& Content);
	void RemoveWebOverlay(const TSharedRef< SWidget >& Content);
private:
	bool bOpenDevTools;
	bool bJustTest;
	TMap<TWeakPtr<IWebBrowserWindow>, TWeakPtr<SWindow>> BrowserWindowWidgets;
	TSharedPtr<SOverlay> BrowserContainer;
	TMap<TWeakPtr<IWebBrowserWindow>, TWeakPtr<SWebBrowserView>> BrowserOverlayWidgets;

};
