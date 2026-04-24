namespace DF
{

DF_API void* ArenaPush(Arena* arena, int size)
{
    if (size > arena->size - arena->offset)
    {
        // TODO: Log error

        return nullptr;
    }

    void* res = (char*)arena->base + arena->offset;

    arena->offset += size;

    return res;
}

} // namespace DF
