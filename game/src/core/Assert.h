#pragma once

namespace core
{
    void reportAssertFailure(
        const char* expression,
        const char* message,
        const char* file,
        int line
    );
}

#define ReturnIf(condition, ...)                                                             \
    do                                                                                       \
    {                                                                                        \
        if (condition)                                                                       \
        {                                                                                    \
            return __VA_ARGS__;                                                              \
        }                                                                                    \
    } while (false)

#define ReturnUnless(condition, ...)                                                         \
    do                                                                                       \
    {                                                                                        \
        if (!(condition))                                                                    \
        {                                                                                    \
            return __VA_ARGS__;                                                              \
        }                                                                                    \
    } while (false)

#define Assert(condition, message)                                                           \
    do                                                                                       \
    {                                                                                        \
        if (!(condition))                                                                    \
        {                                                                                    \
            core::reportAssertFailure(#condition, message, __FILE__, __LINE__);             \
        }                                                                                    \
    } while (false)

#define AssertReturnIf(condition, message, ...)                                              \
    do                                                                                       \
    {                                                                                        \
        if (condition)                                                                       \
        {                                                                                    \
            core::reportAssertFailure(#condition, message, __FILE__, __LINE__);             \
            return __VA_ARGS__;                                                              \
        }                                                                                    \
    } while (false)

#define AssertReturnUnless(condition, message, ...)                                          \
    do                                                                                       \
    {                                                                                        \
        if (!(condition))                                                                    \
        {                                                                                    \
            core::reportAssertFailure(#condition, message, __FILE__, __LINE__);             \
            return __VA_ARGS__;                                                              \
        }                                                                                    \
    } while (false)

#define BreakIf(condition)                                                                   \
    do                                                                                       \
    {                                                                                        \
        if (condition)                                                                       \
        {                                                                                    \
            break;                                                                           \
        }                                                                                    \
    } while (false)

#define BreakUnless(condition)                                                               \
    do                                                                                       \
    {                                                                                        \
        if (!(condition))                                                                    \
        {                                                                                    \
            break;                                                                           \
        }                                                                                    \
    } while (false)

#define AssertBreakIf(condition, message)                                                    \
    do                                                                                       \
    {                                                                                        \
        if (condition)                                                                       \
        {                                                                                    \
            core::reportAssertFailure(#condition, message, __FILE__, __LINE__);             \
            break;                                                                           \
        }                                                                                    \
    } while (false)

#define AssertBreakUnless(condition, message)                                                \
    do                                                                                       \
    {                                                                                        \
        if (!(condition))                                                                    \
        {                                                                                    \
            core::reportAssertFailure(#condition, message, __FILE__, __LINE__);             \
            break;                                                                           \
        }                                                                                    \
    } while (false)

#define ContinueIf(condition)                                                                \
    do                                                                                       \
    {                                                                                        \
        if (condition)                                                                       \
        {                                                                                    \
            continue;                                                                        \
        }                                                                                    \
    } while (false)

#define ContinueUnless(condition)                                                            \
    do                                                                                       \
    {                                                                                        \
        if (!(condition))                                                                    \
        {                                                                                    \
            continue;                                                                        \
        }                                                                                    \
    } while (false)

#define AssertContinueIf(condition, message)                                                 \
    do                                                                                       \
    {                                                                                        \
        if (condition)                                                                       \
        {                                                                                    \
            core::reportAssertFailure(#condition, message, __FILE__, __LINE__);             \
            continue;                                                                        \
        }                                                                                    \
    } while (false)

#define AssertContinueUnless(condition, message)                                             \
    do                                                                                       \
    {                                                                                        \
        if (!(condition))                                                                    \
        {                                                                                    \
            core::reportAssertFailure(#condition, message, __FILE__, __LINE__);             \
            continue;                                                                        \
        }                                                                                    \
    } while (false)
