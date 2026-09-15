// url to be mimicked
// https://api.aladhan.com/v1/timingsByCity/09-09-2026?city=<cityName>&country=<countryCode>
// ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~^~~~~~^~~~~^~~~~~~~~~^^
//           base url                        date     city  city country  country
//                                                    header      header  code

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct strSlice {
  char *str;
  int count;
} strSlice;


const char *baseURL = "https://api.aladhan.com/v1/timingsByCity/";

char *getFullUrl(const char *cityName, const char *countryCode);

strSlice getYearDateSubSection(struct tm timeStruct);

strSlice getMonthDataSubSection(struct tm timeStruct);

strSlice getDayDataSubSection(struct tm timeStruct);

char *getFullUrl(const char *cityName, const char *countryCode)
{
  char *URL = NULL;

  time_t timeObject = time(NULL);
  struct tm timeStruct = *localtime(&timeObject);

  strSlice dateYearSection = getYearDateSubSection(timeStruct);
  strSlice dateMonthSection = getMonthDataSubSection(timeStruct);
  strSlice dateDaySection = getDayDataSubSection(timeStruct);

  strSlice dateSection = {
    .str = NULL,
    .count = 0
  };

  if (!dateDaySection.str || !dateMonthSection.str || !dateYearSection.str)
    goto end;

  dateSection.count = dateDaySection.count + dateMonthSection.count + dateYearSection.count + 2; // +2 for the `-` in the URL

  dateSection.str = malloc((dateSection.count + 1) * sizeof(char)); // +1 for NULL terminator

  if (!dateSection.str)
    goto end;

  int stringFormattedCharsCount = snprintf(dateSection.str, dateSection.count + 1, "%s-%s-%s", dateDaySection.str, dateMonthSection.str, dateYearSection.str);

  if (stringFormattedCharsCount != dateSection.count)
    goto end;

  int lenURL = strlen(baseURL) + dateSection.count + 6 + strlen(cityName) + 9 + strlen(countryCode) + 1; // +6 for `?city=`, +9 for `&country=`, and +1 for NULL terminator;

  URL = malloc(lenURL * sizeof(char));

  if (!URL)
    goto end;

  stringFormattedCharsCount = snprintf(URL, lenURL, "%s%s?city=%s&country=%s", baseURL, dateSection.str, cityName, countryCode);

  if (stringFormattedCharsCount != lenURL - 1)
  {
    free(URL);
    URL = NULL;
    goto end;
  }

  end:
    if (dateDaySection.str)
      free(dateDaySection.str);

    if (dateMonthSection.str)
      free(dateMonthSection.str);

    if (dateYearSection.str)
      free(dateYearSection.str);

    if (dateSection.str)
      free(dateSection.str);

    return URL;
}

strSlice getYearDateSubSection(struct tm timeStruct)
{
  const int year = timeStruct.tm_year;

  char *yearString = (char *) malloc(5 * sizeof(char)); // 4 spaces for each year character +1 for NULL terminator

  if (yearString == NULL)
    return (strSlice) {
      .str = NULL,
      .count = 0 
    };

  int stringFormattedCharsCount = snprintf(yearString, 5, "%d", year + 1900); //fixed the offset; since the timeObject starts counting from 1900 //len YYYY +1 for NULL terminator

  if (stringFormattedCharsCount != 4)
  {
    free(yearString);

    return (strSlice) {
      .str = NULL,
      .count = 0
    };
  }

  return (strSlice) {
    .str = yearString,
    .count = 4
  };
}

strSlice getMonthDataSubSection(struct tm timeStruct)
{
  const int month = timeStruct.tm_mon;

  char *monthString = (char *) malloc((2 + 1) * sizeof(char)); // 2 for MM, +1 for NULL terminator

  if (monthString == NULL)
    return (strSlice) {
      .str = NULL,
      .count = 0
    };

  int stringFormattedCharsCount = snprintf(monthString, 2 + 1, "%02d", month + 1); //fixed the offset; since the month are indexed from 0 to 11. 2 for MM, +1 for NULL terminator

  if (stringFormattedCharsCount != 2)
  {
    free(monthString);

    return (strSlice) {
      .str = NULL,
      .count = 0
    };
  }

  return (strSlice) {
    .str = monthString,
    .count = 2
  };
}

strSlice getDayDataSubSection(struct tm timeStruct)
{
  const int day = timeStruct.tm_mday;

  char *dayString = malloc((2 + 1) * sizeof(char)); // 2 for DD, +1 for NULL terminator

  if (dayString == NULL)
    return (strSlice) {
      .str = NULL,
      .count = 0 
    };

  int stringFormattedCharsCount = snprintf(dayString, 2 + 1, "%02d", day); // 2 for DD, +1 for NULL terminator

  if (stringFormattedCharsCount != 2)
  {
    free(dayString);

    return (strSlice) {
      .str = NULL,
      .count = 0
    };
  }

  return (strSlice) {
    .str = dayString,
    .count = 2
  };
}
