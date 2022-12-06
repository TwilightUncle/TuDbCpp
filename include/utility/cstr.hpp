#pragma once

namespace tudb
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

        /**
         * @fn
         * @brief �ێ����Ă��镶����̒������擾
        */
        constexpr auto size() const noexcept { return view().size(); }
    };

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
    requires (Offset < cstr<N>::max_size)
    constexpr auto substr(const cstr<N>& s)
    {
        constexpr auto len = (std::min)(Count, cstr<N>::max_size - Offset);
        cstr<len + 1> _s{};
        for (auto i = 0; i < len; i++) _s[i] = s[i + Offset];
        return _s;
    }
    // Offset���當����̍Ō���܂�
    template <std::size_t Offset, std::size_t N>
    constexpr auto substr(const cstr<N>& s) { return substr<Offset, cstr<N>::max_size - Offset, N>(s); }

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
        constexpr auto len = get_digit<V, Hex>();
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
     * @brief cstr�ł��邩�̃��^�֐�
    */
    template <class T> struct is_cstr : public std::false_type{};
    template <std::size_t N> struct is_cstr<cstr<N>> : public std::true_type{};
}
