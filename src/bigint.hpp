#pragma once

#include <cstdint>
#include <iostream>
#include <string_view>
#include <cctype>
#include <iomanip>
#include <ranges>
#include <stdexcept>
#include <utility>

class Bigint {
public:
    static constexpr int BASE = 100;
    static constexpr int BASE_DIGITS = 2;

    constexpr Bigint() = default;
    constexpr Bigint(int16_t number);
    constexpr Bigint(std::string_view view);

    constexpr Bigint& operator+=(const Bigint& other);
    friend constexpr Bigint operator+(Bigint lhs, const Bigint& rhs);

    friend std::ostream& operator<<(std::ostream& os, const Bigint& bigint);

private:
    constexpr void add_unsigned(const Bigint& other);
    constexpr void sub_unsigned(const Bigint& other);
    static constexpr int cmp_unsigned(const Bigint& lhs, const Bigint& rhs);

    constexpr void trim_leading_zeroes();

    std::basic_string<int16_t> m_buf;
    bool m_neg = false;
};

constexpr Bigint::Bigint(int16_t number) {
    if (number < 0) {
        m_neg = true;
        number *= -1;
    }
    m_buf.push_back(number);
}

constexpr Bigint::Bigint(std::string_view view) {
    if (view.empty()) {
        throw std::runtime_error("Bigint(std::string_view): Input can't be empty");
    }

    if (view[0] == '-') {
        m_neg = true;
        view = view.substr(1);
        if (view.empty()) {
            throw std::runtime_error("Bigint(std::string_view): Input can't contain signs only");
        }
    }

    auto convert_range_to_num = [&](std::size_t head, std::size_t tail) {
        int16_t num = 0;
        for (auto i = head; i < tail; ++i) {
            if (!std::isdigit(view[i])) {
                throw std::runtime_error("Bigint(std::string_view): Input can't contain non-digit characters besides the sign");
            }
            num = 10 * num + (view[i] - '0');
        }
        return num;
    };

    // reserve int(ceil(view.size() / BASE_DIGITS))
    m_buf.reserve((view.size() + BASE_DIGITS - 1) / BASE_DIGITS);

    auto tail = view.size();
    for (; tail >= BASE_DIGITS; tail -= BASE_DIGITS) {
        const auto head = tail - BASE_DIGITS;
        m_buf.push_back(convert_range_to_num(head, tail));
    }

    if (tail != 0) {
        m_buf.push_back(convert_range_to_num(0, tail));
    }
}

constexpr void Bigint::add_unsigned(const Bigint& other) {
    const auto result_size = std::max(m_buf.size(), other.m_buf.size());
    m_buf.resize(result_size);

    bool carry = false;

    auto add_digit = [&](std::size_t i) {
        const auto other_val = (i < other.m_buf.size())
                             ? other.m_buf[i]
                             : 0;

        m_buf[i] += other_val + carry;
        carry = (m_buf[i] >= BASE);

        if (carry) {
            m_buf[i] -= BASE;
        }
    };

    std::size_t i = 0;
    for (; i < result_size; ++i) {
        add_digit(i);
    }

    if (carry) {
        if (i >= result_size) {
            m_buf.push_back(0);
        }
        add_digit(i);
    }
}

constexpr void Bigint::sub_unsigned(const Bigint& other) {
    bool carry = false;

    auto sub_digit = [&](std::size_t i) {
        const auto other_val = (i < other.m_buf.size())
                             ? other.m_buf[i]
                             : 0;

        m_buf[i] -= (other_val + carry);
        carry = (m_buf[i] < 0);

        if (carry) {
            m_buf[i] += BASE;
        }
    };

    for (std::size_t i = 0; i < other.m_buf.size() || carry; ++i) {
        sub_digit(i);
    }

    trim_leading_zeroes();
}

constexpr int Bigint::cmp_unsigned(const Bigint& lhs, const Bigint& rhs) {
    if (lhs.m_buf.size() != rhs.m_buf.size()) {
        return (lhs.m_buf.size() < rhs.m_buf.size()) ? -1 : 1;
    }

    for (auto i = lhs.m_buf.size(); i-- > 0; ) {
        if (lhs.m_buf[i] != rhs.m_buf[i]) {
            return lhs.m_buf[i] - rhs.m_buf[i];
        }
    }

    return 0;
}

constexpr void Bigint::trim_leading_zeroes() {
    while (!m_buf.empty() && m_buf.back() == 0) {
        m_buf.pop_back();
    }

    if (m_buf.empty()) {
        m_neg = false;
    }
}

constexpr Bigint& Bigint::operator+=(const Bigint& other) {
    if (m_neg == other.m_neg) {
        add_unsigned(other);
    } else if (cmp_unsigned(*this, other) >= 0) {
        sub_unsigned(other);
    } else {
        auto tmp = other;
        std::swap(*this, tmp);
        sub_unsigned(tmp);
    }

    return *this;
}

constexpr Bigint operator+(Bigint lhs, const Bigint& rhs) {
    return lhs += rhs;
}

std::ostream& operator<<(std::ostream& os, const Bigint& bigint) {
    if (bigint.m_buf.empty()) {
        os << 0;
        return os;
    }

    if (bigint.m_neg) {
        os << '-';
    }

    os << int(bigint.m_buf.back());

    auto remaining = std::views::reverse(bigint.m_buf) | std::views::drop(1);
    for (auto elem : remaining) {
        os << std::setw(Bigint::BASE_DIGITS) << std::setfill('0') << int(elem);
    }

    return os;
}

