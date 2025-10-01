#ifndef ERROR_CODES
#define ERROR_CODES

enum class ErrorCode {
    None = 0,
    WatchdogReset = 1,
    CommunicationInitError = 2,
    DsuProtectionError = 3,
    DsuFailure = 4,
    DsuBusError = 5,
    Communication = 6
};

#endif