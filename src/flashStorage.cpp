#include "flashStoragelib.h"
#include <errorCodes.h>
#include <FlashStorage.h>

namespace FlashStorageLibrary {

    FlashStorage(flashStorage, int);

    ErrorCode FlashStorage::readFlashStorage() {
        SerialUSB.println("Reading flash storage");
        ErrorCode code = static_cast<ErrorCode>(flashStorage.read());
        SerialUSB.print("Value found: ");
        SerialUSB.println(static_cast<int>(code));
        return code;
    }

    void FlashStorage::writeFlashStorage(ErrorCode errorCode) {
        SerialUSB.print("Writing ");
        SerialUSB.print(static_cast<int>(errorCode));
        SerialUSB.println(" to flash storage");
        flashStorage.write(static_cast<int>(errorCode));
    }
}