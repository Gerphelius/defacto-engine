#include <stdarg.h>

#define STB_SPRINTF_IMPLEMENTATION
#include <stb_sprintf.h>

namespace DF
{

DF_API String StrFormat(Arena* buffer, const char* str, ...)
{
    String strFmt {};

    strFmt.data = (char*)buffer->base + buffer->offset;

    va_list args {};
    va_start(args, str);
    int capacity = buffer->size - buffer->offset;
    int offset   = stbsp_vsnprintf((char*)strFmt.data, capacity, str, args);
    va_end(args);

    if (offset >= capacity)
    {
        // LOG: String has been truncated

        buffer->offset += capacity;
        strFmt.length = capacity - 1;
    }
    else
    {
        buffer->offset += offset + 1; // +1 is for zero terminator
        strFmt.length = offset;
    }

    return strFmt;
}

DF_API void StrFormat(char* buffer, int size, const char* str, ...)
{
    va_list args {};
    va_start(args, str);
    int offset = stbsp_vsnprintf(buffer, size, str, args);
    va_end(args);
}

} // namespace DF
