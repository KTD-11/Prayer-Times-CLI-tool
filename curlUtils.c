
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

typedef struct CURLResponse {
  char *body;
  size_t count;
  short status; // 0-> ok, 1 -> memory error, 2 curlFetchErr, 3 curl initErr
  CURLcode CURLStatus;
  CURL *root;
} CURLResponse;


CURLResponse curlGetResponse(char *URL);

size_t concatonateResponse(void *buffer, size_t bufferElemSize, size_t bufferElemCount, void *concatonatedResponse);

CURLResponse curlGetResponse(char *URL)
{
  CURLResponse res = {.body = malloc(1), .count = 0};

  curl_global_init(CURL_GLOBAL_DEFAULT);
  CURL *curl = curl_easy_init();
  CURLcode result;

  if (!curl)
  {
    free(res.body);

    res.body = NULL;
    res.count = 0;
    res.status = 3;

   goto end; 
  }


  if (!res.body)
  {
      res.body = NULL;
      res.count = 0;
      res.status = 1;

    goto freeEnd;
  }

  curl_easy_setopt(curl, CURLOPT_URL, URL);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, concatonateResponse);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&res);

  result = curl_easy_perform(curl);

  if (result != CURLE_OK) 
  {
    free(res.body);

    res.body = NULL;
    res.count = 0;
    res.status = 2;
    res.CURLStatus = result;

    goto freeEnd;
  }

  res.status = 0;
  res.CURLStatus = CURLE_OK;

  freeEnd:
    curl_easy_cleanup(curl);
  end:
    return res;
}

size_t concatonateResponse(void *buffer, size_t bufferElemSize, size_t bufferElemCount, void *concatonatedResponse) 
{
  size_t realSize = bufferElemCount * bufferElemSize;

  CURLResponse *castedConcatonatedResponse = concatonatedResponse;

  char *tempBufferRealloc = realloc(castedConcatonatedResponse->body,castedConcatonatedResponse->count + realSize + 1); //+1 for `\0`

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

