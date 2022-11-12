#ifndef SAP_SMTC_H_
#define SAP_SMTC_H_

#include <functional>

#ifdef SAP_SMTC_BUILDING
#define EXPORT_DLL __declspec(dllexport)
#else
#define EXPORT_DLL __declspec(dllimport)
#endif

namespace SMTC
{
class WindowsMediaPlayerWrapper;

/*
Enum indicating the status of the playback.
*/
enum class PlaybackStatus
{
	UNKNOWN = -1,
	PLAYING,
	PAUSED,
	STOPPED
};

enum class PlaybackType
{
	UNKNOWN = -1,
	PLAY_PAUSE,
	PREVIOUS,
	NEXT
};

/*
This class is an interface arround the Windows Media Transport Control protocol.
It allow to display the title and artist name of the current track and to listen for
the media keys events.
*/
class EXPORT_DLL SAP_SMTC
{
	SAP_SMTC(const SAP_SMTC&) = delete;

public:
	SAP_SMTC();
	~SAP_SMTC();

	/*
	Notify the System Media Transport Control about the playback status change.
	*/
	void playbackStatusChanged(PlaybackStatus status);

	/*
	Notify the System Media Transport Control about the change of track.
	*/
	void trackChanged(const char* title, const char* artists);

	/*
	Since we do not have access to the Qt Signal system. We store the user callback for every type of playback (play, pause, previous, next).
	Then, the appropriate callback is then call every time the System Media Transport Control ask for it.
	*/
	void setPlaybackCallback(PlaybackType type, const std::function<void()>& callback);

private:
	WindowsMediaPlayerWrapper* m_SMTC;
};
}

#endif // SAP_SMTC_H_