#pragma once

#ifdef LOCAL_DEV_BUILD
// Nur hier lokal im VS Code Build sind die Logs aktiv
#  include "nrf_log.h"
#  define log NRF_LOG_INFO
#  define log_error NRF_LOG_ERROR
#else
// Für Docker (und alles andere) wird es zum No-Op
#  define log(...) (void)0
#endif

// #pragma once
// #include "nrf_log.h"
// #define log NRF_LOG_INFO
