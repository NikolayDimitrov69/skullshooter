#include "pch.h"

#include "Assert.h"

#include <Windows.h>
#include <intrin.h>

namespace core
{
    void reportAssertFailure(
        const char* expression,
        const char* message,
        const char* file,
        int line
    )
    {
        std::ostringstream stream;
        stream << "Assertion failed.\n\n";
        stream << "Expression: " << expression << "\n";
        stream << "Message: " << message << "\n";
        stream << "File: " << file << "\n";
        stream << "Line: " << line << "\n";

        const std::string errorMessage = stream.str();
        MessageBoxA(nullptr, errorMessage.c_str(), "skullshooter assert", MB_OK | MB_ICONERROR);

#ifdef _DEBUG
        __debugbreak();
#endif
    }
}
