#include "srampuf.h"

// Protocol defines for transmitting data from STM to C#-Backend.
// See datenbank.md for detailed description.
// 10 integers with control data and information are transmitted (protocol fields)
#define SRAMPUF_PROTOCOL_FIELDS                 10
#define SRAMPUF_PROTOCOL_MAGIGNUMBER            0xEB0155E5u

// UART handle pointer
static USART_HandleTypeDef *_husart = 0;
static unsigned int _pufbegin = 0;
static unsigned int _pufend = 0;
static unsigned int _initialised = 0;


int initSramPuf(USART_HandleTypeDef *husart, unsigned int membegin, unsigned int memend) {
    _husart = 0;
    _initialised = 0;
    if (husart == 0) return -1;
    if (husart->State != HAL_USART_STATE_READY) return -2;
    if (membegin < 0x20000000 || memend > 0x20018000) return -3;

    _husart = husart;
    _pufbegin = membegin;
    _pufend = memend;
    _initialised = 1;
    return 0;
}


int uartPrintf(const char *format, ...) {
    int const n = 256;
    int ret = 0;
    char buf[n];
    va_list args;

    if (_husart == 0) return -1;
    
    va_start(args, format);
    ret = vsnprintf(buf, n, format, args);
    va_end(args);

    // ret < 0  -> encoding error
    // ret >= n -> buffer was too small for requested string
    if (ret < 0 || ret >= n) return -2;

	if (HAL_USART_Transmit(_husart, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY) != HAL_OK) return -3;

    return ret;
}


int memoryDump(unsigned int membegin, unsigned int memend, int const humanreadable) {
    if (humanreadable) {
        uartPrintf("Printing memory from 0x%08X - 0x%08X\n", membegin, memend);
        uartPrintf("============================================\n");
        uartPrintf("Address     0 - 3    4 - 7    8 - B    C - F\n\n");

        for (unsigned int* i = (unsigned int*)membegin; i < (unsigned int*)memend; i = i + 4) {
            uartPrintf("%08X    ", (unsigned int)i);

            for(int word = 0; word < 4; word = word + 1) {
                unsigned int value = *(i + word);
                uartPrintf("%08X ", value);
            }

            uartPrintf("\n");
        }
        uartPrintf("\n");
    }
    else {
        for (unsigned int* i = (unsigned int*)membegin; i < (unsigned int*)memend; i = i + 4) {
            for(int word = 0; word < 4; word = word + 1) {
                unsigned int value = *(i + word);
                unsigned char c1 = (value & 0xFF000000) >> 24;
                unsigned char c2 = (value & 0x00FF0000) >> 16;
                unsigned char c3 = (value & 0x0000FF00) >> 8;
                unsigned char c4 = (value & 0x000000FF);
                uartPrintf("%c%c%c%c", c1, c2, c3, c4);
            }
        }
    }

    return 0;
}


int f401reMemDump(int const humanreadable) {
    const unsigned int begin = 0x20000000;
    const unsigned int end   = 0x20018000;
    return memoryDump(begin, end, humanreadable);
}


int srampufDump(int const humanreadable) {
    return memoryDump(_pufbegin, _pufend, humanreadable);
}


// Helper function to split an uint32 into 4 bytes and write into
// buffer. Least significant bits are placed first into the buffer.
// Incremented pointer value is returned.
unsigned char* writeIntToByteBuffer(unsigned char *buf, uint32_t i) {
    if (buf == 0) return buf;
    *buf++ = (i & 0x000000ff);
    *buf++ = (i & 0x0000ff00) >> 8;
    *buf++ = (i & 0x00ff0000) >> 16;
    *buf++ = (i & 0xff000000) >> 24;
    return buf;
}


int getSrampufProtocolBytes() {
    return SRAMPUF_PROTOCOL_FIELDS*4 + (_pufend - _pufbegin);
}


int srampufProtocol(unsigned char *buf, size_t len, size_t bs, float temp) {
    if (buf == 0) return -1;
    if (len != getSrampufProtocolBytes()) return -2;     // Double-check that the right length is used
    if (bs != BS_NUCLEOF401RE) return -3;           // BS must be a valid board specifier, see header
    if (!_initialised) return -4;                   // Initialise the module first

    // Convention: temperature is transmitted as int multiplied by factor 1000 (0.001 °C)
    uint32_t tempInt = (uint32_t)(temp * 1000);

    // Generally: least significant bits are sent first
    // 0xEB0155E5 is sent as 4 bytes: E5 - 55 - 01 - EB

    buf = writeIntToByteBuffer(buf, (uint32_t)SRAMPUF_PROTOCOL_MAGIGNUMBER);
    buf = writeIntToByteBuffer(buf, DeviceID_Get32(0));
    buf = writeIntToByteBuffer(buf, DeviceID_Get32(1));
    buf = writeIntToByteBuffer(buf, DeviceID_Get32(2));
    buf = writeIntToByteBuffer(buf, (uint32_t)BS_NUCLEOF401RE);
    buf = writeIntToByteBuffer(buf, (uint32_t)_pufbegin);
    buf = writeIntToByteBuffer(buf, (uint32_t)_pufend);
    buf = writeIntToByteBuffer(buf, tempInt);

    // PUF region is sent in ascending address order: 0x20000000, then 0x20000001... until the last byte.

    for (unsigned int* i = (unsigned int*)_pufbegin; i < (unsigned int*)_pufend; i++) {
        buf = writeIntToByteBuffer(buf, *i);
    }

    // 8 zero-bytes
    buf = writeIntToByteBuffer(buf, 0);
    buf = writeIntToByteBuffer(buf, 0);  
    return 0;
}



