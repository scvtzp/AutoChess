#pragma once
#include <Windows.h>
#include <dshow.h>
#pragma comment(lib, "strmiids")

class CVideoRenderer;

enum PlaybackState
{
    STATE_NO_GRAPH,
    STATE_RUNNING,
    STATE_PAUSED,
    STATE_STOPPED,
};

const UINT WM_GRAPH_EVENT = WM_APP + 1;

typedef void (CALLBACK* GraphEventFN)(HWND hwnd, long eventCode, LONG_PTR param1, LONG_PTR param2);

class Game_String;
class HGAMEWINDOW;
class HGAMEMOVIE
{
public:
    friend HGAMEWINDOW;

    // 윈도우 하나당 한개만 재생할수 
    // 윈도우랑 일심 동체로 만들겠습니다.
private:
    static HGAMEMOVIE* MainMoviePlay;

public:
    static HRESULT Play(const Game_String& Path);
    static HRESULT Pause();
    static HRESULT Stop();
    // static void PrintMovie();
    static bool IsPlay();

public:
    HGAMEMOVIE(HWND hwnd);
    ~HGAMEMOVIE();

    PlaybackState State() const { 
        return m_state; 
    }

    HRESULT OpenFile(PCWSTR pszFileName);

    HRESULT MoviePlay();
    HRESULT MoviePause();
    HRESULT MovieStop();

    BOOL    HasVideo() const;
    HRESULT UpdateVideoWindow(const LPRECT prc);
    HRESULT Repaint(HDC _DC);
    HRESULT DisplayModeChanged();

    HRESULT HandleGraphEvent(GraphEventFN pfnOnGraphEvent);

private:
    HRESULT InitializeGraph();
    void    TearDownGraph();
    HRESULT CreateVideoRenderer();
    HRESULT RenderStreams(IBaseFilter* pSource);

    PlaybackState   m_state;

    HWND m_hwnd; // Video window. This window also receives graph events.
    HDC m_DC; // Video window. This window also receives graph events.

    IGraphBuilder* m_pGraph;
    IMediaControl* m_pControl;
    IMediaEventEx* m_pEvent;
    CVideoRenderer* m_pVideo;
};