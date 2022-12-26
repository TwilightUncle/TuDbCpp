///----------------------------------
/// @file perser.hpp 
/// @brief regex.hpp�֘A(�p�^�[����͋@�\����̃N���X�ɏW�ς�������)
///----------------------------------
#ifndef TUSTRCPP_INCLUDE_GUARD_REGEX_PERSER_HPP
#define TUSTRCPP_INCLUDE_GUARD_REGEX_PERSER_HPP

namespace tustr
{
    namespace _regex
    {
        /**
         * @fn
         * @brief �ǂ̋@�\�̉�͂��s���Ă��邩�A�������ꉻ�Ŗ��O����
        */
        template <cstr Pattern, std::size_t Pos>
        struct resolve_regex_parser : public regex_general<Pattern, Pos> {};

        /**
         * @fn
         * @brief �����W���̏ꍇ
        */
        template <cstr Pattern, std::size_t Pos>
        requires (bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::CHARSET))
        struct resolve_regex_parser<Pattern, Pos> : public regex_char_set_parser<Pattern, Pos> {};

        /**
         * @fn
         * @brief �����N���X�̏ꍇ
        */
        template <cstr Pattern, std::size_t Pos>
        requires (bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::CLASS))
        struct resolve_regex_parser<Pattern, Pos> : public regex_char_class_parser<Pattern, Pos> {};

        /**
         * @fn
         * @brief �L���v�`���O���[�v�p
        */
        template <cstr Pattern, std::size_t Pos>
        requires (Pattern[Pos] == '(')
        struct resolve_regex_parser<Pattern, Pos> : public regex_capture_parser<Pattern, Pos> {};

        namespace bk
        {
            /**
             * @fn
             * @brief �o�b�N�X���b�V���ɑ��������ɑ΂��ĉ�������(�f�t�H���g�̓G�X�P�[�v)
            */
            template <cstr Pattern, std::size_t Pos>
            struct resolve_regex_parser : public regex_general<Pattern, Pos> {};

            /**
             * @fn
             * @brief �L���v�`���Q��
            */
            template <cstr Pattern, std::size_t Pos>
            requires (bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::REFERENCE))
            struct resolve_regex_parser<Pattern, Pos> : public regex_reference_parser<Pattern, Pos> {};

            /**
             * @fn
             * @brief �o�b�N�X���b�V������n�܂镶���N���X
            */
            template <cstr Pattern, std::size_t Pos>
            requires (regex_char_attribute::check_attrs_conjuction<regex_char_attribute::CLASS, regex_char_attribute::BK>(Pattern[Pos]))
            struct resolve_regex_parser<Pattern, Pos> : public regex_char_class_parser<Pattern, Pos> {};
        }

        /**
         * @fn
         * @brief �o�b�N�X���b�V������n�܂�ꍇ�̋N�_(�����Ńo�b�N�X���b�V���𗘗p����]���͏I���̂ŁA���̕����ɐi��)
        */
        template <cstr Pattern, std::size_t Pos>
        requires (Pattern[Pos] == '\\' && Pattern.size() - 1 > Pos)
        struct resolve_regex_parser<Pattern, Pos> : public bk::resolve_regex_parser<Pattern, Pos + 1> { static constexpr auto begin_pos = Pos; };

        /**
         * @fn
         * @brief �擪�̐��K�\���p�^�[����K�p�ς݂Ƃ���
        */
        template <template <cstr, RegexParseable> class F, cstr Pattern>
        struct bind_regex_pattern
        {
            template <RegexParseable T>
            struct apply : public F<Pattern, T> {};
            using type = tudb::quote<apply>;
        };

        template <template <cstr, RegexParseable> class F, cstr Pattern> using bind_regex_pattern_t = bind_regex_pattern<F, Pattern>::type;
    }

    /**
     * @class
     * @brief ���K�\���p�^�[���̎w����̉�͂��s��(�p���͈ꊇ�œK�p�������֐��������邱�Ƃ�z�肵�āA�E��ݍ��݂Ŏ���)
     * @tparam Pattern ���K�\���p�^�[���̕����񃊃e�������w��
     * @tparam Pos Pattern�̉�͂���J�n�ʒu���w��
    */
    template <cstr Pattern, std::size_t Pos>
    struct regex_parser : public tudb::foldr_t<
        tudb::quote<tudb::apply>,
        _regex::bind_regex_pattern_t<add_quantifier, Pattern>,
        _regex::resolve_regex_parser<Pattern, Pos>
    > {
        // �P�̂Ŏw�肵�Ă͂����Ȃ�����������
        static_assert(
            !(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::DENY),
            "Invalied template argment [Pattern, Pos]. Must not specified of '}', ')', ']'."
        );
    };
}

#endif // TUSTRCPP_INCLUDE_GUARD_REGEX_PERSER_HPP
