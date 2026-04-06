#include "log_define.h"
#include "log_format.h"
#include "printf_impl.h"
#include "util_tool.h"

#define LOG_LEVEL_SYMBOLE_SIZE 2

#define LOG_CONTENT_SIZE_MAX (LOG_BUFF_SIZE_MAX - LOG_PREPEND_SIZE_MAX - 2)

const static char HEX_CHAR_TABLE[] = "0123456789ABCDEF";

const static char level_symbols[] =
{
    [LOG_LEVEL_TRACE] = '>',
    [LOG_LEVEL_DEBUG] = '.',
    [LOG_LEVEL_INFO]  = '-',
    [LOG_LEVEL_WARN]  = '~',
    [LOG_LEVEL_ERROR] = '=',
    [LOG_LEVEL_FATAL] = '*',
    [LOG_LEVEL_NONE]  = ' ',
};

uint16_t log_format_string_args(const char* p_format, va_list args, uint8_t* p_buffer, uint16_t offset)
{
    offset = GET_MIN(offset, LOG_PREPEND_SIZE_MAX);

    uint16_t buffer_len = 0;

    /* Check if contains '%' symbol
     * If yes, use vsnprintf to format the string
     * If not, use memcpy to copy the string
     */
    char* match_ptr = strchr(p_format, '%');
    if (match_ptr != NULL)
    {
        buffer_len = VSNPRINTF_IMPL((char*)&p_buffer[offset], LOG_CONTENT_SIZE_MAX, p_format, args);
    }
    else
    {
        buffer_len = strlen(p_format);
        memcpy(&p_buffer[offset], p_format, buffer_len);
    }

    return buffer_len;
}

uint16_t log_format_string_number(const char* p_string, uint32_t number, uint8_t* p_buffer, uint16_t offset)
{
    offset = GET_MIN(offset, LOG_PREPEND_SIZE_MAX);

    uint16_t stringLen = 0;
    if (p_string != NULL)
    {
        stringLen = strlen(p_string);

        memcpy(&p_buffer[offset], p_string, stringLen);

        p_buffer[offset + stringLen]     = ':';
        p_buffer[offset + stringLen + 1] = ' ';

        stringLen += 2;
    }

    uint32_t numberValue = number;
    uint16_t numberLen   = log_format_number(numberValue, &p_buffer[offset + stringLen]);

    return numberLen + stringLen;
}

uint16_t log_format_hex_array(const char* p_caption, uint8_t* p_data, uint16_t data_len, uint8_t* p_buffer, uint16_t offset)
{
    offset = GET_MIN(offset, LOG_PREPEND_SIZE_MAX);
    uint16_t captionLen = strlen(p_caption);

    /**
     * Add caption at the beginning.
     * Eg. Output[003h]: 11 22 33 $
     */
    if (captionLen > 0)
    {
        memcpy(&p_buffer[offset], p_caption, captionLen);
        memcpy(&p_buffer[offset + captionLen], "[FFFh]: ", 8);

        if (data_len <= 0x0FFF)
        {
            uint8_t hiByte = (uint8_t)(data_len >> 8);
            uint8_t loByte = (uint8_t)(data_len);

            p_buffer[offset + captionLen + 1] = HEX_CHAR_TABLE[hiByte & 0x0F];
            p_buffer[offset + captionLen + 2] = HEX_CHAR_TABLE[(loByte & 0xF0) >> 4];
            p_buffer[offset + captionLen + 3] = HEX_CHAR_TABLE[loByte & 0x0F];
        }
    }

    uint16_t byte_index = captionLen + 8;

    /* Convert byte to two hex chars and one space, e.g. 0x1617 = 16 17 $ */
    uint16_t hexNum = GET_MIN(LOG_HEX_COUNT_MAX, data_len);

    for (uint16_t i = 0; i < hexNum; i++)
    {
        uint8_t byte = p_data[i];

        p_buffer[offset + byte_index]     = HEX_CHAR_TABLE[byte >> 4];
        p_buffer[offset + byte_index + 1] = HEX_CHAR_TABLE[byte & 0x0F];
        p_buffer[offset + byte_index + 2] = ' ';

        byte_index += 3;
    }

    return byte_index;
}

