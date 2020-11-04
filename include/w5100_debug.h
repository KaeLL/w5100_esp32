
#pragma once

#include "esp_log.h"

#define tag_def( X ) static const char *TAG = X

#define f_entry() ESP_LOGD( TAG, "entered %s", __func__ )
#define f_exit()  ESP_LOGD( TAG, "exited %s at %s:%d", __func__, __FILE__, __LINE__ )
