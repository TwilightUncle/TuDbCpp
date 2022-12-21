///----------------------------------
/// @file perser.hpp 
/// @brief regex.hpp�֘A(�p�^�[����͋@�\����̃N���X�ɏW�ς�������)
///----------------------------------
#ifndef TUSTRCPP_INCLUDE_GUARD_REGEX_PERSER_HPP
#define TUSTRCPP_INCLUDE_GUARD_REGEX_PERSER_HPP

namespace tustr
{
    /**
     * @fn
     * @brief �ǂ̋@�\�̉�͂��s���Ă��邩�A�������ꉻ�ŏ�������B�f�t�H���g
    */
    template <cstr Pattern, std::size_t Pos>
    struct regex_parser : public add_quantifier<Pattern, regex_general<Pattern, Pos>>
    {
        // �P�̂Ŏw�肵�Ă͂����Ȃ�����������
        using type = add_quantifier<Pattern, regex_general<Pattern, Pos>>;
        static_assert(
            type::end_pos == Pattern.size()
            || !(regex_char_attribute::attributes[Pattern[type::end_pos]] & regex_char_attribute::DENY),
            "Invalied template argment [Pattern, Pos]. Must not specified of '}', ')', ']'."
        );
    };

    /**
     * @fn
     * @brief �����W���̏ꍇ
    */
    template <cstr Pattern, std::size_t Pos>
    requires (bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::CHARSET))
    struct regex_parser<Pattern, Pos> : public add_quantifier<Pattern, regex_char_set_parser<Pattern, Pos>> {};

    /**
     * @fn
     * @brief �����N���X�̏ꍇ
    */
    template <cstr Pattern, std::size_t Pos>
    requires (
        bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::CLASS)
        && !bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::BK)
    )
    struct regex_parser<Pattern, Pos> : public add_quantifier<Pattern, regex_char_class_parser<Pattern, Pos>> {};

    /**
     * @fn
     * @brief �����N���X�̏ꍇ2(�o�b�N�X���b�V���܂߂�2�����̂��߁A�ʒu����i�߂Ă���)
    */
    template <cstr Pattern, std::size_t Pos>
    requires (
        Pattern[Pos] == '\\' && Pattern.size() - 1 > Pos
        && regex_char_attribute::check_attrs_conjuction<regex_char_attribute::CLASS, regex_char_attribute::BK>(Pattern[Pos + 1])
    )
    struct regex_parser<Pattern, Pos> : public add_quantifier<Pattern, regex_char_class_parser<Pattern, Pos + 1>> { static constexpr auto begin_pos = Pos; };

    /**
     * @fn
     * @brief �o�b�N�X���b�V���ɂ��A���ꕶ���Ƃ��Ă̋@�\��������(�G�X�P�[�v�ς�)����(�o�b�N�X���b�V���܂߂�2�����̂��߁A�ʒu����i�߂Ă���)
    */
    template <cstr Pattern, std::size_t Pos>
    requires (
        Pattern[Pos] == '\\' && Pattern.size() - 1 > Pos
        && bool(regex_char_attribute::attributes[Pattern[Pos + 1]])
        && !bool(regex_char_attribute::attributes[Pattern[Pos + 1]] & regex_char_attribute::BK)
    )
    struct regex_parser<Pattern, Pos> : public add_quantifier<Pattern, regex_general<Pattern, Pos + 1>> { static constexpr auto begin_pos = Pos; };

    /**
     * @fn
     * @brief �L���v�`���O���[�v�p
    */
    template <cstr Pattern, std::size_t Pos>
    requires (Pattern[Pos] == '(')
    struct regex_parser<Pattern, Pos> : public add_quantifier<Pattern, regex_capture_parser<Pattern, Pos>> {};
}

#endif // TUSTRCPP_INCLUDE_GUARD_REGEX_PERSER_HPP