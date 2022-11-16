#pragma once
#include <array>

namespace tudb
{
    /**
     * @class
     * @brief �p�����[�^�p�b�N�ŕ����񃊃e�������g�p���邽�߂̌^
    */
    template <std::size_t N>
    struct cstr
    {
        char buf[N];
        // �I�[�������i�[�ς݂ł�����̂Ƃ��Ĉ���
        static constexpr auto size = N - 1;
    };

    template <std::size_t N1, std::size_t N2>
    constexpr auto concat(const cstr<N1>& s1, const cstr<N2>& s2)
    {
        // s1�̏I�[�����͏������邽�߁A-1
        cstr<N1 + N2 - 1> _s{};
        for (int i = 0; i < N1 - 1; i++) _s.buf[i] = s1.buf[i];
        for (int i = 0; i < N2; i++) _s.buf[i + N1 - 1] = s2.buf[i];
        return _s;
    }
    template <std::size_t N1, std::size_t N2, std::size_t... Sizes>
    constexpr auto concat(const cstr<N1>& s1, const cstr<N2>& s2, const cstr<Sizes>&... strs)
    {
        return concat(concat(s1, s2), strs...);
    }

    template <std::size_t N1, std::size_t N2>
    constexpr bool operator==(const cstr<N1>& s1, const cstr<N2>& s2)
    {
        // �����v�f���̏ꍇ�Aarray�ɔ�r���Z�q�̒�`������̂ł����K�p
        if constexpr (N1 == N2)
            return std::to_array(s1.buf) == std::to_array(s2.buf);
        return false;
    }

    /**
     * @fn
     * @brief �e���v���[�g�����œn���������l�̌������擾����B�������͔C�ӂ̐i�����w�肷��
    */
    template <std::integral auto V, std::size_t Hex = 10>
    requires (Hex >= 2)
    constexpr auto get_digit()
    {
        std::size_t digit = 1;
        for (auto val = V; val /= static_cast<decltype(V)>(Hex); digit++);
        return digit;
    }

    /**
     * @fn
     * @brief �e���v���[�g�����œn���������l��cstr�ɕϊ�����B�i����n�����ƂŁA(2,8,16)�i�����e�����̂悤�ȕ�����ɕϊ�����
    */
    template <std::integral auto V, int Hex = 10, bool UsePrefix = false>
    requires (V >= 0 && Hex >= 2)
    constexpr auto to_string()
    {
        // 10�i���Ƃ��Č������擾
        constexpr auto len = get_digit<V, Hex>();
        // �I�[�����̒������z��̗v�f���Ɋ܂߂�
        cstr<len + 1> s;
        s.buf[len] = '\0';
        auto val = V;
        for (auto i = len; i > 0; val /= Hex) {
            const auto code = static_cast<char>(val % Hex);
            s.buf[--i] = (code < 10)
                ? '0' + code
                : 'A' + code - 10;
        }

        // �i�����e�����̐ړ��q��t�^
        if constexpr (UsePrefix) {
            if constexpr (Hex == 2) return concat(cstr{"0b"}, s);
            else if constexpr (Hex == 8) return concat(cstr{"0"}, s);
            else if constexpr (Hex == 16) return concat(cstr{"0x"}, s);
            else return s;
        }
        else return s;
    }
    // �����̏ꍇ
    template <std::integral auto V, std::size_t Hex = 10, bool UsePrefix = false>
    constexpr auto to_string() { return concat(cstr{"-"}, to_string<-V, Hex, UsePrefix>()); }

    template <auto V> struct to_cstr;

    /**
     * @fn
     * @brief cstr�ł��邩�̃��^�֐�
    */
    template <class T> struct is_cstr : public std::false_type{};
    template <std::size_t N> struct is_cstr<cstr<N>> : public std::true_type{};

    /**
     * @brief cstr�ł��邱�Ƃ���������R���Z�v�g
    */
    template <class T>
    concept StringLiteralSpecificable = is_cstr<T>::value;

    // �֐��Ńe���v���[�g�����Ɏw��\�ł������ꍇ�͕s�v�Ȃ̂ŏ���
    // �^�Ƃ��ĕ�����ێ���������R���e�i�Ƃ��Ďc�����ق����ǂ����H
    template <StringLiteralSpecificable auto... Strs>
    struct concat_cstr { static constexpr auto value = concat(Strs...); };
    template <StringLiteralSpecificable auto... Strs>
    constexpr auto concat_cstr_v = concat_cstr<Strs...>::value;

    template <StringLiteralSpecificable FormatStr, auto... Args>
    struct format_cstr
    {

    };
}
