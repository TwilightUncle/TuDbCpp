///----------------------------------
/// @file regex.hpp 
/// @brief �萔���ɒu���鐳�K�\��(���j�R�[�h���Ή�)
///----------------------------------
#ifndef TUSTRCPP_INCLUDE_GUARD_REGEX_HPP
#define TUSTRCPP_INCLUDE_GUARD_REGEX_HPP

#include <regex>
#include <tustrcpp/cstr.hpp>

#include <tustrcpp/regex/common.hpp>
#include <tustrcpp/regex/bracket.hpp>
#include <tustrcpp/regex/char_class.hpp>
#include <tustrcpp/regex/quantifier.hpp>
#include <tustrcpp/regex/char_set.hpp>
#include <tustrcpp/regex/capture.hpp>

namespace tustr
{
    /**
     * @fn
     * @brief �n���ꂽ�����͈͂�����/���ۃ��X�g�ɓW�J���A�^����ꂽ�����񂪃}�b�`���邩���肷��֐��I�u�W�F�N�g��ԋp����
    */
    template <cstr Pattern, std::size_t Pos>
    constexpr auto get_regex_char_range_matcher()
    {
        return [](const std::string& comp) -> bool {
            using range_parser = regex_char_set_parser<Pattern, Pos>;
            return is_allowed_string(
                comp,
                range_parser::allow_or_deny,
                range_parser::value.data(),
                range_parser::bk_value.data(),
                false
            );
        };
    }

    /**
     * @class
     * @brief ���K�\���i�[�I�u�W�F�N�g�B���I�ɐ������ꂽ�p�^�[���ɂ��Ă͍l�����Ȃ�
    */
    template <cstr Pattern>
    struct regex
    {
        static_assert(is_collect_regex_back_slash(Pattern.view()));

        using flag_type = std::regex_constants::syntax_option_type;

        /**
         * @fn
         * @brief ���ʓ��̐��K�\������
        */
        static constexpr std::optional<std::regex_constants::error_type> parse_bracket_inner(const std::string_view& bracket_inner, const char bracket_be)
        {
            switch (bracket_be) {
                case '[':
                    // �ċA�I�Ȋ��ʂ͑��݂��Ȃ����߁A���ۃ��X�g�ɊJ�n���ʂ��܂܂�Ă�����A�E�g
                    if (!is_allowed_string(std::string(bracket_inner), false, "[", "")) return std::regex_constants::error_brack;
                    break;
                case '(':
                    // �L���v�`�����X�g�͍ċA�I��
                    if (const auto error = parse_pattern(bracket_inner)) return error;
                    break;
                case '{':
                    // ���ʎ��ƃ��j�R�[�h�̎w��݂̂̂��߁A�����X�g�Ŋm�F
                    if (!is_allowed_string(std::string(bracket_inner), true, ",0123456789abcdef", "")) return std::regex_constants::error_brack;
                    break;
            }
            return std::nullopt;
        }

        /**
         * @fn
         * @brief ���K�\���̕��@�̌���
        */
        static constexpr std::optional<std::regex_constants::error_type> parse_pattern(const std::string_view& pattern_view)
        {
            // �o�b�N�X���b�V���̌���
            if (is_collect_regex_back_slash(pattern_view)) return std::regex_constants::error_escape;

            // �ꕶ��������
            for (
                auto [i, ch] = std::pair{0, pattern_view[0]};
                i < pattern_view.size();
                ch = pattern_view[++i]
            ) {
                const auto attr = get_regex_character_attribute(ch);
                // ���ꕶ���ł͂Ȃ�
                if (!(attr & REGEX_SPECIAL_MASK)) continue;

                // ���ꕶ���̑O�Ƀo�b�N�X���b�V�����肩�̔���
                const auto is_before_bk = eq_before_char(pattern_view, i, '//');
                if ((attr & (REGEX_BK_SPECIAL | REGEX_CLASSES_BK_SPECIAL) && !is_before_bk)) continue;
                if ((attr & (REGEX_SPECIAL | REGEX_CLASSES_SPECIAL)) && is_before_bk) continue;

                // �Ƃ����ʂ������Ȃ茻���̂͂����������߃G���[
                if (attr & REGEX_BRANCKET_EN) {
                    switch (ch) {
                        case ']': return std::regex_constants::error_brack;
                        case ')': return std::regex_constants::error_paren;
                        case '}': return std::regex_constants::error_brace;
                    }
                }

                // ���ʂ����o
                // if (attr & REGEX_BRANCKET_BE) {
                //     const auto [inner, en_pos, br_error] = extract_regex_bracket_inner(pattern_view, i);
                //     if (br_error) return br_error;
                //     // ���ʓ����̌��؂͕ʊ֐��ɈϏ��̂��߁A�Ƃ����ʂ܂ŃC���f�b�N�X��i�߂�
                //     if (const auto inner_error = parse_bracket_inner(inner, ch)) return inner_error;
                //     i = en_pos;
                //     continue;
                // }
            }
            return std::nullopt;
        }

        // consteval regex()
        // {
        //     // ���K�\���̃t���O,���@�`�F�b�N
        //     // assert(validate_syntax_option(syntax));
        //     static_assert(is_collect_regex_back_slash(Pattern.view()));
        //     // assert(validate_regex_syntax(pattern.view(), syntax));
        // };
        
        /**
         * @fn
         * @brief syntax���w�肷��r�b�g�̂ݔ����o��
        */
        static constexpr flag_type get_syntax_bits(flag_type syntax)
        {
            constexpr auto syntax_mask = std::regex_constants::ECMAScript
                | std::regex_constants::basic
                | std::regex_constants::extended
                | std::regex_constants::awk
                | std::regex_constants::grep
                | std::regex_constants::egrep;
            return syntax & syntax_mask;
        }

        /**
         * @fn
         * @brief syntax�Ɉ���w�肳��Ă��Ȃ��ꍇ�AECMAScript�̃r�b�g�𗧂Ă�
        */
        static constexpr flag_type shap_syntax_option(flag_type syntax)
        {
            if (!get_syntax_bits(syntax))
                syntax |= flag_type::ECMAScript;
            return syntax;
        }
    
        /**
         * @fn
         * @brief syntax�̌���
        */
        static constexpr bool validate_syntax_option(flag_type syntax)
        {
            // return std::popcount((std::uint64_t)get_syntax_bits(syntax)) <= 1;
            // ����ECMAScript�o�[�W���������������Ă��Ȃ��̂ŁA�ق��̕��@���w�肳��Ă�����NG�Ƃ���
            return get_syntax_bits(shap_syntax_option(syntax)) == flag_type::ECMAScript;
        }
    };

    using empty_regex = regex<"">;
}

#endif // TUSTRCPP_INCLUDE_GUARD_REGEX_HPP
