#include <stddef.h>
#include <stdlib.h>

int timeStampToInt(const char *timeStamp);

void addTimerToUpcoming(prayerObject *Prayer, int timeStampInt);

void addTimerToUpcoming(prayerObject *Prayer, int timeStampInt)
{
  int prayersTimeStampInts[] = {
    timeStampToInt(Prayer->Lastthird),
    timeStampToInt(Prayer->Fajr),
    timeStampToInt(Prayer->Sunrise),
    timeStampToInt(Prayer->Dhuhr),
    timeStampToInt(Prayer->Asr),
    timeStampToInt(Prayer->Maghrib),
    timeStampToInt(Prayer->Isha),
  };

  int upcomingPrayerIndex = 0;

  for (size_t i = 0; i < 7; ++i)
    if (prayersTimeStampInts[i] == -1)
    {
      Prayer->timeToUpcoming = -1;
      Prayer->upcoming = NULL;
      return;
    }

  if (timeStampInt > prayersTimeStampInts[6])
  {
    upcomingPrayerIndex = 0;
    Prayer->timeToUpcoming = (1440 - timeStampInt) + prayersTimeStampInts[0];
  }

  else
  {
    for (size_t i = 0; i < 7; ++i)
    {
      if (timeStampInt <= prayersTimeStampInts[i])
      {
        upcomingPrayerIndex = i;
        break;
      }
    }

    Prayer->timeToUpcoming = prayersTimeStampInts[upcomingPrayerIndex] - timeStampInt;
  }


  switch (upcomingPrayerIndex) {
    case 0: Prayer->upcoming = "Lastthird"; break;
    case 1: Prayer->upcoming = "Fajr";      break;
    case 2: Prayer->upcoming = "Sunrise";   break;
    case 3: Prayer->upcoming = "Dhuhr";     break;
    case 4: Prayer->upcoming = "Asr";       break;
    case 5: Prayer->upcoming = "Maghrib";   break;
    case 6: Prayer->upcoming = "Isha";      break;
  }

  return;
}


int timeStampToInt(const char *timeStamp)
{
  char *endPtr = NULL;

  //HH:MM
  //^ ^
  //t endPtr
  int hours = (int) strtol(timeStamp, &endPtr, 10); //10 for the base

  if (endPtr - timeStamp != 2)
    return -1;

  //HH:MM
  //   ^  ^
  //   t  e
  
  int minutes = (int) strtol(timeStamp + 3, &endPtr, 10); // +3 to shift the string ptr to point towards minutes

  if (endPtr - (timeStamp + 3)!= 2)
    return -1;

  return hours * 60 + minutes;
}


