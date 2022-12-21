///----------------------------------
/// @file regex.hpp 
/// @brief �萔���ɒu���鐳�K�\��(���j�R�[�h���Ή�)
///----------------------------------
#ifndef TUSTRCPP_INCLUDE_GUARD_REGEX_HPP
#define TUSTRCPP_INCLUDE_GUARD_REGEX_HPP

#include <regex>
#include <tustrcpp/cstr.hpp>

namespace tustr
{
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
        requires std::is_same_v<decltype(T::generated_func), regex_generated_function_type>; // �֐�

        // �ᔽ���Ă���ꍇ�A�����ċA���������Ă��܂�
        requires T::begin_pos < T::end_pos;
    };
    
    // �O���錾
    template <cstr Pattern, std::size_t Pos> struct regex_parser;
    template <cstr Pattern, template <cstr, std::size_t> class Parser> struct regex;

    template <class T> struct is_regex : public std::false_type {};
    template <cstr Pattern, template <cstr, std::size_t> class Parser>
    struct is_regex<regex<Pattern, Parser>> : public std::true_type {};

    // �ċA�I�ɐ��K�\���N���X�����Ă��邩
    template <class T>
    concept InnerRegexReferable = requires {
        // �L���v�`���O���[�v�����ʓ����ċA�I�ɉ�͂��Ă���ꍇ
        typename T::inner_regex;
        requires is_regex<typename T::inner_regex>::value;
    };

    // �L���v�`�����s�����̂�
    template <class T>
    concept RegexParserCaptureable = requires {
        T::is_capture;
        requires std::is_same_v<decltype(T::is_capture), bool>;
        requires T::is_capture;
    };
}

#include <tustrcpp/regex/common.hpp>
#include <tustrcpp/regex/bracket.hpp>
#include <tustrcpp/regex/char_class.hpp>
#include <tustrcpp/regex/quantifier.hpp>
#include <tustrcpp/regex/char_set.hpp>
#include <tustrcpp/regex/capture.hpp>
#include <tustrcpp/regex/general.hpp>
#include <tustrcpp/regex/perser.hpp>

namespace tustr
{
    /**
     * @fn
     * @brief �n���ꂽ�����͈͂�����/���ۃ��X�g�ɓW�J���A�^����ꂽ�����񂪃}�b�`���邩���肷��֐��I�u�W�F�N�g��ԋp����
     * TODO: ����Ȃ��̂ł����e�X�g�̏C���Ɩ{�֐����폜����
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
     * @tparam Pttern ���K�\���̕����񃊃e�������w��
     * @tparam Parser ���K�\������͂���e���v���[�g�N���X���w��
    */
    template <cstr Pattern, template <cstr, std::size_t> class Parser = regex_parser>
    struct regex
    {
        // �L���v�`���\�ȍő吔(�ÓI�Ɋi�[���ʂ̃L���v�`���̈���m�ۂ��������߁A�����݂���)
        static constexpr std::size_t allowed_max_capture_count = 65535;
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

            auto [f_arr, capture_cnt] = parse<parser::end_pos, N + 1>();
            f_arr[N] = parser::generated_func;

            // ���ʂ�ۑ����Ȃ���΂Ȃ�Ȃ��L���v�`�������݂���ꍇ���Z
            if constexpr (InnerRegexReferable<parser>)
                capture_cnt = (std::max)(
                    capture_cnt + parser::inner_regex::max_capture_count * parser::max_count,
                    (std::max)(parser::max_count, capture_cnt)
                );
            if constexpr (RegexParserCaptureable<parser>)
                capture_cnt = (std::max)(
                    capture_cnt + parser::max_count,
                    (std::max)(parser::max_count, capture_cnt)
                );

            return std::pair{f_arr, (std::min)(capture_cnt, allowed_max_capture_count)};
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
            return std::pair{
                std::array<regex_generated_function_ptr_type, N>{nullptr},
                std::size_t{}
            };
        }
        static constexpr auto parse_result = parse<0, 0>();

    public:
        // ���K�\���̃p�^�[������͂������ʂ̃��[�����֐��W���Ƃ�������
        static constexpr auto& match_rules = parse_result.first;

        // �z�肳���ő�L���v�`����
        static constexpr auto max_capture_count = parse_result.second;

        /**
         * @fn
         * @brief �p�^�[���}�b�`�̊�{�B���s��̌��ʂ�Pttern��size�ƂȂ邪�A��v���Ȃ������ꍇ��std::string_view::npos���Ԃ����
        */
        static constexpr std::size_t run(const std::string_view& s, std::size_t offset = 0, bool is_pos_lock = false)
        {
            for(regex_generated_function_ptr_type before = nullptr; const auto& f : match_rules) {
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
