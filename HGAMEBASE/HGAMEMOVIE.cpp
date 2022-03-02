#include <new>
#include <windows.h>
#include "HGAMEMOVIE.h"
#include "HGAMEVIDEO.h"
#include "HGAMEDEBUG.h"
#include "HGAMESTRING.h"

HGAMEMOVIE* HGAMEMOVIE::MainMoviePlay = nullptr;

HRESULT HGAMEMOVIE::Play(const Game_String& Path) 
{
    // 
    if (S_OK != MainMoviePlay->OpenFile(Path))
    {
        AMSG(L"무비파일 오픈에 실패했습니다.");
    }

    MainMoviePlay->MoviePlay();


    return S_OK;
}
HRESULT HGAMEMOVIE::Pause() 
{
    if (nullptr != MainMoviePlay)
    {
        MainMoviePlay->Pause();
    }
    else
    {
        AMSG(L"메인 무비 재생기가 존재하지 않습니다 윈도우를 만드세요");
    }

    return S_OK;
}
HRESULT HGAMEMOVIE::Stop() 
{
    if (nullptr != MainMoviePlay)
    {
        MainMoviePlay->MovieStop();
    }
    else 
    {
        AMSG(L"메인 무비 재생기가 존재하지 않습니다 윈도우를 만드세요");
    }

    return S_OK;
}

bool HGAMEMOVIE::IsPlay() 
{
    return MainMoviePlay->State() == PlaybackState::STATE_RUNNING;
}

//void HGAMEMOVIE::PrintMovie()
//{
//    MainMoviePlay->Repaint();
//}

HGAMEMOVIE::HGAMEMOVIE(HWND hwnd) :
    m_state(STATE_NO_GRAPH),
    m_hwnd(hwnd),
    m_pGraph(NULL),
    m_pControl(NULL),
    m_pEvent(NULL),
    m_pVideo(NULL)
{
    if (nullptr == MainMoviePlay)
    {
        MainMoviePlay = this;
    }

    m_DC = GetDC(m_hwnd);
}

HGAMEMOVIE::~HGAMEMOVIE()
{
    TearDownGraph();
}

// Open a media file for playback.
HRESULT HGAMEMOVIE::OpenFile(PCWSTR pszFileName)
{
    IBaseFilter* pSource = NULL;

    // Create a new filter graph. (This also closes the old one, if any.)
    HRESULT hr = InitializeGraph();
    if (FAILED(hr))
    {
        goto done;
    }

    // Add the source filter to the graph.
    hr = m_pGraph->AddSourceFilter(pszFileName, NULL, &pSource);
    if (FAILED(hr))
    {
        goto done;
    }

    // Try to render the streams.
    hr = RenderStreams(pSource);

done:
    if (FAILED(hr))
    {
        TearDownGraph();
    }
    SafeRelease(&pSource);
    return hr;
}


// Respond to a graph event.
//
// The owning window should call this method when it receives the window
// message that the application specified when it called SetEventWindow.
//
// Caution: Do not tear down the graph from inside the callback.

HRESULT HGAMEMOVIE::HandleGraphEvent(GraphEventFN pfnOnGraphEvent)
{
    if (!m_pEvent)
    {
        return E_UNEXPECTED;
    }

    long evCode = 0;
    LONG_PTR param1 = 0, param2 = 0;

    HRESULT hr = S_OK;

    // Get the events from the queue.
    while (SUCCEEDED(m_pEvent->GetEvent(&evCode, &param1, &param2, 0)))
    {
        // Invoke the callback.
        pfnOnGraphEvent(m_hwnd, evCode, param1, param2);

        // Free the event data.
        hr = m_pEvent->FreeEventParams(evCode, param1, param2);
        if (FAILED(hr))
        {
            break;
        }
    }
    return hr;
}

HRESULT HGAMEMOVIE::MoviePlay()
{
    if (m_state != STATE_PAUSED && m_state != STATE_STOPPED)
    {
        return VFW_E_WRONG_STATE;
    }

    HRESULT hr = m_pControl->Run();
    if (SUCCEEDED(hr))
    {
        m_state = STATE_RUNNING;
    }

    InvalidateRect(m_hwnd, nullptr, false);

    return hr;
}

HRESULT HGAMEMOVIE::MoviePause()
{
    if (m_state != STATE_RUNNING)
    {
        return VFW_E_WRONG_STATE;
    }

    HRESULT hr = m_pControl->Pause();
    if (SUCCEEDED(hr))
    {
        m_state = STATE_PAUSED;
    }



    return hr;
}

HRESULT HGAMEMOVIE::MovieStop()
{
    if (m_state != STATE_RUNNING && m_state != STATE_PAUSED)
    {
        return VFW_E_WRONG_STATE;
    }

    HRESULT hr = m_pControl->Stop();
    if (SUCCEEDED(hr))
    {
        m_state = STATE_STOPPED;
    }
    return hr;
}


// EVR/VMR functionality

BOOL HGAMEMOVIE::HasVideo() const
{
    return (m_pVideo && m_pVideo->HasVideo());
}

// Sets the destination rectangle for the video.

HRESULT HGAMEMOVIE::UpdateVideoWindow(const LPRECT prc)
{
    if (m_pVideo)
    {
        return m_pVideo->UpdateVideoWindow(m_hwnd, prc);
    }
    else
    {
        return S_OK;
    }
}

