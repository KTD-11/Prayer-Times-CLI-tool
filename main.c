
/*
 * prayer: a CLI prayer-times tool
 * Copyright (C) 2026 KTD
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

typedef enum errorTypes {
  NONE,
  TOO_MANY_ARGUMENTS,
  FEW_ARGUMENTS,
  HELP_REQUEST,
  VERSION_REQUEST,
  REQUEST_WITH_DEFAULT,
  SET_DEFAULT,
  SET_DEFAULT_INVALID_ARGUEMNT_ORDER,
  SET_DEFAULT_INVALID_NUMBER_ARGEMENTS,
  SET_DEFAULT_INVALID_COUNTRY_CODE_LENGTH,
  SET_DEFAULT_UNABLE_TO_ACCESS_FILE,
  SUCCESSFULLY_ADDED,
  URL_PARSEING,
  RETRIEVE_FAIL_TO_ACCESS,
  RETRIEVE_FAIL_TO_MALLOC,
  RETRIEVE_DONE,
  CURL_MEMORY_ALLOCATION,
  CURL_FETCHING,
  CURL_INIT,
  cJSON_PARSING,
  TIME_TO_UPCOMING
} errorTypes;

#include "cjsonUtils.c"
#include "counterToUpcoming.c"
#include "curlUtils.c"
#include "parseCommandLineArguments.c"
#include "prepUrl.c"
#include "printUtils.c"

int main(int argc, char *argv[]) {

  errorTypes errorStatus = parseArgV(argc, argv);
  char *countryCode = NULL;
  char *cityName = NULL;
  int didRetrieveDefaultConfig = 0;

  if (errorStatus == SET_DEFAULT) {
    errorStatus = generateDefaultConfig(argc, argv);
    goto end;
  }

  if (!(errorStatus == NONE || errorStatus == REQUEST_WITH_DEFAULT))
    goto end;

  if (errorStatus == REQUEST_WITH_DEFAULT) {
    char *configStr = NULL;
    errorStatus = retrieveDefaultConfig(&configStr);

    if (errorStatus != RETRIEVE_DONE)
      goto end;

    didRetrieveDefaultConfig = 1;

    //<countryCode>,<cityName>\0
    //^~~~~~~~~~~~~^~~~~~~~~~~^
    // can only     Arbitrary
    // be length    length
    // 2

    configStr[2] =
        '\0'; // replacing the comma with a NULL to split the string in two

    countryCode = configStr;
    cityName = configStr + 3;
  }

  else {
    countryCode = argv[1];
    cityName = argv[2];
  }

  char *URL = getFullUrl(cityName, countryCode);

  if (!URL) {
    errorStatus = URL_PARSEING;
    goto end;
  }

  CURLResponse res = curlGetResponse(URL);

  if (res.status != 0) {
    errorStatus = (res.status == 1)   ? CURL_MEMORY_ALLOCATION
                  : (res.status == 2) ? CURL_FETCHING
                                      : CURL_INIT;

    goto end;
  }

  prayerObject Prayers = parseResObject(res.body);

  if (Prayers.status != 0) {
    errorStatus = cJSON_PARSING;
    goto end;
  }

  addTimerToUpcoming(&Prayers, getCurrentTimeStampInt(getTimeStruct()));

  if (Prayers.upcoming == NULL) {
    errorStatus = TIME_TO_UPCOMING;
    goto end;
  }

end:
  switch (errorStatus) {
  case URL_PARSEING:
    free(URL);
    fprintf(stderr, "Fatal Error: System ran out of memory during initializing "
                    "enpoint prayers URL. Terminating with status 1\n");
    goto endFailure;

  case TOO_MANY_ARGUMENTS:
    fprintf(stderr, "Fatal Error: Too many arguemnts; check --help for correct "
                    "usage. Terminating with status 1\n");
    goto endFailure;

  case FEW_ARGUMENTS:
    fprintf(stderr, "Fatal Error: Not enough arguemnts; check --help for "
                    "correct usage. Terminating with status 1\n");
    goto endFailure;

  case SET_DEFAULT_INVALID_ARGUEMNT_ORDER:
    fprintf(stderr, "Fatal Error: Invalid arguemnt order; check --help for "
                    "correct usage. Terminating with status 1\n");
    goto endFailure;

  case SET_DEFAULT_INVALID_NUMBER_ARGEMENTS:
    fprintf(stderr, "Fatal Error: Invalid amount of arguemnt; check --help for "
                    "correct usage. Terminating with status 1\n");
    goto endFailure;

  case SET_DEFAULT_INVALID_COUNTRY_CODE_LENGTH:
    fprintf(stderr,
            "Fatal Error: Invalid length for countryCode; countryCode has to "
            "be exactly 2 letters, e.g. US. Terminating with status 1\n");
    goto endFailure;

  case SET_DEFAULT_UNABLE_TO_ACCESS_FILE:
    fprintf(stderr, "Fatal Error: System couldn't make config file. "
                    "Terminating with status 1\n");
    goto endFailure;

  case VERSION_REQUEST:
    printf("prayers v1.2.2\n");
    return EXIT_SUCCESS;

  case SUCCESSFULLY_ADDED:
    printf("\nSuccessfully added default settings; you can now call `prayer` "
           "directly without needing to specify the arguemnt.\n"
           "You can always change the defaults with the same flag or call the "
           "tool regularly with the normal arguemnts\n");
    return EXIT_SUCCESS;

  case HELP_REQUEST:
    printf("Usage: prayers <countryCode> <cityName>\n"
           "Options:\n"
           "--help                                    Display the help menu "
           "(this one)\n\n"
           "--set-default <countryCode> <cityName>    Sets default values for "
           "countryCode and cityName so that you're able to call `prayer` "
           "directly\n\n"
           "                                          without needing to "
           "specify them with each call. You can always change them with the "
           "same flag, or call the tool regularly\n"
           "--version                                 Displays the current "
           "version of the tool\n\n");
    return EXIT_SUCCESS;

  case RETRIEVE_FAIL_TO_ACCESS:
    fprintf(stderr, "Fatal Error: System couldn't access config file. "
                    "Terminating with status 1\n");
    goto endFailure;

  case RETRIEVE_FAIL_TO_MALLOC:
    fprintf(stderr, "Fatal Error: system ran out of memory while accessing the "
                    "config file. Terminating with status 1\n");
    goto endFailure;

  case CURL_MEMORY_ALLOCATION:
    fprintf(stderr, "Fatal Error: system ran out of memory while beginning the "
                    "prayers http request. Terminating with status 1\n");
    goto endCleanFailure;

  case CURL_FETCHING:
    fprintf(stderr, "Fatal Error: %s. Terminating with status 1\n",
            curl_easy_strerror(res.CURLStatus));
    goto endCleanFailure;

  case CURL_INIT:
    fprintf(stderr, "Fatal Error: System couldn't begin prayers http request. "
                    "Terminating with status 1\n");
    goto endCleanFailure;

  case cJSON_PARSING:
    fprintf(stderr,
            "Fatal Error: System failed during parsing the prayers response "
            "object; the server may be down temporarily or you may have "
            "entered and invalid cityName/countryCode\n"
            "FYI: make sure the the city name is hyphinated if it consists of "
            "more than one word, e.g. `new-york`. also make sure that the "
            "order is countryCode then cityName. Terminating with status 1\n");
    free(res.body);
    goto endCleanFailure;

  case TIME_TO_UPCOMING:
    fprintf(stderr,
            "Fatal Error: System ran into a problem while parsing prayers for "
            "the upcoming prayer .Terminating with status 1\n");
    free(res.body);
    cJSON_Delete(Prayers.root);
    goto endCleanFailure;
  default:
    printFinal(countryCode, cityName, Prayers);
    if (didRetrieveDefaultConfig)
      free(countryCode);

    cJSON_Delete(Prayers.root);
    free(res.body);
    free(URL);
    return EXIT_SUCCESS;
  }

endCleanFailure:
  free(URL);

endFailure:
  if (didRetrieveDefaultConfig)
    free(countryCode);
  return EXIT_FAILURE;
}
