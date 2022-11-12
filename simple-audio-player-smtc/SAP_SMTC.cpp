#include "SAP_SMTC.h"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.h>
#include <winrt/Windows.Media.Playback.h>

using namespace winrt::Windows::Media;

namespace SMTC
{
/*
This class is a wrapper arround the Windows Media Player object.
*/
class WindowsMediaPlayerWrapper
{
	WindowsMediaPlayerWrapper(const WindowsMediaPlayerWrapper&) = delete;

public:
	WindowsMediaPlayerWrapper() :
		m_mediaPlayer(nullptr)
	{
		winrt::init_apartment(winrt::apartment_type::single_threaded); // Initialize Windows Runtime.

		// Initialize the media player.
		m_mediaPlayer = Playback::MediaPlayer();
		m_mediaPlayer.CommandManager().IsEnabled(false);

		// Handle media keys event.
		SMTC().ButtonPressed(
			[this](SystemMediaTransportControls sender, SystemMediaTransportControlsButtonPressedEventArgs args) -> void
			{
				this->handleMediaCommand(sender, args);
			});

		// Enable playback control.
		SMTC().IsPlayEnabled(true);
		SMTC().IsPauseEnabled(true);
		SMTC().IsStopEnabled(false); // Not implemented yet.
		SMTC().IsPreviousEnabled(true);
		SMTC().IsNextEnabled(true);

		SMTC().PlaybackStatus(MediaPlaybackStatus::Closed);
		SMTC().IsEnabled(true);

		SMTC().DisplayUpdater().Type(MediaPlaybackType::Music);
		SMTC().DisplayUpdater().Update();
	}

	~WindowsMediaPlayerWrapper()
	{
		// Deinit media player and Windows Runtime.
		m_mediaPlayer = Playback::MediaPlayer(nullptr);
		winrt::uninit_apartment();
	}

	void updatePlaybackStatus(PlaybackStatus status)
	{
		MediaPlaybackStatus SMTCStatus;

		// Convert PlaybackStatus into MediaPlaybackStatus.
		switch (status)
		{
		case PlaybackStatus::PLAYING:
		{
			SMTCStatus = MediaPlaybackStatus::Playing;
		} break;
		case PlaybackStatus::PAUSED:
		{
			SMTCStatus = MediaPlaybackStatus::Paused;
		} break;
		case PlaybackStatus::UNKNOWN:
		default:
		case PlaybackStatus::STOPPED:
		{
			SMTCStatus = MediaPlaybackStatus::Stopped;
		} break;
		}

		//// Notify System Media Transport Control about the playback change.
		SMTC().PlaybackStatus(SMTCStatus);
		SMTC().DisplayUpdater().Update();
	}

	void updateMediaInfo(const char* title, const char* artists)
	{
		winrt::hstring hTitle, hArtists;

		// Convert title and artists into Windows Runtime string.
		if (title)
		{
			hTitle = winrt::to_hstring(title);
		}
		else
		{
			hTitle = winrt::to_hstring(u8"Unknown Title");
		}

		if (artists)
		{
			hArtists = winrt::to_hstring(artists);
		}
		else
		{
			hArtists = winrt::to_hstring(u8"Unknown Artists");
		}

		// Notify System Media Transport Control about the track change.
		SMTC().DisplayUpdater().MusicProperties().Title(hTitle);
		SMTC().DisplayUpdater().MusicProperties().Artist(hArtists);
		SMTC().DisplayUpdater().Update();
	}

private:
	/*
	Called everytime there is a media commend event (like play, pause...).
	*/
	void handleMediaCommand(SystemMediaTransportControls sender, SystemMediaTransportControlsButtonPressedEventArgs args)
	{

	}

	inline SystemMediaTransportControls SMTC()
	{
		return m_mediaPlayer.SystemMediaTransportControls();
	}

	Playback::MediaPlayer m_mediaPlayer;
};

/*
SAP_SMTC method implementations.
*/
SAP_SMTC::SAP_SMTC() :
	m_SMTC(new WindowsMediaPlayerWrapper())
{}

SAP_SMTC::~SAP_SMTC()
{
	if (m_SMTC)
	{
		delete m_SMTC;
	}
}

void SAP_SMTC::playbackStatusChanged(PlaybackStatus status)
{
	m_SMTC->updatePlaybackStatus(status);
}

void SAP_SMTC::trackChanged(const char* title, const char* artists)
{
	m_SMTC->updateMediaInfo(title, artists);
}
}