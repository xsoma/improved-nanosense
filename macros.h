#pragma once

#define stringify(x) #x
#define concat_impl(x, y) x##y
#define concat(x, y) concat_impl(x, y)

template<typename t, t v>
struct constant_holder
{
    enum class val_holder : t
    {
        val = v
    };
};

#define constant(value) ((decltype(value))constant_holder<decltype(value), value>::val_holder::val)
