#include "BrowserComponent.h"

BrowserComponent::BrowserComponent() :
  juce::WebBrowserComponent (
    [] {
      juce::File appOrDLLPath = juce::File::getSpecialLocation (juce::File::currentApplicationFile);
      juce::File parentFolderToAppOrDLL = appOrDLLPath.getParentDirectory();
      juce::File webView2LoaderFile = parentFolderToAppOrDLL.getChildFile ("WebView2Loader.dll");

      auto userDataFolder = juce::File::getSpecialLocation (juce::File::currentApplicationFile).getParentDirectory().getChildFile ("userDataFolder");

      juce::WebBrowserComponent::Options options;

      return options
        .withKeepPageLoadedWhenBrowserIsHidden()
        .withBackend (
          juce::WebBrowserComponent::Options::Backend::webview2)
        .withUserAgent (
          "...")
        .withWinWebView2Options (
          juce::WebBrowserComponent::Options::WinWebView2()
            .withDLLLocation (webView2LoaderFile)
            .withUserDataFolder (userDataFolder)
            .withBuiltInErrorPageDisabled()
            .withBackgroundColour (juce::Colours::white)
            .withStatusBarDisabled());
    }())
{
  loadUI();
}

BrowserComponent::BrowserComponent (juce::String initialUrl) :
  BrowserComponent::BrowserComponent()
{
  goToURL (initialUrl);
}

void BrowserComponent::loadUI()
{
#if JUCE_MAC || JUCE_IOS
  NSString* devServerIpNsString = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"DevServerIP"];
  // TODO [NSBundle mainBundle] refers to the plugin host when running as AudioUnit
  // so can't access plist - hardcoding for now
  auto devServerIp = devServerIpNsString == nil ? "127.0.0.1" : [devServerIpNsString UTF8String];
#else
  auto devServerIp = "127.0.0.1";
#endif

#if BROWSER_INTEGRATION_USE_DEV_SERVER_IN_DEBUG && JUCE_DEBUG
  auto url = juce::String ("http://") + devServerIp + ":3000";
#else
  auto url = "file://" + juce::File::getSpecialLocation (juce::File::SpecialLocationType::currentApplicationFile).getParentDirectory().getChildFile ("js").getChildFile ("index.html").getFullPathName();
#endif

  DBG ("Loading URL: " + url);
  goToURL (url);
}

void BrowserComponent::sendMessage (const juce::var message, bool suppressLog)
{
  const auto jsonMessage = juce::JSON::toString (message, true);
  const auto url = "javascript:" + jsCallbackName + "(" + jsonMessage + ")";

  if (!suppressLog)
    DBG ("sendMessage: " << jsonMessage);

  goToURL (url);
}

bool BrowserComponent::pageAboutToLoad (const juce::String& newURL)
{
  if (newURL.startsWith (urlSchema))
  {
    if (onMessageCallback)
    {
      auto messageString = juce::URL::removeEscapeChars (newURL.substring (urlSchema.length()));
      auto message = juce::JSON::parse (messageString);

      DBG ("message: " << messageString);

      onMessageCallback (message);
    }
    else
    {
      DBG ("No onMessageCallback defined in BrowserComponent");
      jassertfalse;
    }

    return false;
  }
  else
    return true;
}

void BrowserComponent::setOnMessageCallback (std::function<void (juce::var)> cb)
{
  onMessageCallback = cb;
}

void BrowserComponent::scriptMessageReceived (const juce::var messageBody)
{
  onMessageCallback (messageBody);
}
