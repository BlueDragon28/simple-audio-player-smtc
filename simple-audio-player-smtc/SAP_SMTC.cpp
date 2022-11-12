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

	void setPlaybackCallback(PlaybackType type, const std::function<void()>& callback)
	{
		// Check what type it is and store the callback to the appropriate member variable.
		switch (type)
		{
		case PlaybackType::PLAY_PAUSE:
		{
			m_playPauseCallback = callback;
		} break;

		case PlaybackType::PREVIOUS:
		{
			m_previousCallback = callback;
		} break;

		case PlaybackType::NEXT:
		{
			m_nextCallback = callback;
		} break;

		case PlaybackType::UNKNOWN:
		default:
		{
			return;
		} break;
		}
	}

private:
	/*
	Called everytime there is a media commend event (like play, pause...).
	*/
	void handleMediaCommand(SystemMediaTransportControls sender, SystemMediaTransportControlsButtonPressedEventArgs args)
	{
		// Watch what media key has been cliked and call the corresponding callback.
		switch (args.Button())
		{
		case SystemMediaTransportControlsButton::Play:
		case SystemMediaTransportControlsButton::Pause:
		{
			try
			{
				m_playPauseCallback();
			}
			catch (const std::bad_function_call&)
			{
				return;
			}
		} break;

		case SystemMediaTransportControlsButton::Previous:
		{
			try
			{
				m_previousCallback();
			}
			catch (const std::bad_function_call&)
			{
				return;
			}
		} break;

		case SystemMediaTransportControlsButton::Next:
		{
			try
			{
				m_nextCallback();
			}
			catch (const std::bad_function_call&)
			{
				return;
			}
		} break;

		default:
		{
			return;
		} break;
		}
	}

	inline SystemMediaTransportControls SMTC()
	{
		return m_mediaPlayer.SystemMediaTransportControls();
	}

	// The callback to call on media keys event.
	std::function<void()>
		m_playPauseCallback,
		m_previousCallback,
		m_nextCallback;

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

void SAP_SMTC::setPlaybackCallback(PlaybackType type, const std::function<void()>& callback)
{
	m_SMTC->setPlaybackCallback(type, callback);
}
}