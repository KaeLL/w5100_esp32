
#pragma once

#include "sdkconfig.h"

#if CONFIG_W5100_ENABLE_DEBUG_LOGS
#	include "esp_log.h"

#	define f_entry() ESP_LOGD( TAG, "entered %s", __func__ )
#	define f_exit()  ESP_LOGD( TAG, "exited %s at %s:%d", __func__, __FILE__, __LINE__ )
#else
#	define f_entry()
#	define f_exit()
#endif

#define tag_def( X ) static const char __attribute__((unused)) *TAG = X