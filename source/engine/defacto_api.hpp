#pragma once

#if defined(_WIN32)
#define DF_EXPORT extern "C" __declspec(dllexport)
#else
#define DF_EXPORT __attribute__((visibility("default")))
#endif

#define ON_UPDATE(name) const char* name()

namespace DF::API
{

typedef ON_UPDATE(OnUpdateT);

} // namespace DF::API
