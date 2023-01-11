///----------------------------------
/// @file regex/result.hpp 
/// @brief ���ʊi�[�p�I�u�W�F�N�g�Q��`
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_STR_REGEX_RESULT_HPP
#define TUUTILCPP_INCLUDE_GUARD_STR_REGEX_RESULT_HPP

namespace tuutil::str
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
     * @class
     * @brief ���K�\���Ɉ�v�����͈͂��i�[����
    */
    struct regex_match_result : public std::array<std::size_t, 2>
    {
        inline static constexpr auto make_unmatch() { return regex_match_result{std::string_view::npos, 0}; }

        inline constexpr bool is_match() const noexcept { return this->at(0) != std::string_view::npos; }
        inline constexpr explicit operator bool() const noexcept { return this->is_match(); }
        inline constexpr auto get_begin_pos() const noexcept { return this->at(0); }
        inline constexpr auto match_length() const noexcept { return this->at(1); }
        inline constexpr auto get_end_pos() const noexcept { return bool(*this) ? this->at(0) + this->at(1) : std::string_view::npos; }

        inline constexpr void set_begin_pos(std::size_t pos) noexcept { this->at(0) = pos; }
        inline constexpr void set_begin_pos(const regex_match_result& range) noexcept { this->at(0) = range[0]; }
        inline constexpr void set_match_length(std::size_t len) noexcept { this->at(1) = len; }
        inline constexpr void set_match_length(const regex_match_result& range) noexcept { this->at(1) = range[1]; }
        inline constexpr void set_end_pos(std::size_t pos) noexcept
        {
            if (pos == std::string_view::npos) (*this) = make_unmatch();
            else if (this->at(0) > pos) {
                this->at(0) = pos;
                this->at(1) = 0;
            }
            else this->at(1) = pos - this->at(0);
        }
        inline constexpr void set_end_pos(const regex_match_result& range) noexcept { this->set_end_pos(range.get_end_pos()); }
    };
}

#endif // TUUTILCPP_INCLUDE_GUARD_STR_REGEX_RESULT_HPP
