#pragma once

template <typename T>
class tracker {
    T value;

public:
    bool dirty = false;

    tracker(T theValue) : value(theValue) {}

    T& get()
    {
        dirty = true;
        return value;
    }

    T const& get() const { return value; }

    operator const T&() const { return value; }
    operator T&() { return value; }

    template <typename U>
    requires (requires (T& t, U const& u) { t = u; })
        tracker& operator=(U const& other) {
        dirty = true;
        value = other;
        return *this;
    }

    template <typename U>
    requires (requires (T& t, U const& u) { t += u; })
        tracker& operator+=(U const& other) {
        dirty = true;
        value += other;
        return *this;
    }

    template <typename U>
    requires (requires (T& t, U const& u) { t -= u; })
        tracker& operator-=(U const& other) {
        dirty = true;
        value -= other;
        return *this;
    }

    template <typename U>
    requires (requires (T& t, U const& u) { t *= u; })
        tracker& operator*=(U const& other) {
        dirty = true;
        value *= other;
        return *this;
    }

    template <typename U>
    requires (requires (T& t, U const& u) { t /= u; })
        tracker& operator/=(U const& other) {
        dirty = true;
        value /= other;
        return *this;
    }

    template <typename U>
    requires (requires (T& t, U const& u) { t + u; })
        tracker operator+(U const& other) {
        tracker<T> ret = this + other;
        return ret;
    }

    template <typename U>
    requires (requires (T& t, U const& u) { t - u; })
        tracker operator-(U const& other) {
        tracker<T> ret = this - other;
        return ret;
    }

    template <typename U>
    requires (requires (T& t, U const& u) { t* u; })
        tracker operator*(U const& other) {
        tracker<T> ret = this * other;
        return ret;
    }

    template <typename U>
    requires (requires (T& t, U const& u) { t / u; })
        tracker operator/(U const& other) {
        tracker<T> ret = this / other;
        return ret;
    }
};