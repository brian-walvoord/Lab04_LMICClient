#ifndef FLASH_STORAGE
#define FLASH_STORAGE

#include <errorCodes.h>

/**
 * Need to log:
 * 1. Timestamps
 *  - Each entry if possible
 *  - Count since last reset
 * 2. System resets
 *  - Source of reset
 * 3. Run-time Errors
 *  - Faulty sensor reading
 *  - Comms error (antenna not connected)
 */

namespace FlashStorageLibrary {

    class FlashStorage {
        public: 
            /**
             * Method reads error code in flash storage and returns
             * the stored value
             */
            ErrorCode readFlashStorage();

            /**
             * Method writes an integer error code to flash storage
             */
            void writeFlashStorage(ErrorCode errorCode);
    };
}

#endif