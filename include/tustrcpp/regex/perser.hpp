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
         * @brief �ǂ̋@�\�̉�͂��s���Ă��邩�A�������ꉻ�ŉ���
        */
        template <cstr Pattern, std::size_t Pos>
        struct resolve_regex_parser : public regex_general<Pattern, Pos> {};

        namespace sp
        {
            /**
             * @fn
             * @brief �����N���X�̏ꍇ
            */
            template <cstr Pattern, std::size_t Pos>
            // requires (bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::CLASS))
            struct resolve_regex_parser : public regex_char_class_parser<Pattern, Pos> {};

            /**
             * @fn
             * @brief �����W���̏ꍇ
            */
            template <cstr Pattern, std::size_t Pos>
            requires (bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::CHARSET))
            struct resolve_regex_parser<Pattern, Pos> : public regex_char_set_parser<Pattern, Pos> {};

            /**
             * @fn
             * @brief ����($��^)�̏ꍇ
            */
            template <cstr Pattern, std::size_t Pos>
            requires (bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::ANCHOR))
            struct resolve_regex_parser<Pattern, Pos> : public regex_assertion_parser<Pattern, Pos> {};
        }

        /**
         * @fn
         * @brief �ʏ�̓��ꕶ��
        */
        template <cstr Pattern, std::size_t Pos>
        requires (
            bool(regex_char_attribute::attributes[Pattern[Pos]])
            && !bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::BK)
            && Pattern[Pos] != '\\'
            && Pattern[Pos] != '('
        )
        struct resolve_regex_parser<Pattern, Pos> : public sp::resolve_regex_parser<Pattern, Pos> {};

        namespace br
        {
            /**
             * @fn
             * @brief ���ʂɑ��������ɑ΂��ĉ�������(�f�t�H���g�̓L���v�`��)
            */
            template <cstr Pattern, std::size_t Pos>
            struct resolve_regex_parser : public regex_capture_parser<Pattern, Pos> {};

            /**
             * @fn
             * @brief �O��ǂ݂̌���
            */
            template <cstr Pattern, std::size_t Pos>
            requires (
                exists_in_position("(?=", Pattern, Pos)
                || exists_in_position("(?!", Pattern, Pos)
                || exists_in_position("(?<=", Pattern, Pos)
                || exists_in_position("(?<!", Pattern, Pos)
            )
            struct resolve_regex_parser<Pattern, Pos> : public regex_assertion_parser<Pattern, Pos> {};
        }

        /**
         * @fn
         * @brief �ۊ��ʂ���n�܂�ꍇ
        */
        template <cstr Pattern, std::size_t Pos>
        requires (Pattern[Pos] == '(')
        struct resolve_regex_parser<Pattern, Pos> : public br::resolve_regex_parser<Pattern, Pos> {};

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

            /**
             * @fn
             * @brief �o�b�N�X���b�V������n�܂錾��
            */
            template <cstr Pattern, std::size_t Pos>
            requires (regex_char_attribute::check_attrs_conjuction<regex_char_attribute::ANCHOR, regex_char_attribute::BK>(Pattern[Pos]))
            struct resolve_regex_parser<Pattern, Pos> : public regex_assertion_parser<Pattern, Pos> {};
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
     * @brief ���K�\���p�^�[�����(or�̏ꍇ)
    */
    template <cstr Pattern, std::size_t Pos>
    struct regex_parser
    {
        using parsed_type = regex_or_parser<Pattern, get_or_pos_regex_pattern(Pattern)>;
        static constexpr std::size_t max_capture_count = parsed_type::inner_regex::parser::max_capture_count;

        template <std::size_t MaxCaptureCount>
        static constexpr auto exec(std::string_view subject, std::size_t offset, bool is_pos_lock, regex_capture_store<MaxCaptureCount>& capture_store)
        {
            return parsed_type::template generated_func<MaxCaptureCount>(subject, offset, is_pos_lock, capture_store);
        }
    };

    /**
     * @class
     * @brief ���K�\���p�^�[���̎w����̉�͂��s��(�p���͈ꊇ�œK�p�������֐��������邱�Ƃ�z�肵�āA�E��ݍ��݂Ŏ���)
     * @tparam Pattern ���K�\���p�^�[���̕����񃊃e�������w��
     * @tparam Pos Pattern�̉�͂���J�n�ʒu���w��
    */
    template <cstr Pattern, std::size_t Pos>
    requires (get_or_pos_regex_pattern(Pattern) == std::string_view::npos && Pattern.size() > Pos)
    struct regex_parser<Pattern, Pos>
    {
        // �P�̂Ŏw�肵�Ă͂����Ȃ�����������
        static_assert(
            !(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::DENY),
            "Invalied template argment [Pattern, Pos]. Must not specified of '}', ')', ']'."
        );

        using parsed_type = tudb::foldr_t<
            tudb::quote<tudb::apply>,
            _regex::bind_regex_pattern_t<add_quantifier, Pattern>,
            _regex::resolve_regex_parser<Pattern, Pos>
        >;

        using parsed_next_type = regex_parser<Pattern, parsed_type::end_pos>;

        static constexpr std::size_t max_capture_count = []() {
            constexpr std::size_t allowed_max_capture_count = 65535;
            std::size_t cnt{};
            // �ċA�I��regex����`����Ă���ꍇ
            if constexpr (InnerRegexReferable<parsed_type>)
                cnt += (parsed_type::max_count == std::string_view::npos)
                    ? allowed_max_capture_count
                    : parsed_type::inner_regex::parser::max_capture_count * parsed_type::max_count;
            // ���g���L���v�`��
            if constexpr (RegexParserCaptureable<parsed_type>)
                cnt += (std::min)(allowed_max_capture_count, parsed_type::max_count);
            return (std::min)(allowed_max_capture_count, cnt + parsed_next_type::max_capture_count);
        }();

        template <std::size_t MaxCaptureCount>
        static constexpr auto exec(std::string_view subject, std::size_t offset, bool is_pos_lock, regex_capture_store<MaxCaptureCount>& capture_store)
        {
            std::size_t cnt = 0;
            regex_capture_store<MaxCaptureCount> temp_capture_store = capture_store;
            auto result = regex_match_result::make_unmatch();
            bool is_lock = is_pos_lock;

            for (auto end_pos = offset, begin_pos = std::string_view::npos; end_pos < subject.size() && cnt < parsed_type::max_count;) {
                // ���肪�^�̎��̂ݒl���X�V�����悤�A��U�ꎞ�ϐ��ɍX�V�l���L�^
                auto cs = temp_capture_store;
                const auto temp_result = parsed_type::template generated_func2<MaxCaptureCount>(
                    subject,
                    end_pos,
                    std::exchange(is_lock, true),
                    cs
                );
                if (!temp_result) break;

                // ����ʂ����̂ōX�V
                begin_pos = (std::min)(begin_pos, temp_result.get_begin_pos());
                end_pos = temp_result.get_end_pos();
                temp_capture_store = cs;

                if (++cnt >= parsed_type::min_count) {
                    const auto re = parsed_next_type::exec<MaxCaptureCount>(subject, end_pos, true, temp_capture_store);
                    if (!re) continue;
                    result.set_begin_pos(begin_pos);
                    result.set_end_pos(re);
                    capture_store = temp_capture_store;
                    if constexpr (parsed_type::negative) break;
                }
            }

            // 0��}�b�`�ł�OK�̏ꍇ
            if (!cnt && !parsed_type::min_count)
                result = parsed_next_type::exec<MaxCaptureCount>(subject, offset, is_pos_lock, capture_store);

            return (cnt < parsed_type::min_count)
                    ? regex_match_result::make_unmatch()
                    : result;
        }
    };

    /**
     * @class
     * @brief �ċA��`�̏I�_
    */
    template <cstr Pattern, std::size_t Pos>
    requires (Pattern.size() <= Pos)
    struct regex_parser<Pattern, Pos>
    {
        static constexpr std::size_t max_capture_count = 0;

        template <std::size_t MaxCaptureCount>
        static constexpr auto exec(std::string_view subject, std::size_t offset, bool is_pos_lock, regex_capture_store<MaxCaptureCount>&)
        {
            return regex_match_result{offset, 0};
        }
    };
}

#endif // TUSTRCPP_INCLUDE_GUARD_REGEX_PERSER_HPP
