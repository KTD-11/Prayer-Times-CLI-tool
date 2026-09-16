
/*
 * prayer: a CLI prayer-times tool
 * Copyright (C) 2026 <Your Name>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <cjson/cJSON.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include <stdio.h>
#include <stdlib.h>

#include "prepUrl.c"
#include "curlUtils.c"
#include "cjsonUtils.c"
#include "printUtils.c"

typedef enum errorTypes {
  NONE,
  URL_PARSEING,
  CURL_MEMORY_ALLOCATION,
  CURL_FETCHING,
  CURL_INIT,
  cJSON_PARSING
} errorTypes;

int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    fprintf(stderr, "USAGE: prayer <countryCode> <cityName>");
    goto endFailure;
  }

  errorTypes errorStatus = NONE;

  char *URL = getFullUrl(argv[2], argv[1]);

  if (!URL)
  {
    errorStatus = URL_PARSEING;
    goto end;
  }

  CURLResponse res = curlGetResponse(URL);

  if (res.status != 0)
  {
    errorStatus = (res.status == 1) ? CURL_MEMORY_ALLOCATION :
                  (res.status == 2) ? CURL_FETCHING : CURL_INIT;

    goto end;
  }

  prayerObject Prayers = parseResObject(res.body);

  if (Prayers.status != 0)
  {
    errorStatus = cJSON_PARSING;
    goto end;
  }

  end:

  switch (errorStatus) {
    case URL_PARSEING:
      free(URL);
      fprintf(stderr, "Fatal Error: System ran out of memory during initializing enpoint prayers URL. Terminating with status 1\n");
      goto endFailure;

    case CURL_MEMORY_ALLOCATION:
      fprintf(stderr, "Fatal Error: system ran out of memory while beginning the prayers http request. Terminating with status 1\n");
      goto endCleanFailure;

    case CURL_FETCHING:
      fprintf(stderr, "Fatal Error: %s. Terminating with status 1\n", curl_easy_strerror(res.CURLStatus));
      goto endCleanFailure;

    case CURL_INIT:
      fprintf(stderr, "Fatal Error: System couldn't begin prayers http request. Terminating with status 1\n");
      goto endCleanFailure;

    case cJSON_PARSING:
      fprintf(stderr, "Fatal Error: System failed during parsing the prayers response object; the server may be down temporarily or you may have entered and invalid cityName/countryCode\n"
                                      "FYI: make sure the the city name is hyphinated if it consists of more than one word, e.g. `new-york`. also make sure that the order is countryCode then cityName. Terminating with status 1\n");
      free(res.body);
      goto endCleanFailure;

    default:
      printFinal(argv[1], argv[2], Prayers);
      cJSON_Delete(Prayers.root);
      free(res.body);
      free(URL);
      return EXIT_SUCCESS;
  }

  endCleanFailure:
    free(URL);

  endFailure:
    return EXIT_FAILURE;
}
