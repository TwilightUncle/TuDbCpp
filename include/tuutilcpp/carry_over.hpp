///----------------------------------
/// @file carry_over.hpp
/// @brief �񍀂̈������󂯎�鏈���ɑ΂��ČJ��グ������K�p�ł���悤�ȃR���e�i
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_CARRY_OVER_HPP
#define TUUTILCPP_INCLUDE_GUARD_CARRY_OVER_HPP


namespace tudb
{
    /**
     * @brief �������n���Ď��s�������ʁA���̃t�H�[�}�b�g�Ō��ʂ��Ԃ� -> tuple{(T)���オ�肵�Ă��Ȃ�����, (T)���オ�肵������, (bool)���オ�肵�����ǂ���}
    */
    template <class F, class T>
    concept CarryOverCallable = std::is_invocable_r_v<std::tuple<T, T, bool>, F, T, T>;

    template <class T, std::size_t Size>
    struct carry_over_container : std::array<T, Size>
    {
        /**
         * @fn
         * @brief run�����o�̖߂�l����A�p����ł̌^�L���X�g���߂�ǂ��������Ȃ̂ŗp��
        */
        constexpr void set(const carry_over_container& v) { (*this) = v; }

        /**
         * @fn
         * @brief �����^���n���ꂽ�Ƃ�
         * @param f �ړI�̏���
         * @param arg ����
         * @param offset �����̃C���f�b�N�X�ɂ��ꂪ����Ƃ��w��
        */
        template <std::size_t N, std::convertible_to<T> Type>
        constexpr carry_over_container with_carry_up(CarryOverCallable<T> auto f, const carry_over_container<Type, N>& arg, const int offset = 0) const
        {
            return with_carry_up(f, arg, f, offset);
        }

        /**
         * @fn
         * @brief �����^���n���ꂽ�Ƃ�
         * @param f �ړI�̏���
         * @param arg ����
         * @param carry_over_process ���オ��ɑ΂��Ď��{����鏈��
         * @param offset �����̃C���f�b�N�X�ɂ��ꂪ����Ƃ��w��
        */
        template <std::size_t N, std::convertible_to<T> Type>
        constexpr carry_over_container with_carry_up(
            CarryOverCallable<T> auto f,
            const carry_over_container<Type, N>& arg,
            CarryOverCallable<T> auto carry_over_process,
            const int offset = 0
        ) const {
            auto result = (*this);
            for (unsigned int i = 0; i < (std::min)(Size, N); i++)
                result = result.with_carry_up(f, arg[i], carry_over_process, i + offset);
            return result;
        }

        /**
         * @fn
         * @brief ���オ��̌v�Z��buf�ɓK�p����(���オ��̌v�Z���A�ŏ��Ɠ����ꍇ �������Z�̂Ђ��Z�݂�����)
         * @param f �ړI�̏���
         * @param arg ����
         * @param offset �v�Z�Ώۂ̃C���f�b�N�X���w��
         * @param offset �����̃C���f�b�N�X�ɂ��ꂪ����Ƃ��w��
        */
        constexpr carry_over_container with_carry_up(CarryOverCallable<T> auto f, const T& arg, unsigned int offset = 0) const
        {
            return with_carry_up(f, arg, f, offset);
        }

        /**
         * @fn
         * @brief ���オ��̌v�Z��buf�ɓK�p����(���オ��̌v�Z���A�ŏ��ƈقȂ�ꍇ ���|���Z�̂Ђ��Z�݂�����)
         * @param f �ړI�̏���
         * @param arg ����
         * @param carry_over_process ���オ��ɑ΂��Ď��{����鏈��
         * @param offset �v�Z�Ώۂ̃C���f�b�N�X���w��
         * @param offset �����̃C���f�b�N�X�ɂ��ꂪ����Ƃ��w��
        */
        constexpr carry_over_container with_carry_up(
            CarryOverCallable<T> auto f,
            const T& arg,
            CarryOverCallable<T> auto carry_over_process,
            const int offset = 0
        ) const {
            auto result = (*this);
            if (offset < Size && offset >= 0) {
                auto [lower, upper, is_carry] = f(this->at(offset), arg);
                result[offset] = lower;

                for (unsigned int i = offset + 1; i < Size && is_carry; i++)
                    std::tie(result[i], upper, is_carry) = carry_over_process(this->at(i), upper);
            }
            return result;
        }

        /**
         * @fn
         * @brief ���g�Ɠ����^�̃I�u�W�F�N�g�ɂ�邽�����|��
        */
        template <std::size_t N, std::convertible_to<T> Type>
        constexpr carry_over_container with_carry_up_all(CarryOverCallable<T> auto f, const carry_over_container<Type, N>& arg) const
        {
            return with_carry_up_all(f, arg, f);
        }

