///----------------------------------
/// @file regex.hpp 
/// @brief �萔���ɒu���鐳�K�\��(���j�R�[�h���Ή�)
///----------------------------------
#ifndef TUSTRCPP_INCLUDE_GUARD_REGEX_HPP
#define TUSTRCPP_INCLUDE_GUARD_REGEX_HPP

#include <regex>
#include <tustrcpp/cstr.hpp>
#include <tustrcpp/regex/result.hpp>

namespace tustr
{
    /**
     * @brief for��while�ŉ񂹂�悤�ɁA�֐��֕ϊ�����ۂ̌^
    */
    template <std::size_t N>
    using regex_generated_function_type = regex_match_range(std::string_view, std::size_t, bool, regex_capture_store<N>&);
    template <std::size_t N>
    using regex_generated_function_ptr_type = regex_match_range(*)(std::string_view, std::size_t, bool, regex_capture_store<N>&);

    template <class T>
    concept RegexParseable = requires {
        // �ÓI�����o����`�ς݂�
        T::begin_pos;
        T::end_pos;
        // T::generated_func<0>;

        // ��L�����o�̌^�`�F�b�N
        requires std::is_same_v<decltype(T::begin_pos), const std::size_t>;
        requires std::is_same_v<decltype(T::end_pos), const std::size_t>;
        // requires is_regex_generated_function_type<decltype(T::generated_func)>::value; // �֐�

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
        requires std::is_same_v<decltype(T::is_capture), const bool>;
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
#include <tustrcpp/regex/reference.hpp>
#include <tustrcpp/regex/assertion.hpp>
#include <tustrcpp/regex/perser.hpp>

namespace tustr
{
    /**
     * @class
     * @brief ���K�\���i�[�I�u�W�F�N�g�B���I�ɐ������ꂽ�p�^�[���ɂ��Ă͍l�����Ȃ��B�C���X�^���X�ɂ̓p�^�[���}�b�`�̌��ʂ��i�[�����B
     * @tparam Pttern ���K�\���̕����񃊃e�������w��
     * @tparam Parser ���K�\������͂���e���v���[�g�N���X���w��(option)
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
         * @brief �e��͌��ʂɂ�����z�肳���ő�̃L���v�`�������v�Z����
        */
        template <RegexParseable ParserPart>
        static consteval auto get_parser_max_capture_count()
        {
            std::size_t cnt{};
            // �ċA�I��regex����`����Ă���ꍇ
            if constexpr (InnerRegexReferable<ParserPart>)
                cnt += (ParserPart::max_count == std::string_view::npos)
                    ? allowed_max_capture_count
                    : ParserPart::inner_regex::max_capture_count * ParserPart::max_count;
            // ���g���L���v�`��
            if constexpr (RegexParserCaptureable<ParserPart>)
                cnt += (std::min)(allowed_max_capture_count, ParserPart::max_count);
            return cnt;
        }

        /**
         * @fn
         * @brief ���K�\���̉��
        */
        template <std::size_t Pos, std::size_t N, std::size_t MaxCaptureCnt>
        static consteval auto parse()
        {
            // ���炩���߁APattern[Pos]�̒l�ɂ���ĕ������ꉻ���A
            // �Ăяo����鏈�����I�[�o�[���[�h�ł���悤�ɂ���
            using parser = Parser<Pattern, Pos>;

            static_assert(RegexParseable<parser>, "Invaild template argment [Parser]. See concept [RegexParseable].");

            auto [f_arr, capture_cnt] = parse<
                parser::end_pos,
                N + 1,
                (std::min)(MaxCaptureCnt + get_parser_max_capture_count<parser>(), allowed_max_capture_count)
            >();

            f_arr[N] = parser::generated_func;
            return std::pair{f_arr, capture_cnt};
        }

        /**
         * @fn
         * @brief �ċA�̏I�[
        */
        template <std::size_t Pos, std::size_t N, std::size_t MaxCaptureCnt>
        requires (Pattern.size() <= Pos)
        static consteval auto parse()
        {
            // �eperser�ňȉ��֐��|�C���^�Ɋi�[�\�Ȋ֐����`���邱�ƂŁA
            // ��͌��ʂ��֐��|�C���^�̔z��Ƃ��ĕێ��ł���悤�ɂ���
            return std::pair{
                std::array<regex_generated_function_ptr_type<MaxCaptureCnt>, N>{nullptr},
                MaxCaptureCnt
            };
        }

        // ��͂����s���A���߂��i�[
        static constexpr auto parse_result = parse<0, 0, 0>();

    public:
        // ���K�\���̃p�^�[������͂������ʂ̃��[�����֐��W���Ƃ�������
        static constexpr auto& match_rules = parse_result.first;

        // �z�肳���ő�L���v�`����
        static constexpr auto max_capture_count = parse_result.second;

        using capture_store_type = regex_capture_store<max_capture_count>;
        using function_ptr_type = regex_generated_function_ptr_type<max_capture_count>;

        /**
         * @fn
         * @brief �p�^�[���}�b�`�̊�{�B���s��̌��ʂ�Pttern��size�ƂȂ邪�A��v���Ȃ������ꍇ��std::string_view::npos���Ԃ����
        */
        static constexpr auto run(std::string_view s, std::size_t offset = 0, bool is_pos_lock = false)
        {
            auto cs = capture_store_type{};
            auto re = regex_match_range::make_unmatch();
            for(const auto& f : match_rules) {
                const auto part_range = f(s, offset, std::exchange(is_pos_lock, true), cs);
                if (!part_range)
                    return std::pair{cs, regex_match_range::make_unmatch()};
                re.set_begin_pos((std::min)(part_range.get_begin_pos(), re.get_begin_pos()));
                offset = part_range.get_end_pos();
            }
            re.set_end_pos(offset);
            return std::pair{cs, re};
        }

        /**
         * @fn
         * @brief �����̕�������ɁA�p�^�[���}�b�`���镔���������Ă���ΐ^
        */
        static constexpr bool search(std::string_view s) { return bool(run(s, 0).second); }

        /**
         * @fn
         * @brief �����̕�����̑S�̂��p�^�[���}�b�`���Ă���ꍇ�^
        */
        static constexpr bool match(std::string_view s) { return run(s, 0, true).second.match_length() == s.size(); }

    private:
        // ���ʃL���v�`�����X�g���i�[
        const capture_store_type capture_list;
        // ���ʂ̃}�b�`�͈͂��i�[
        const regex_match_range match_range;
        // �e�X�g�Ώ�
        const std::string_view test_target;

        constexpr regex(std::string_view test_target, const std::pair<capture_store_type, regex_match_range>& run_result)
            : capture_list(run_result.first)
            , match_range(run_result.second)
            , test_target(test_target)
        {}

    public:
        /**
         * @fn
         * @brief �p�^�[���}�b�`���s���A�C���X�^���X�ɂ͌��ʂ��i�[����
         * @param test_target �p�^�[���}�b�`�̑Ώە�����
         * @param offset �Ώە�����̃p�^�[���}�b�`���J�n����ʒu
         * @param is_pos_lock �^�̏ꍇ�p�^�[���}�b�`�̈ʒu��offset�ŌŒ肷��Boffset�̈ʒu�����v���Ă��Ȃ��ꍇ��v�Ȃ��ƂȂ�
        */
        constexpr regex(std::string_view test_target, std::size_t offset = 0, bool is_pos_lock = false)
            : regex(test_target, run(test_target, offset, is_pos_lock))
        {}

        /**
         * @fn
         * @brief �����Ώە�����Ƀp�^�[���}�b�`���Ă��镔�������݂�����^
        */
        constexpr bool is_find() const noexcept { return bool(match_range); }

        /**
         * @fn
         * @brief �����Ώۂ̕�����S�̂��p�^�[���}�b�`���Ă���Ƃ��^
        */
        constexpr bool is_match() const noexcept { return match_range.match_length() == test_target.size(); }

        /**
         * @fn
         * @brief �p�^�[���}�b�`���������̕�������擾����B���o���ʂȂ��̏ꍇ�󕶎���ԋp
         * @param index 0���w�肷��ƁA��v���S�́A1�ȏ�̒l��ݒ肷��ƊY������L���v�`���̃}�b�`���擾
        */
        constexpr std::string_view get_match_part(std::size_t index = 0) const
        {
            if (!this->is_find() || this->capture_list.size() < index) return std::string_view{};
            if (!index) return test_target.substr(match_range.get_begin_pos(), match_range.match_length());
            return this->capture_list.get(index - 1);
        }
    };

    using empty_regex = regex<"">;
}

#endif // TUSTRCPP_INCLUDE_GUARD_REGEX_HPP
