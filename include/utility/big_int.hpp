///----------------------------------
/// @file big_int.hpp
/// @brief 64 �~ Size�r�b�g�̐���
///----------------------------------
#pragma once

namespace tudb
{
    // template <class T>
    // concept 

    template <std::size_t Size = 2>
    requires (Size > 1)
    struct big_int : public carry_over_container<unsigned long long, Size>
    {
        using value_type = unsigned long long;

        // �����������Ă��Ȃ�
        bool sign = true;

        constexpr big_int(std::integral auto i)
            : carry_over_container<unsigned long long, Size>{}
        {
            this->at(0) = (i < 0) ? -i : i;
            this->sign = (i >= 0);
        }

        template <std::unsigned_integral... Types>
        requires (sizeof...(Types) == Size)
        constexpr big_int(Types... args)
            : carry_over_container<unsigned long long, Size>{(value_type)args...}
        {
            // for (int i = 0; i < Size; i++) this->at(i) = v[i];
            // auto swallow[] = {}
        }

        template <std::size_t N>
        constexpr big_int(const big_int<N>& v)
            : carry_over_container<unsigned long long, Size>{}
        {
            for (std::size_t i = 0; i < (std::min)(Size, N); i++) this->at(i) = v[i];
        }

        static constexpr auto plus(value_type l_v, value_type r_v)
        {
            const auto is_carried = (std::numeric_limits<value_type>::max)() - l_v < r_v;
            return std::pair{
                std::array{l_v + r_v, (value_type)is_carried},
                is_carried
            };
        }

        static constexpr auto mul(value_type l_v, value_type r_v)
        {
            constexpr auto harf_digits = std::numeric_limits<value_type>::digits / 2;
            constexpr auto harf_mask = ~(value_type)0 >> harf_digits;

            // ���������ꂼ��ő�r�b�g���̔����ŕ���
            const auto upper_l = l_v >> harf_digits;
            const auto lowwer_l = l_v & harf_mask;
            const auto upper_r = r_v >> harf_digits;
            const auto lowwer_r = r_v & harf_mask;

            // �������|��(�����Z����ہA���オ����l������K�v�����镔����plus��)
            const auto lowwer = lowwer_l * lowwer_r;
            const auto [middle, _dust1] = plus(lowwer_l * upper_r, upper_l * lowwer_r);
            const auto upper = upper_l * upper_r;

            // �������|���̌��ʂ����v(�r�b�g���Z�q�Ɖ��Z�E���Z�ł̓r�b�g���Z�q�̂ق����D��x���Ⴂ�̂Œ���)
            const auto [lowwer_result, _dust2] = plus(lowwer, middle[0] << harf_digits);
            const auto upper_result = upper
                + (middle[0] >> harf_digits)
                + (middle[1] << harf_digits)
                + lowwer_result[1];

            return std::pair{std::array{lowwer_result[0], upper_result}, upper_result > 0};
        }

        constexpr big_int& operator++(int)
        {
            big_int _this = (*this);
            this->set(this->run(plus, 1));
            return _this;
        }

        constexpr big_int& operator++()
        {
            this->set(this->run(plus, 1));
            return (*this);
        }

        constexpr big_int operator~() const
        {
            big_int result = (*this);
            for (auto& v : result) v = ~v;
            return result;
        }

        constexpr operator value_type() const noexcept { return this->front(); }

        constexpr explicit operator bool() const noexcept
        {
            for (auto v : (*this))
                if (v) return true;
            return false;
        }

        constexpr bool operator!() const noexcept { return !static_cast<bool>(*this); }

        template <std::size_t N>
        constexpr big_int& operator+=(const big_int<N>& v)
        {
            this->set(this->run(plus, v));
            return (*this);
        }

        template <std::size_t N>
        constexpr big_int& operator*=(const big_int<N>& v)
        {
            this->set(this->run(mul, v, plus));
            return (*this);
        }
    };

    template <std::size_t N1, std::size_t N2>
    constexpr std::strong_ordering operator<=>(const big_int<N1>& l, const big_int<N2>& r)
    {
        using value_type = big_int<N1>::value_type;
        constexpr auto larger = (std::max)(N1, N2);

        // �T�C�Y���傫���ق��֌^�𓝈ꂷ��
        const auto l_arr = big_int<larger>(l);
        const auto r_arr = big_int<larger>(r);

        // ���̑傫���ق������r����
        for (auto i : std::views::iota((std::size_t)0, larger) | std::views::reverse) {
            if (l_arr[i] < r_arr[i]) return std::strong_ordering::less;
            if (l_arr[i] > r_arr[i]) return std::strong_ordering::greater;
        }
        return std::strong_ordering::equal;
    }

    template <std::size_t N1, std::size_t N2>
    constexpr auto operator+(const big_int<N1>& l, const big_int<N2>r) { return big_int<(std::max)(N1, N2)>(l) += r; }
}
