#ifndef SBW_ANALYZER_SETTINGS
#define SBW_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

class SbwAnalyzerSettings : public AnalyzerSettings
{
public:
	SbwAnalyzerSettings();
	virtual ~SbwAnalyzerSettings();

	virtual bool SetSettingsFromInterfaces();
	virtual void LoadSettings( const char* settings );
	virtual const char* SaveSettings();

	void UpdateInterfacesFromSettings();
	
	Channel mTDIOChannel;
	Channel mTCKChannel;

protected:
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mTDIOChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mTCKChannelInterface;
};

#endif //SBW_ANALYZER_SETTINGS
