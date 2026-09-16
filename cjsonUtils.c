
#include <cjson/cJSON.h>

typedef struct prayerObject {
  short status; // 0 -> success, 1 -> failure
  int timeToUpcoming;
  cJSON *root;
  const char *upcoming;
  const char *date;
  const char *Fajr;
  const char *Sunrise;
  const char *Dhuhr;
  const char *Asr;
  const char *Maghrib;
  const char *Isha;
  const char *Lastthird;
} prayerObject;

#define prayerObjectDecompose(prayerObject) (prayerObject).Fajr, (prayerObject).Sunrise,(prayerObject).Dhuhr, (prayerObject).Asr, (prayerObject).Maghrib, (prayerObject).Isha, (prayerObject).Lastthird

prayerObject parseResObject(const char *resBody);

char *getStrFromcJSON(const cJSON *entryBlock, const char *entryName);

prayerObject initPrayersFromcJSONEntryBlock(const cJSON *entryBlock);

prayerObject parseResObject(const char *resBody)
{
  cJSON *responseJSONObject = cJSON_Parse(resBody);

  if (!responseJSONObject)
    goto failure;

  const cJSON *responseJSONObjectData = cJSON_GetObjectItemCaseSensitive(responseJSONObject, "data");

  //get prayer times
  const cJSON *responseJSONObjectTimings = cJSON_GetObjectItemCaseSensitive(responseJSONObjectData, "timings");

  if (responseJSONObjectTimings == NULL)
    goto cleanFailure;

  prayerObject Prayers = initPrayersFromcJSONEntryBlock(responseJSONObjectTimings);

  if (!Prayers.Fajr || !Prayers.Sunrise || !Prayers.Dhuhr || !Prayers.Asr || !Prayers.Maghrib || !Prayers.Isha || !Prayers.Lastthird)
    goto cleanFailure;


  //get the current date
  const cJSON *responseJSONObjectDate = cJSON_GetObjectItemCaseSensitive(responseJSONObjectData, "date");

  if (!responseJSONObjectDate)
    goto cleanFailure;

  char *readableDate = getStrFromcJSON(responseJSONObjectDate, "readable");

  if (!readableDate)
    goto cleanFailure;

  Prayers.date = readableDate;
  Prayers.status = 0;
  Prayers.root = responseJSONObject; //To be deleted in main

  return Prayers;

  cleanFailure:
    cJSON_Delete(responseJSONObject);

  failure:
    return (prayerObject) {
    .status = 1
  };

}

char *getStrFromcJSON(const cJSON *entryBlock, const char *entryName)
{
  const cJSON *entryTab = cJSON_GetObjectItemCaseSensitive(entryBlock, entryName);

  if(!entryTab)
    return NULL;

  return (cJSON_IsString(entryTab)) ? entryTab->valuestring : NULL;
}

prayerObject initPrayersFromcJSONEntryBlock(const cJSON *entryBlock)
{
  return (prayerObject) {
    .upcoming = NULL,
    .timeToUpcoming = 0,
    .Fajr = getStrFromcJSON(entryBlock, "Fajr"),
    .Sunrise  = getStrFromcJSON(entryBlock, "Sunrise"),
    .Dhuhr = getStrFromcJSON(entryBlock, "Dhuhr"),
    .Asr = getStrFromcJSON(entryBlock, "Asr"),
    .Maghrib = getStrFromcJSON(entryBlock, "Maghrib"),
    .Isha = getStrFromcJSON(entryBlock, "Isha"),
    .Lastthird = getStrFromcJSON(entryBlock, "Lastthird"),
  };
}
