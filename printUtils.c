#include <stdio.h>

void printFinal(char *countryCode, char *cityName, prayerObject Prayers)
{
  printf(" \n");
  printf("✦───────────────────────────────────────✦\n");
  printf("             Prayer Times\n");
  printf("        %s, %s · %s\n", countryCode, cityName, Prayers.date);
  printf("✦───────────────────────────────────────✦\n\n");

  printf("           Fajr          %s\n", Prayers.Fajr);
  printf("           Sunrise       %s\n", Prayers.Sunrise);
  printf("           Dhuhr         %s\n", Prayers.Dhuhr);
  printf("           Asr           %s\n", Prayers.Asr);
  printf("           Maghrib       %s\n", Prayers.Maghrib);
  printf("           Isha          %s\n", Prayers.Isha);
  printf("           Last Third    %s\n", Prayers.Lastthird);

  printf("\n✦───────────────────────────────────────✦\n");
}
