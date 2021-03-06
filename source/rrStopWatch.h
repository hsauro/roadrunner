#ifndef rrStopWatchH
#define rrStopWatchH
//---------------------------------------------------------------------------
#include "rrExporter.h"

namespace rr
{

/**
 * @internal
 */
class RR_DECLSPEC StopWatch
{
    private:
        int                     mStartTime;            //Ticks...
        int                     mTotalTime;
        bool                 mIsRunning;
        int                    GetMilliSecondCount();
        int                    GetMilliSecondSpan();

    public:
                            StopWatch();
                           ~StopWatch();
        int                 Start();
        int                 Stop();
        double                 GetTime();
};
}

#endif
