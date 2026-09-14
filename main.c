#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct CURLResponse {
  char *body;
  size_t count;
} CURLResponse;

typedef struct prayerObject {
  const char *Fajr;
  const char *Sunrise;
  const char *Dhuhr;
  const char *Asr;
  const char *Sunset;
  const char *Maghrib;
  const char *Isha;
  const char *Lastthird;
} prayerObject;

size_t concatonateResponse(void *buffer, size_t bufferElemSize, size_t bufferElemCount, void *concatonatedResponse);

inline static char *helper_cJSONGetStr(const cJSON *entryBlock, const char *entryName);

prayerObject initPrayersFromcJSONEntryBlock(const cJSON *entryBlock);

#define prayerObjectDecompose(prayerObject) (prayerObject).Fajr, (prayerObject).Sunrise,(prayerObject).Dhuhr, (prayerObject).Asr, (prayerObject).Maghrib, (prayerObject).Isha, (prayerObject).Lastthird

int main(int argc, char *argv[]) {

  curl_global_init(CURL_GLOBAL_DEFAULT);
  CURL *curl = curl_easy_init();
  CURLcode result;

  if (!curl) {
    fprintf(stderr, "Error initializing curl");
    return EXIT_FAILURE;
  }

  CURLResponse res = {.body = malloc(1), .count = 0};

  if (!res.body) {
    fprintf(stderr, "Error: Device ran out of memory");
    curl_easy_cleanup(curl);
    return EXIT_FAILURE;
  }

  curl_easy_setopt(curl, CURLOPT_URL,
                   "https://api.aladhan.com/v1/timingsByCity/"
                   "09-09-2026?city="
                   "Cairo"
                   "&country="
                   "EG");

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, concatonateResponse);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&res);

  result = curl_easy_perform(curl);

  if (result != CURLE_OK) {
    fprintf(stderr, "Error: %s", curl_easy_strerror(result));
    free(res.body);
    return EXIT_FAILURE;
  }

  //transforming the string into a json object
  cJSON *responseJSONObject = cJSON_Parse(res.body);

  if (responseJSONObject == NULL)
    goto failure;

  const cJSON *responseJSONObjectTimings = cJSON_GetObjectItemCaseSensitive(
    cJSON_GetObjectItemCaseSensitive(responseJSONObject, "data"), "timings"); 

  if (responseJSONObjectTimings == NULL)
    goto failure;

  prayerObject Prayers = initPrayersFromcJSONEntryBlock(responseJSONObjectTimings);

  if (!Prayers.Fajr || !Prayers.Sunrise || !Prayers.Dhuhr || !Prayers.Asr || !Prayers.Maghrib || !Prayers.Isha || !Prayers.Lastthird)
    goto failure;

  printf("Fajr => %s\nSunrise => %s\nDhuhr = > %s\nAsr => %s\nMaghrib => %s\nIsha => %s\nLastthird => %s\n", prayerObjectDecompose(Prayers));

  cJSON_Delete(responseJSONObject);
  free(res.body);
  curl_easy_cleanup(curl);
  return EXIT_SUCCESS;

  failure:
    cJSON_Delete(responseJSONObject);
    free(res.body);
    curl_easy_cleanup(curl);

    fprintf(stderr, "Parsing Error: failed to parse JSON response object");
    return EXIT_FAILURE;
}

size_t concatonateResponse(void *buffer, size_t bufferElemSize,
                           size_t bufferElemCount, void *concatonatedResponse) {
  size_t realSize = bufferElemCount * bufferElemSize;

  CURLResponse *castedConcatonatedResponse = concatonatedResponse;

  char *tempBufferRealloc =
      realloc(castedConcatonatedResponse->body,
              castedConcatonatedResponse->count + realSize + 1); //+1 for `\0`

  if (!tempBufferRealloc) {
    return CURL_WRITEFUNC_ERROR;
  }

  castedConcatonatedResponse->body = tempBufferRealloc;

  memcpy(castedConcatonatedResponse->body + castedConcatonatedResponse->count,
         buffer, realSize);
  
  castedConcatonatedResponse->count += realSize;

  castedConcatonatedResponse->body[castedConcatonatedResponse->count] = '\0';
  return realSize;
}

inline static char *helper_cJSONGetStr(const cJSON *entryBlock, const char *entryName){ 
  const cJSON *entryTab = cJSON_GetObjectItemCaseSensitive(entryBlock, entryName);

  if(!entryTab)
    return NULL;

  return (cJSON_IsString(entryTab)) ? entryTab->valuestring : NULL;
}

prayerObject initPrayersFromcJSONEntryBlock(const cJSON *entryBlock){
  return (prayerObject) {
    .Fajr = helper_cJSONGetStr(entryBlock, "Fajr"),
    .Sunrise  = helper_cJSONGetStr(entryBlock, "Sunrise"),
    .Dhuhr = helper_cJSONGetStr(entryBlock, "Dhuhr"),
    .Asr = helper_cJSONGetStr(entryBlock, "Asr"),
    .Maghrib = helper_cJSONGetStr(entryBlock, "Maghrib"),
    .Isha = helper_cJSONGetStr(entryBlock, "Isha"),
    .Lastthird = helper_cJSONGetStr(entryBlock, "Lastthird"),
  };
}
