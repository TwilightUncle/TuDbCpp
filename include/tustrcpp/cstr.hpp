///----------------------------------
/// @file cstr.hpp 
/// @brief �萔���ɒu���镶����
///----------------------------------
#ifndef TUSTRCPP_INCLUDE_GUARD_CSTR_HPP
#define TUSTRCPP_INCLUDE_GUARD_CSTR_HPP

#include <tuutilcpp/utility.hpp>

namespace tustr
{
    /**
     * @class
     * @brief �Œ蒷�̈�̕�����
    */
    template <std::size_t N>
    struct cstr : public std::array<char, N>
    {
    private:
        using parent_type = std::array<char, N>;

    public:
        // �I�[�������i�[�ς݂ł�����̂Ƃ��Ĉ���
        static constexpr auto max_size = N - 1;

        constexpr cstr() : parent_type{} {}
        constexpr cstr(const char (&str_literal)[N])
            : parent_type{}
        {
            for (auto i = 0; i < max_size; i++)
                this->data()[i] = str_literal[i];
        }

        /**
         * @fn
         * @brief string_view���擾
        */
        constexpr auto view() const noexcept { return std::string_view{this->data()}; }
        constexpr operator std::string_view() const noexcept { return this->view(); }
        constexpr operator std::string() const noexcept { return std::string(this->data()); }

        /**
         * @fn
         * @brief �ێ����Ă��镶����̒������擾
        */
        constexpr auto size() const noexcept { return view().size(); }

        /**
         * @fn
         * @brief s�̕����񂪏o�����鐔���J�E���g
        */
        constexpr auto count(const std::string_view& s) const
        {
            const auto sv = this->view();
            std::size_t cnt = 0;
            for (int i = 0; i < sv.size(); i++) {
                if (sv[i] == s[0]) {
                    for (int j = 0; j < s.size() && i + j < sv.size(); j++) {
                        if (sv[i + j] != s[j]) break;
                        if (j == s.size() - 1) cnt++;
                    }
                }
            }
            return cnt;
        }

        /**
         * @fn
         * @brief �����񂪍ŏ��ɏo������ꏊ����������
        */
        constexpr auto find(const std::string_view& s, std::size_t offset = 0) const
        {
            const auto sv = this->view();
            for (std::size_t i = offset; i < sv.size(); i++) {
                if (sv[i] == s[0]) {
                    for (std::size_t j = 0; j < s.size() && i + j < sv.size(); j++) {
                        if (sv[i + j] != s[j]) break;
                        if (j == s.size() - 1) return i;
                    }
                }
            }
            return std::string_view::npos;
        }

        /**
         * @fn
         * @brief �����W�����������A�ŏ��Ɍ����������̈ʒu�ƁA��������A�����č��v�����������̃y�A��Ԃ�
         * @param char_set �����W��
         * @param offset �J�n�ʒu
         * @param is_allow false���w�肷��ƕ����W���Ƀ}�b�`���Ȃ��Ƃ��͈̔͂�Ԃ�
         * @return ������Ȃ��ꍇ�A�ʒu��std::string_view::npos
        */
        constexpr auto get_charset_match_range(const std::string_view& char_set, std::size_t offset = 0, bool is_allow = true) const
        {
            std::size_t s_pos = std::string_view::npos, cnt{};
            for (std::size_t i = 0; i < this->size(); i++) {
                const auto is_find = char_set.find_first_of(this->data()[i]) != std::string_view::npos;
                if (is_allow != is_find) break;
                s_pos = (std::min)(s_pos, i);
                cnt++;
            }
            return std::array{s_pos, cnt};
        }

        /**
         * @fn
         * @brief �����W�����������A�ŏ��Ɍ�����Ȃ����������̈ʒu�ƁA��������A�����č��v�������Ȃ��������̃y�A��Ԃ�
         * @param char_set �����W��
         * @param offset �J�n�ʒu
        */
        constexpr auto get_charset_unmatch_range(const std::string_view& char_set, std::size_t offset = 0) const
        {
            return this->get_charset_match_range(char_set, offset, false);
        }

        /**
         * @fn
         * @brief �S�Ă̗v�f���w�肳�ꂽ�����W���Ɋ܂܂�Ă���Ƃ��^
        */
        constexpr auto match_all_charset(const std::string_view& char_set, bool is_allow = true) const
        {
            auto [s_pos, cnt] = this->get_charset_match_range(char_set, 0, is_allow);
            return s_pos == 0 && cnt == this->size();
        }

        /**
         * @fn
         * @brief �S�Ă̗v�f���w�肵�������W���Ɋ܂܂�Ă��Ȃ��Ƃ��^
        */
        constexpr auto unmatch_all_charset(const std::string_view& char_set) const { return this->match_all_charset(char_set, false); }

        /**
         * @fn
         * @brief ������������擾����
        */
        template <std::size_t Offset, std::size_t Count = max_size>
        requires (Offset <= max_size)
        constexpr auto substr() const
        {
            constexpr auto len = (std::min)(Count, max_size - Offset);
            cstr<len + 1> _s{};
            for (auto i = 0; i < len; i++) _s[i] = this->data()[i + Offset];
            return _s;
        }

