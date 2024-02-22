#ifndef testing_utils_hpp_
#define testing_utils_hpp_

#include <concepts>
#include <iostream>
#include <cstdio>
#include <type_traits>
#include <string>
#include <format>

template <typename T>
concept IsRunnable = requires (T func) {
    {func()} -> std::same_as<void>;
};

template <typename S, typename T>
concept IsComparable = std::is_convertible_v<S, T>;

template <typename S, IsComparable<S> T>
bool Expect(T expected, S actual, const std::string_view mismatchedMessage) {
    if (expected == actual) {
        return true;
    }

    std::cout << std::vformat(mismatchedMessage,std::make_format_args(expected, actual)) << "\n";

    return false;
}


template <IsRunnable Runnable>
void RunTestWithTestName(Runnable runner, const char* testName) {
    std::cout << "running: " << testName << "\n";
    runner();
}

#define run(func) ({ \
RunTestWithTestName(func, #func); \
})

#endif
