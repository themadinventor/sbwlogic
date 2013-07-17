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
	
	Channel mTCKChannel;
	Channel mTMSChannel;
	Channel mTDIChannel;
	Channel mTDOChannel;
	Channel mTRSTChannel;

    AnalyzerEnums::ShiftOrder mShiftOrder;

protected:
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mTCKChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mTMSChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mTDIChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mTDOChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mTRSTChannelInterface;
    std::auto_ptr< AnalyzerSettingInterfaceNumberList > mShiftOrderInterface;
};

#endif //SBW_ANALYZER_SETTINGS
