namespace SCPX
{

static void Text(DF::String string, uint8_t size = 16)
{
    Clay_String cString = {
        .length = string.length,
        .chars  = string.data,
    };

    CLAY_TEXT(cString,
              CLAY_TEXT_CONFIG({
                .textColor = { 255, 255, 255, 255 },
                .fontId    = 0,
                .fontSize  = size,
              }));
}

static void Text(const char* string, uint8_t size = 16)
{
    Clay_String cString = {
        .length = (int32_t)strlen(string),
        .chars  = string,
    };

    CLAY_TEXT(cString,
              CLAY_TEXT_CONFIG({
                .textColor = { 255, 255, 255, 255 },
                .fontId    = 0,
                .fontSize  = size,
              }));
}

static void Spacer()
{
    CLAY_AUTO_ID(
      { .layout = { .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) } } })
    {
    }
}

static void Button(const char* name,
                   void (*callback)(Clay_ElementId, Clay_PointerData, void*),
                   void* data)
{
    CLAY_AUTO_ID({
        .layout = {
            .sizing = {
                .width  = CLAY_SIZING_FIXED(0),
                .height = CLAY_SIZING_FIXED(0),
            },
            .padding = { 16, 16, 8, 8 },
            .childAlignment  = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER },
        },
        .backgroundColor = { 64.0f, (Clay_Hovered() ? 140.0f : 128.0f), 32.0f, 1.0f },
    })
    {
        Clay_OnHover(callback, data);

        Clay_String cName = {
            .length = (int32_t)strlen(name),
            .chars  = name,
        };

        CLAY_TEXT(cName,
                  CLAY_TEXT_CONFIG({
                    .textColor = { 255, 255, 255, 255 },
                    .fontId    = 0,
                    .fontSize  = 16,
                  }));
    }
}

} // namespace SCPX
