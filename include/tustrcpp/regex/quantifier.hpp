///----------------------------------
/// @file quantifier.hpp 
/// @brief regex.hpp�֘A
///----------------------------------
#ifndef TUSTRCPP_INCLUDE_GUARD_REGEX_QUANTIFIER_HPP
#define TUSTRCPP_INCLUDE_GUARD_REGEX_QUANTIFIER_HPP

namespace tustr
{
    /**
     * @class
     * @brief ���ʎq����͂���
    */
    template <cstr Pattern, std::size_t Pos>
    requires (Pos > 0)
    struct regex_quantifier_perser
        : public regex_char_attribute
    {
        static_assert(
            quantifier_chars.contains(char_to_cstr(Pattern[Pos])),
            "Invalied template argment [Pattern, Pos]. Must specified of '*', '+', '?', '{'."
        );

        // ���ʎq�̊J�n�ʒu
        static constexpr auto begin_pos = Pos;

        /**
         * @fn
         * @brief ���ʎq������������𒊏o
        */
        template <char C> requires (C == '{')
        static constexpr auto extract_quantifier()
        {
            using bracket_inner = regex_bracket_inner<Pattern, Pos>;
            constexpr auto inner_str = bracket_inner::value;

            // ���ʎq�̍\���`�F�b�N
            // {n}, {n,}, {n,m}�̂����ꂩ�̍\���ɍ��v���Ă��邱�Ƃ��m�F(n, m�͐��l�ł��邱��)
            static_assert(
                inner_str[0] != ','
                && inner_str.match_all_charset("0123456789,")
                && std::ranges::count(inner_str, ',') <= 1,
                "An error has occurred. [regex_quantifier_perser]"
            );

            return bracket_inner::value_with_bracket;
        }
        template <char C> requires (C != '{')
        static constexpr auto extract_quantifier() { return char_to_cstr(C); }

        // ���ʎq�̕�����𒊏o��������
        static constexpr auto quantifier = extract_quantifier<Pattern[Pos]>();

        // ���×~�̏ꍇ�^
        static constexpr bool negative = []() {
            const auto check_pos = Pos + quantifier.size();
            return check_pos < Pattern.size() && Pattern[check_pos] == '?';
        }();

        // ���ʎq�̖����̎��̈ʒu
        static constexpr auto end_pos = quantifier.size() + std::size_t(negative);

        // �ŏ��J��Ԃ���
        static constexpr auto min_count = []()->std::size_t {
            if constexpr (quantifier[0] == '{') {
                constexpr auto inner_str = quantifier.remove_prefix_suffix<1, 1>();
                return to_int<std::size_t>(split<inner_str, ",">()[0]);
            }
            else {
                if (quantifier[0] == '+') return 1;
                return 0;
            }
        }();

        // �ő�J��Ԃ���
        static constexpr auto max_count = []()->std::size_t {
            if constexpr (quantifier[0] == '?') return 1;
            else if constexpr (quantifier[0] == '{') {
                constexpr auto devides = split<quantifier.remove_prefix_suffix<1, 1>(), ",">();
                if (devides.size() == 1) return to_int<std::size_t>(devides[0]);
                else if (!devides[1].empty()) return to_int<std::size_t>(devides[1]);
            }
            return std::string_view::npos;
        }();
    };
}

#endif // TUSTRCPP_INCLUDE_GUARD_REGEX_QUANTIFIER_HPP
