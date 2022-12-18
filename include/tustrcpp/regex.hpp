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
#include <tustrcpp/regex/general.hpp>

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
     * @fn
     * @brief �ǂ̋@�\�̉�͂��s���Ă��邩�A�������ꉻ�ŏ�������B�f�t�H���g
    */
    template <cstr Pattern, std::size_t Pos>
    struct regex_parser : public regex_general<Pattern, Pos> {};

    /**
     * @fn
     * @brief �����W���̏ꍇ
    */
    template <cstr Pattern, std::size_t Pos>
    requires (bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::CHARSET))
    struct regex_parser<Pattern, Pos> : public regex_char_set_parser<Pattern, Pos> {};

    /**
     * @fn
     * @brief �����N���X�̏ꍇ
    */
    template <cstr Pattern, std::size_t Pos>
    requires (
        bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::CLASS)
        && !bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::BK)
    )
    struct regex_parser<Pattern, Pos> : public regex_char_class_parser<Pattern, Pos> {};

    /**
     * @fn
     * @brief �����N���X�̏ꍇ2(�o�b�N�X���b�V���܂߂�2�����̂��߁A�ʒu����i�߂Ă���)
    */
    template <cstr Pattern, std::size_t Pos>
    requires (
        Pattern[Pos] == '\\' && Pattern.size() - 1 > Pos
        && regex_char_attribute::check_attrs_conjuction<regex_char_attribute::CLASS, regex_char_attribute::BK>(Pattern[Pos])
    )
    struct regex_parser<Pattern, Pos> : public regex_char_class_parser<Pattern, Pos + 1> { static constexpr auto begin_pos = Pos; };

    /**
     * @fn
     * @brief �o�b�N�X���b�V���ɂ��A���ꕶ���Ƃ��Ă̋@�\��������(�G�X�P�[�v�ς�)����(�o�b�N�X���b�V���܂߂�2�����̂��߁A�ʒu����i�߂Ă���)
    */
    template <cstr Pattern, std::size_t Pos>
    requires (
        Pattern[Pos] == '\\' && Pattern.size() - 1 > Pos
        && bool(regex_char_attribute::attributes[Pattern[Pos]])
        && !bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::BK)
    )
    struct regex_parser<Pattern, Pos> : public regex_general<Pattern, Pos + 1> { static constexpr auto begin_pos = Pos; };

    /**
     * @brief for��while�ŉ񂹂�悤�ɁA�֐��֕ϊ�����ۂ̌^
    */
    using regex_generated_function_type = std::size_t(const std::string_view&, std::size_t, bool);
    using regex_generated_function_ptr_type = std::size_t(*)(const std::string_view&, std::size_t, bool);

    template <class T>
    concept RegexParseable = requires {
        // �ÓI�����o����`�ς݂�
        T::begin_pos;
        T::end_pos;
        T::generated_func;

        // ��L�����o�̌^�`�F�b�N
        requires std::is_same_v<decltype(T::begin_pos), const std::size_t>;
        requires std::is_same_v<decltype(T::end_pos), const std::size_t>;
        requires std::is_same_v<decltype(T::generated_func), regex_generated_function_type>;

        // �ᔽ���Ă���ꍇ�A�����ċA���������Ă��܂�
        requires T::begin_pos < T::end_pos;
    };

    /**
     * @class
     * @brief ���K�\���i�[�I�u�W�F�N�g�B���I�ɐ������ꂽ�p�^�[���ɂ��Ă͍l�����Ȃ�
     * @tparam Pttern ���K�\���̕����񃊃e�������w��
     * @tparam Parser ���K�\������͂���e���v���[�g�N���X���w��
    */
    template <cstr Pattern, template <cstr, std::size_t> class Parser = regex_parser>
    struct regex
    {
    private:
        // ���@�エ�������o�b�N�X���b�V���̏o���͂����Ō��o�A�G���[�Ƃ���
        static_assert(is_collect_regex_back_slash(Pattern.view()));

        /**
         * @fn
         * @brief ���K�\���̉�́B���ꕶ�����܂܂Ȃ����̒S��
        */
        template <std::size_t Pos, std::size_t N>
        static consteval auto parse()
        {
            // ���炩���߁APattern[Pos]�̒l�ɂ���ĕ������ꉻ���A
            // �Ăяo����鏈�����I�[�o�[���[�h�ł���悤�ɂ���
            using parser = Parser<Pattern, Pos>;

            static_assert(RegexParseable<parser>, "Invaild template argment [Parser]. See concept [RegexParseable].");

            auto parse_result = parse<parser::end_pos, N + 1>();
            parse_result[N] = parser::generated_func;
            return parse_result;
        }

        /**
         * @fn
         * @brief �ċA�̏I�[
        */
        template <std::size_t Pos, std::size_t N>
        requires (Pattern.size() <= Pos)
        static consteval auto parse()
        {
            // �eperser�ňȉ��֐��|�C���^�Ɋi�[�\�Ȋ֐����`���邱�ƂŁA
            // ��͌��ʂ��֐��|�C���^�̔z��Ƃ��ĕێ��ł���悤�ɂ���
            return std::array<regex_generated_function_ptr_type, N>{nullptr};
        }

    public:
        static constexpr auto parse_result = parse<0, 0>();

        /**
         * @fn
         * @brief �p�^�[���}�b�`�̊�{�B���s��̌��ʂ�Pttern��size�ƂȂ邪�A��v���Ȃ������ꍇ��std::string_view::npos���Ԃ����
        */
        static constexpr std::size_t run(const std::string_view& s, std::size_t offset = 0)
        {
            bool is_pos_lock = false;
            for(regex_generated_function_ptr_type before = nullptr; const auto& f : parse_result) {
                if ((offset = f(s, offset, std::exchange(is_pos_lock, true))) == std::string_view::npos) return offset;
                before = f;
            }
            return offset;
        }

        static constexpr bool match(const std::string_view& s) { return run(s, 0) != std::string_view::npos; }
    };

    using empty_regex = regex<"">;
}

#endif // TUSTRCPP_INCLUDE_GUARD_REGEX_HPP
