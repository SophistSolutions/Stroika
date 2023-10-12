# Stroika::[Foundation](../)::Time

This folder contains all the Stroika Library [Foundation](../)::Time (time related) source code.

There is code for managing Dates, Time Durations, RealTime values (ticks), and timezones.

- [Common.h](Common.h)
- [Date.h](Date.h) - just a date - without time involved
- [DateTime.h](DateTime.h) - date and time combined
- [Duration.h](Duration.h) - chrono::duration\<> but without template bother, and with formatting/ISO-8601 suport
- [Realtime.h](Realtime.h) - DurationSecondsType, GetTickCount (), etc...
- [TimeOfDay.h](TimeOfDay.h) - intra-day time
- [Timezone.h](Timezone.h)

## Things considered and intentionally omitted

- WhenType
  - Fuzzy version of DateTime – from conversation with sterl from 2023-10-02 – gedCom – ‘before 1890’
	Datevalue = date/dateperiod/daterange/dateApprox. Dateperiod is from and to – daterange
  - A when-type might be useful, but hard to capture the very application-specific notions of fuzzyness.
    Depending on this notion, that impacts what things you can say about and do with a WhenType. For now
    I think best to leave this as application-layer code.
