#ifndef __https_h
#define __https_h

#include "esp_err.h"

void httpsTask(void *pvParameter);
esp_err_t httpsAuthenticate(void);

#endif // !__https_h