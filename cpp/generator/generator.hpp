#ifndef generator_h_
#define generator_h_

#include <iostream>
#include <thread>
#include <coroutine>
#include <concepts>
#include <optional>
#include <functional>

namespace tetris::generator {

    template<typename T>
    struct Generator {
        struct promise_type;
        using handle_type = std::coroutine_handle<promise_type>;
                    
        struct promise_type {
            std::optional<T> value;
            std::exception_ptr exception;
                
            Generator get_return_object() {
                return Generator(std::coroutine_handle<promise_type>::from_promise(*this));
            }

            std::suspend_always initial_suspend() { return {}; }
            std::suspend_always final_suspend() noexcept { return {}; };
            void unhandled_exception() { exception = std::current_exception(); }

            template<std::convertible_to<T> From> 
            std::suspend_always yield_value(From &&from) {
                value = std::optional<T>(std::forward<From>(from));
                return {};
            }

            template<std::convertible_to<T> From> 
            void return_value(From &&from) {
                value = std::optional<T>(std::forward<From>(from));                
            }
        };

        handle_type coroutineHandle;

        Generator(handle_type h) : coroutineHandle(h) {}
        Generator(const Generator &) = delete;
        ~Generator() { coroutineHandle.destroy(); }

        std::optional<T> operator()() {            
            
            if (isDone) {
                return std::nullopt;
            }

            coroutineHandle();
            isDone = coroutineHandle.done();

            if (isDone) {
                return std::nullopt;
            }

            if (coroutineHandle.promise().exception) {
                std::rethrow_exception(coroutineHandle.promise().exception);
            }

            return std::move(coroutineHandle.promise().value);
        }

        void loop(std::function<void(T)> func) {
            
            while(std::optional<T> next = (*this)()) {
                //next.and_then(func);
                func(next.value());
            }
            
        }

        private:
            bool isDone = false;
    };
}

#endif