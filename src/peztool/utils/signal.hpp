#pragma once
#include <vector>
#include <functional>


template<typename TSignal>
struct Dispatcher
{
public:
    using SignalCallback = std::function<void(TSignal const&)>;

public:
    static void emit(TSignal const& signal)
    {
        for (auto const& l : s_listener) {
            l(signal);
        }
    }

    static void subscribe(SignalCallback callback)
    {
        s_listener.push_back(callback);
    }

private:
    static std::vector<SignalCallback> s_listener;
};

template<typename TSignal>
std::vector<typename Dispatcher<TSignal>::SignalCallback> Dispatcher<TSignal>::s_listener = {};