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
     * @class
     * @brief �L���v�`�����ʂ̕ۑ��Ǝ擾���s���I�u�W�F�N�g�B
    */
    template <std::size_t N>
    class regex_capture_store
    {
    private:
        std::array<std::string_view, N> buf{};
        std::size_t end_pos{};
    
    public:
        constexpr regex_capture_store() noexcept {}

        /**
         * @fn
         * @brief �L���v�`�����ʂ��i�[
        */
        constexpr void push_back(std::string_view sv)
        {
            if (this->end_pos >= N) throw std::out_of_range(std::string("orver max size. max value is ") + std::to_string(N));
            this->buf[this->end_pos++] = sv;
        }

        template <std::size_t M>
        constexpr void push_back(const regex_capture_store<M>& cs)
        {
            for (auto i = std::size_t{}; this->end_pos < N && i < cs.size(); i++)
                this->push_back(cs.get(i));
        }

        /**
         * @fn
         * @brief �w��C���f�b�N�X�Ɋi�[���ꂽ�L���v�`�����e�����o��
        */
        constexpr auto get(std::size_t index) const
        {
            // �l�����i�[�̗̈�ւ̃A�N�Z�X�͋֎~����
            if (index >= this->end_pos)
                throw std::out_of_range(
                    std::string("index exceeds maximum allowed value. size: ")
                    + std::to_string(this->end_pos) + ", specify index: " + std::to_string(index)
                );
            return this->buf[index];
        }

        constexpr auto front() const noexcept
        {
            static_assert(N, "don't call function. because [regex_capture_store<0>] max size is 0.");
            return this->buf[0];
        }
        constexpr auto back() const noexcept
        {
            static_assert(N, "don't call function. because [regex_capture_store<0>] max size is 0.");
            return this->buf[this->end_pos - 1];
        }

        constexpr bool empty() const noexcept { return !bool(this->end_pos); }
        constexpr std::size_t size() const noexcept { return this->end_pos; }
    };

    /**
     * @brief for��while�ŉ񂹂�悤�ɁA�֐��֕ϊ�����ۂ̌^
    */
    template <std::size_t N>
    using regex_generated_function_type = std::size_t(std::string_view, std::size_t, bool, regex_capture_store<N>&);
    template <std::size_t N>
    using regex_generated_function_ptr_type = std::size_t(*)(std::string_view, std::size_t, bool, regex_capture_store<N>&);

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
            for(function_ptr_type before = nullptr; const auto& f : match_rules) {
                if ((offset = f(s, offset, std::exchange(is_pos_lock, true), cs)) == std::string_view::npos)
                    return std::pair{cs, offset};
                before = f;
            }
            return std::pair{cs, offset};
        }

        /**
         * @fn
         * @brief �����̕�������ɁA�p�^�[���}�b�`���镔���������Ă���ΐ^
        */
        static constexpr bool search(std::string_view s) { return run(s, 0).second != std::string_view::npos; }

        /**
         * @fn
         * @brief �����̕�����̑S�̂��p�^�[���}�b�`���Ă���ꍇ�^
        */
        static constexpr bool match(std::string_view s) { return run(s, 0, true).second == s.size(); }
    };

    using empty_regex = regex<"">;
}

#endif // TUSTRCPP_INCLUDE_GUARD_REGEX_HPP
