
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

errorTypes parseArgV(int argc, char **argv) {
  if (argc == 1)
    return REQUEST_WITH_DEFAULT;

  for (int i = 1; i < argc;
       ++i) // i = 1, skipping the first argv argument (i.e. program name)
  {
    if (strcmp(argv[i], "--help") == 0)
      return HELP_REQUEST;

    if (strcmp(argv[i], "--set-default") == 0)
      return SET_DEFAULT;

    if (strcmp(argv[i], "--version") == 0)
      return VERSION_REQUEST;
  }

  if (argc > 3)
    return TOO_MANY_ARGUMENTS;

  return NONE;
}

errorTypes generateDefaultConfig(int argc, char **argv) {
  if (argc != 4) // program name, config flag, countryCode, cityName
    return SET_DEFAULT_INVALID_NUMBER_ARGEMENTS;

  // setting the order of arguments
  if (strcmp(argv[1], "--set-default") != 0)
    return SET_DEFAULT_INVALID_ARGUEMNT_ORDER;

  if (strlen(argv[2]) != 2)
    return SET_DEFAULT_INVALID_COUNTRY_CODE_LENGTH;

  FILE *configFile = fopen("./prayer.conf", "w");

  if (!configFile)
    return SET_DEFAULT_UNABLE_TO_ACCESS_FILE;

  // writing the following format to the config file <countryCode>,<cityName>
  int charactersWritten = fprintf(configFile, "%s,%s", argv[2], argv[3]);

  fclose(configFile);

  // making sure that the number of characters written is as it should
  if (charactersWritten != strlen(argv[2]) + strlen(argv[3]) + 1)
    return SET_DEFAULT_UNABLE_TO_ACCESS_FILE;

  return SUCCESSFULLY_ADDED;
}

errorTypes retrieveDefaultConfig(char **configStr) {
  FILE *configFile = fopen("./prayer.conf", "rb");

  if (!configFile)
    return RETRIEVE_FAIL_TO_ACCESS;

  // calculating the buffer size to dump the file into
  fseek(configFile, 0l, SEEK_END);
  int bufferSize = ftell(configFile);
  rewind(configFile);

  char *localBuffer =
      malloc((bufferSize + 1) * sizeof(char)); // +1 for NULL terminator

  if (!localBuffer) {
    fclose(configFile);
    return RETRIEVE_FAIL_TO_MALLOC;
  }

  fread(localBuffer, sizeof(char), bufferSize, configFile);

  localBuffer[bufferSize] = '\0';

  *configStr = localBuffer;

  return RETRIEVE_DONE;
}
