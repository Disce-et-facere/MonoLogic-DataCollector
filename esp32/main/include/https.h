#ifndef __https_h
#define __https_h

#include "esp_err.h"

#define updateTime 30

void httpsTask(void *pvParameter);
esp_err_t httpsAuthenticate(void);

#endif // !__https_h
