#pragma once

class PreInitialization
{
public:
    template <typename... Fns>
    PreInitialization(Fns&&... fns)
    {
        (void(std::forward<Fns>(fns)()), ...);;
    }
};
