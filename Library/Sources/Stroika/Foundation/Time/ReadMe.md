# Stroika::[Foundation](../ReadMe.md)::Time

This folder contains all the Stroika Library [Foundation](../ReadMe.md)::Time (time related) source code.

There is code for managing Dates, Time Durations, RealTime values (ticks), and timezones.

- [Common.h](Common.h)
- [Date.h](Date.h) - just a date - without time involved
- [DateTime.h](DateTime.h) - date and time combined
- [Duration.h](Duration.h) - chrono::duration\<> but without template bother, and with formatting/ISO-8601 suport
- [Realtime.h](Realtime.h) - DurationSecondsType, GetTickCount (), etc...
- [TimeOfDay.h](TimeOfDay.h) - intra-day time
- [Timezone.h](Timezone.h)
