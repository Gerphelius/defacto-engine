// ArgsList: Stores a list of types
template <typename... Args>
struct ArgsList {};

// FuncTraitsImpl: Base for storing argument types
template <typename... Args>
struct FuncTraitsImpl {
    using Types = ArgsList<Args...>;
};

// FuncTraits: Primary template (undefined for non-callables)
template <typename Func>
struct FuncTraits;

// FuncTraits specialization for free functions/function pointers
template <typename Ret, typename... Args>
struct FuncTraits<Ret(*)(Args...)> : FuncTraitsImpl<Args...> {};

// FuncTraits specialization for non-const member function pointers (e.g., lambdas with captures)
template <typename Ret, typename Class, typename... Args>
struct FuncTraits<Ret(Class::*)(Args...)> : FuncTraitsImpl<Args...> {};

// FuncTraits specialization for const member function pointers (e.g., lambdas without captures)
template <typename Ret, typename Class, typename... Args>
struct FuncTraits<Ret(Class::*)(Args...) const> : FuncTraitsImpl<Args...> {};

// FuncArgsT: Extracts argument types from Func
template <typename Func>
using FuncArgsT = typename FuncTraits<decltype(&std::decay_t<Func>::operator())>::Types;

// FirstArgImpl: Primary template (Func, ArgsListType)
template <typename Func, typename ArgsListType>
struct FirstArgImpl;

// FirstArgImpl specialization: Extracts first type from ArgsList<T, Rest...>
template <typename Func, typename T, typename... Rest>
struct FirstArgImpl<Func, ArgsList<T, Rest...>> {
    using Type = T;
};

// FirstArg: Extracts first argument type of Func
template <typename Func>
struct FirstArg {
    using Type = typename FirstArgImpl<Func, FuncArgsT<Func>>::Type;
};

// FirstArgT: Alias for FirstArg::Type
template <typename Func>
using FirstArgT = typename FirstArg<Func>::Type;