        /**
         * @fn
         * @brief �w�蕶�����A������̑O�ォ����
        */
        template <std::size_t Prefix, std::size_t Suffix>
        requires (Prefix + Suffix <= max_size)
        constexpr auto remove_prefix_suffix() const
        {
            return this->substr<Prefix, max_size - Prefix - Suffix>();
        }

        /**
         * @fn
         * @brief �擪��N�������폜����
        */
        template <std::size_t Prefix>
        constexpr auto remove_prefix() const { return this->remove_prefix_suffix<Prefix, 0>(); }

        /**
         * @fn
         * @brief ������N�������폜����
        */
        template <std::size_t Suffix>
        constexpr auto remove_suffix() const { return this->remove_prefix_suffix<0, Suffix>(); }
    };

    /**
     * @fn
     * @brief �������璷��1�̕�������쐬
    */
    inline constexpr auto char_to_cstr(char ch) { return cstr{{ch, '\0'}}; }

    /**
     * @fn
     * @brief ���cstr�^���������������
    */
    template <std::size_t N1, std::size_t N2>
    constexpr auto concat(const cstr<N1>& s1, const cstr<N2>& s2)
    {
        // s1�̏I�[�����͏������邽�߁A-1
        cstr<N1 + N2 - 1> _s{};
        for (int i = 0; i < N1 - 1; i++) _s[i] = s1[i];
        for (int i = 0; i < N2; i++) _s[i + N1 - 1] = s2[i];
        return _s;
    }

    /**
     * @fn
     * @brief n��cstr�^���������Ɍ�������
    */
    template <std::size_t N1, std::size_t N2, std::size_t... Sizes>
    constexpr auto concat(const cstr<N1>& s1, const cstr<N2>& s2, const cstr<Sizes>&... strs)
    {
        return concat(concat(s1, s2), strs...);
    }

    /**
     * @fn
     * @brief Offset����Count�̕���������V�K������Ƃ��Đ؂�o��
    */
    template <std::size_t Offset, std::size_t Count, std::size_t N>
    constexpr auto substr(const cstr<N>& s) { return s.substr<Offset, Count>(); }
    // Offset���當����̍Ō���܂�
    template <std::size_t Offset, std::size_t N>
    constexpr auto substr(const cstr<N>& s) { return s.substr<Offset>(); }

    /**
     * @fn
     * @brief target����؂蕶���ŕ����������ʂ�view�̔z��ŕԋp
    */
    template <cstr target, cstr delimiter>
    constexpr auto split()
    {
        constexpr auto size = target.count(delimiter);
        auto res = std::array<std::string_view, size + 1>{};
        auto sv = target.view();

        for (std::size_t pos{}, prev{}, i{}; (pos = target.find(delimiter, pos)) != std::string_view::npos; prev = pos) {
            res[i++] = sv.substr(0, pos - prev);
            sv = sv.substr((pos += delimiter.size()) - prev);
        }
        res[size] = sv;
        return res;
    }

    /**
     * @fn
     * @brief ���Z�q�I�[�o�[���[�h(��v�B�s��v�͓��o�����̂ŕs�v�B�ׂ����d�l��c++20�̈�є�r�Ŋm�F���邱��)
    */
    template <std::size_t N1, std::size_t N2>
    constexpr bool operator==(const cstr<N1>& s1, const cstr<N2>& s2) { return s1.view() == s2.view(); }

    /**
     * @fn
     * @brief ���Z�q�I�[�o�[���[�h(�F���D�B�ׂ����d�l��c++20�̈�є�r�Ŋm�F���邱��)
    */
    template <std::size_t N1, std::size_t N2>
    constexpr std::strong_ordering operator<=>(const cstr<N1>& s1, const cstr<N2>& s2) { return s1.view() <=> s2.view(); }

    /**
     * @fn
     * @brief �e���v���[�g�����œn���������l��cstr�ɕϊ�����B�i����n�����ƂŁA(2,8,16)�i�����e�����̂悤�ȕ�����ɕϊ�����
    */
    template <auto V, int Hex = 10, bool UsePrefix = false>
    requires (V >= 0 && Hex >= 2 && (std::integral<decltype(V)> || tudb::is_big_int<decltype(V)>::value))
    constexpr auto to_string()
    {
        // 10�i���Ƃ��Č������擾
        constexpr auto len = tudb::get_digit<V, Hex>();
        // �I�[�����̒������z��̗v�f���Ɋ܂߂�
        cstr<len + 1> s{};
        auto val = V;
        for (auto i = len; i > 0; val /= Hex) {
            const auto code = static_cast<char>(val % Hex);
            s[--i] = (code < 10)
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

    /**
     * @fn
     * @brief ������𐮐��^�ɕϊ�
    */
    template <std::integral T>
    constexpr T to_int(const std::string& s)
    {
        T val{};
        for (const auto c: s) {
            val *= 10;
            val += (c - '0');
        }
        return val;
    }
    template <std::integral T>
    constexpr T to_int(const std::string_view& s) { return to_int<T>(std::string(s)); }

    /**
     * @fn
     * @brief cstr�ł��邩�̃��^�֐�
    */
    template <class T> struct is_cstr : public std::false_type{};
    template <std::size_t N> struct is_cstr<cstr<N>> : public std::true_type{};
}

#endif // TUSTRCPP_INCLUDE_GUARD_CSTR_HPP