uint16_t log_format_number(uint32_t number, uint8_t* p_buffer)
{
    // Add '0x' prefix
    p_buffer[0] = '0';
    p_buffer[1] = 'x';

    uint16_t byte_index = 2;

    if (number == 0)
    {
        p_buffer[byte_index] = '0';

        byte_index += 1;
    }
    else if (number <= 0xFF)
    {
        p_buffer[byte_index]     = HEX_CHAR_TABLE[number >> 4];
        p_buffer[byte_index + 1] = HEX_CHAR_TABLE[number & 0x0F];

        byte_index += 2;
    }
    else if (number <= 0xFFFF)
    {
        p_buffer[byte_index]     = HEX_CHAR_TABLE[(number >> 12) & 0x0F];
        p_buffer[byte_index + 1] = HEX_CHAR_TABLE[(number >> 8) & 0x0F];
        p_buffer[byte_index + 2] = HEX_CHAR_TABLE[(number >> 4) & 0x0F];
        p_buffer[byte_index + 3] = HEX_CHAR_TABLE[number & 0x0F];

        byte_index += 4;
    }
    else
    {
        p_buffer[byte_index]     = HEX_CHAR_TABLE[(number >> 28) & 0x0F];
        p_buffer[byte_index + 1] = HEX_CHAR_TABLE[(number >> 24) & 0x0F];
        p_buffer[byte_index + 2] = HEX_CHAR_TABLE[(number >> 20) & 0x0F];
        p_buffer[byte_index + 3] = HEX_CHAR_TABLE[(number >> 16) & 0x0F];
        p_buffer[byte_index + 4] = HEX_CHAR_TABLE[(number >> 12) & 0x0F];
        p_buffer[byte_index + 5] = HEX_CHAR_TABLE[(number >> 8) & 0x0F];
        p_buffer[byte_index + 6] = HEX_CHAR_TABLE[(number >> 4) & 0x0F];
        p_buffer[byte_index + 7] = HEX_CHAR_TABLE[number & 0x0F];

        byte_index += 8;
    }

    return byte_index;
}

uint16_t log_format_append_eol(uint8_t* p_buffer, uint16_t buffer_len)
{
    if (buffer_len <= LOG_CONTENT_SIZE_MAX)
    {
        p_buffer[buffer_len]     = '\r';
        p_buffer[buffer_len + 1] = '\n';

        buffer_len += 2;
    }

    return buffer_len;
}

uint16_t log_format_prepend_tick_time(uint32_t timestamp, uint8_t* p_buffer, uint16_t offset)
{
    uint16_t byte_index = 0;

    p_buffer[offset + byte_index++] = '[';

    p_buffer[offset + byte_index++] = HEX_CHAR_TABLE[(timestamp >> 16) & 0x0F];
    p_buffer[offset + byte_index++] = HEX_CHAR_TABLE[(timestamp >> 12) & 0x0F];
    p_buffer[offset + byte_index++] = HEX_CHAR_TABLE[(timestamp >> 8) & 0x0F];
    p_buffer[offset + byte_index++] = HEX_CHAR_TABLE[(timestamp >> 4) & 0x0F];
    p_buffer[offset + byte_index++] = HEX_CHAR_TABLE[timestamp & 0x0F];

    p_buffer[offset + byte_index++] = ']';

    return byte_index;
}

uint16_t log_format_prepend_task_name(const char* p_name, uint8_t* p_buffer, uint16_t offset)
{
    uint16_t byte_index = 0;

    uint8_t name_len = strlen(p_name);

    p_buffer[offset + byte_index] = '[';

    byte_index += 1;

    if (name_len > 0)
    {
        memcpy(&p_buffer[offset + byte_index], p_name, name_len);

        byte_index += name_len;
    }
    else
    {
        p_buffer[offset + byte_index] = '@';

        byte_index += 1;
    }

    p_buffer[offset + byte_index] = ']';

    return byte_index + 1;
}

uint16_t log_format_prepend_level_symbol(uint8_t level, uint8_t* p_buffer, uint16_t offset)
{
    for (uint16_t i = 0; i < LOG_LEVEL_SYMBOLE_SIZE; i++)
    {
        p_buffer[offset + i] = (uint8_t)level_symbols[level];
    }

    return LOG_LEVEL_SYMBOLE_SIZE;
}