// Repaints the video. Call this method when the application receives WM_PAINT.

HRESULT HGAMEMOVIE::Repaint(HDC _DC)
{
    if (m_pVideo)
    {
        return m_pVideo->Repaint(m_hwnd, _DC);
    }
    else
    {
        return S_OK;
    }
}


// Notifies the video renderer that the display mode changed.
//
// Call this method when the application receives WM_DISPLAYCHANGE.

HRESULT HGAMEMOVIE::DisplayModeChanged()
{
    if (m_pVideo)
    {
        return m_pVideo->DisplayModeChanged();
    }
    else
    {
        return S_OK;
    }
}


// Graph building

// Create a new filter graph. 
HRESULT HGAMEMOVIE::InitializeGraph()
{
    TearDownGraph();

    // Create the Filter Graph Manager.
    HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL,
        CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pGraph));
    if (FAILED(hr))
    {
        goto done;
    }

    hr = m_pGraph->QueryInterface(IID_PPV_ARGS(&m_pControl));
    if (FAILED(hr))
    {
        goto done;
    }

    hr = m_pGraph->QueryInterface(IID_PPV_ARGS(&m_pEvent));
    if (FAILED(hr))
    {
        goto done;
    }

    // Set up event notification.
    hr = m_pEvent->SetNotifyWindow((OAHWND)m_hwnd, WM_GRAPH_EVENT, NULL);
    if (FAILED(hr))
    {
        goto done;
    }

    m_state = STATE_STOPPED;

done:
    return hr;
}

void HGAMEMOVIE::TearDownGraph()
{
    // Stop sending event messages
    if (m_pEvent)
    {
        m_pEvent->SetNotifyWindow((OAHWND)NULL, NULL, NULL);
    }

    SafeRelease(&m_pGraph);
    SafeRelease(&m_pControl);
    SafeRelease(&m_pEvent);

    delete m_pVideo;
    m_pVideo = NULL;

    m_state = STATE_NO_GRAPH;
}


HRESULT HGAMEMOVIE::CreateVideoRenderer()
{
    HRESULT hr = E_FAIL;

    enum { Try_EVR, Try_VMR9, Try_VMR7 };

    for (DWORD i = Try_EVR; i <= Try_VMR7; i++)
    {
        switch (i)
        {
        case Try_EVR:
            m_pVideo = new (std::nothrow) CEVR();
            break;

        case Try_VMR9:
            m_pVideo = new (std::nothrow) CVMR9();
            break;

        case Try_VMR7:
            m_pVideo = new (std::nothrow) CVMR7();
            break;
        }

        if (m_pVideo == NULL)
        {
            hr = E_OUTOFMEMORY;
            break;
        }

        hr = m_pVideo->AddToGraph(m_pGraph, m_hwnd);
        if (SUCCEEDED(hr))
        {
            break;
        }

        delete m_pVideo;
        m_pVideo = NULL;
    }
    return hr;
}


// Render the streams from a source filter. 

HRESULT HGAMEMOVIE::RenderStreams(IBaseFilter* pSource)
{
    BOOL bRenderedAnyPin = FALSE;

    IFilterGraph2* pGraph2 = NULL;
    IEnumPins* pEnum = NULL;
    IBaseFilter* pAudioRenderer = NULL;
    HRESULT hr = m_pGraph->QueryInterface(IID_PPV_ARGS(&pGraph2));
    if (FAILED(hr))
    {
        goto done;
    }

    // Add the video renderer to the graph
    hr = CreateVideoRenderer();
    if (FAILED(hr))
    {
        goto done;
    }

    // Add the DSound Renderer to the graph.
    hr = AddFilterByCLSID(m_pGraph, CLSID_DSoundRender,
        &pAudioRenderer, L"Audio Renderer");
    if (FAILED(hr))
    {
        goto done;
    }

    // Enumerate the pins on the source filter.
    hr = pSource->EnumPins(&pEnum);
    if (FAILED(hr))
    {
        goto done;
    }

    // Loop through all the pins
    IPin* pPin;
    while (S_OK == pEnum->Next(1, &pPin, NULL))
    {
        // Try to render this pin. 
        // It's OK if we fail some pins, if at least one pin renders.
        HRESULT hr2 = pGraph2->RenderEx(pPin, AM_RENDEREX_RENDERTOEXISTINGRENDERERS, NULL);

        pPin->Release();
        if (SUCCEEDED(hr2))
        {
            bRenderedAnyPin = TRUE;
        }
    }

    hr = m_pVideo->FinalizeGraph(m_pGraph);
    if (FAILED(hr))
    {
        goto done;
    }

    // Remove the audio renderer, if not used.
    BOOL bRemoved;
    hr = RemoveUnconnectedRenderer(m_pGraph, pAudioRenderer, &bRemoved);

done:
    SafeRelease(&pEnum);
    SafeRelease(&pAudioRenderer);
    SafeRelease(&pGraph2);

    // If we succeeded to this point, make sure we rendered at least one 
    // stream.
    if (SUCCEEDED(hr))
    {
        if (!bRenderedAnyPin)
        {
            hr = VFW_E_CANNOT_RENDER;
        }
    }
    return hr;
}