        /**
         * @fn
         * @brief ���g�Ɠ����^�̃I�u�W�F�N�g�ɂ�邽�����|��
        */
        template <std::size_t N, std::convertible_to<T> Type>
        constexpr carry_over_container with_carry_up_all(
            CarryOverCallable<T> auto f,
            const carry_over_container<Type, N>& arg,
            CarryOverCallable<T> auto carry_over_process
        ) const {
            auto result = carry_over_container{};
            for (unsigned int i = 0; i < (std::min)(Size, N); i++)
                result = result.with_carry_up(
                    carry_over_process,
                    with_carry_up_all(f, arg[i], carry_over_process, i)
                );
            return result;
        }

        /**
         * @fn
         * @brief ���������ׂĂ̗v�f�ɓK�p����
        */
        constexpr carry_over_container with_carry_up_all(CarryOverCallable<T> auto f, const T& arg, const int offset = 0) const
        {
            return with_carry_up_all(f, arg, f, offset);
        }

        /**
         * @fn
         * @brief ���������ׂĂ̗v�f�ɓK�p����
        */
        constexpr carry_over_container with_carry_up_all(
            CarryOverCallable<T> auto f,
            const T& arg,
            CarryOverCallable<T> auto carry_over_process,
            const int offset = 0
        ) const {
            auto result = carry_over_container{};
            for (unsigned int i = 0; i < Size; i++) {
                const auto [lower, upper, is_carry] = f(this->at(i), arg);
                result = result.with_carry_up(
                    carry_over_process,
                    carry_over_container<T, 2>{lower, upper},
                    i + offset
                );
            }
            return result;
        }

        template <class Derived, class... ArgTypes>
        requires std::is_base_of_v<carry_over_container, Derived>
        constexpr Derived& set_with_carry_up(const ArgTypes&... args)
        {
            this->set(this->with_carry_up(args...));
            return static_cast<Derived&>(*this);
        }

        template <class Derived, class... ArgTypes>
        requires std::is_base_of_v<carry_over_container, Derived>
        constexpr Derived& set_with_carry_up_all(const ArgTypes&... args)
        {
            this->set(this->with_carry_up_all(args...));
            return static_cast<Derived&>(*this);
        }
    };

    /**
     * @fn
     * @brief �����^���i�[���Ă�����̂ɑ΂��āA��ʗv�f��0�𖢎g�p�Ƃ݂Ȃ��A�g�p���̗v�f���𐔂���B�S��0�̏ꍇ��1
    */
    template <std::unsigned_integral T, std::size_t Size>
    constexpr auto count_using_size(const carry_over_container<T, Size>& v) noexcept
    {
        auto result = Size;
        while (!v[(--result)] && result > 0);
        return result + 1;
    }

    /**
     * @fn
     * @brief �����^���i�[����carry_over_container�����̐��x������Ȃ��悤��ToBufT�̃T�C�Y�̐����^��carry_over_container�֕ϊ�����
    */
    template <std::unsigned_integral ToBufT, std::unsigned_integral FromBufT, std::size_t Size>
    constexpr auto convert_diff_size_buffer(const carry_over_container<FromBufT, Size>& from) noexcept
    {
        constexpr auto from_buffer_digits = std::numeric_limits<FromBufT>::digits;
        constexpr auto from_all_digits = from_buffer_digits * Size;
        constexpr auto to_buffer_digits = std::numeric_limits<ToBufT>::digits;
        constexpr auto to_buffer_size = from_all_digits / to_buffer_digits + (bool)(from_all_digits % to_buffer_digits);
        using larger_type = std::conditional_t<(from_buffer_digits > to_buffer_digits), FromBufT, ToBufT>;

        auto to =  carry_over_container<ToBufT, to_buffer_size>{};
        for (auto dig = 0; dig < from_all_digits; dig += (std::min)(from_buffer_digits, to_buffer_digits)) {
            const auto to_i = dig / to_buffer_digits;
            const auto from_i = dig / from_buffer_digits;
            const auto diff_digits = (int)(to_i * to_buffer_digits) - (int)(from_i * from_buffer_digits);
            // �V�t�g�O�ɑ傫���ق��̃T�C�Y�̌^�ɓ���(�������܂܂��ƁA�V�t�g�������ɑ΂��đ������A�R���p�C���ł��Ȃ��ꍇ������)
            to[dig / to_buffer_digits] |= ((larger_type)from[dig / from_buffer_digits] >> (std::max)(0, diff_digits)) << (std::max)(0, -diff_digits);
        }
        return to;
    }
    template <std::unsigned_integral ToBufT, std::unsigned_integral FromBufT>
    constexpr auto convert_diff_size_buffer(const FromBufT& from) noexcept
    {
        return convert_diff_size_buffer<ToBufT>(carry_over_container<FromBufT, 1>{from});
    }
}

#endif // TUUTILCPP_INCLUDE_GUARD_CARRY_OVER_HPP
