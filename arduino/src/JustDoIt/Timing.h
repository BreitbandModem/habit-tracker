#ifndef _TIMING_H_
#define _TIMING_H_

#include <Arduino.h>
#include <eztime.h>

class Timing {
    public:
        static bool syncTime();
        static bool isSynced();

        static String getDate();

        static void onInterval(int minutes, void(*function)());
        static void onNextDay(void (*function)());
        // void onQuietHour(int start, int end, void(*function)(bool));

    private:
        static Timezone tz;
        static const char MYISO8601[];

        static int intervalMinutes;
        static void(*intervalCallback)();
        static time_t nextInterval(int minutes);
        static void onIntervalScheduler();

        static void(*nextDayCallback)();
        static time_t nextDay();
        static void onNextDayScheduler();
};

#endif