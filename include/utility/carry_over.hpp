///----------------------------------
/// @file utility.hpp
/// @brief �񍀂̈������󂯎�鏈���ɑ΂��ČJ��グ������K�p�ł���悤�ȃR���e�i
///----------------------------------
#pragma once
#include <array>

namespace tudb
{
    /**
     * @brief �������n���Ď��s�������ʁA���̃t�H�[�}�b�g�Ō��ʂ��Ԃ� -> pair{array{(T)���オ�肵�Ă��Ȃ�����, (T)���オ�肵������}, (bool)���オ�肵�����ǂ���}
    */
    template <class F, class T>
    concept CarryOverCallable = std::is_invocable_r_v<std::pair<std::array<T, 2>, bool>, F, T, T>;

    template <class T, std::size_t Size>
    struct carry_over_container : std::array<T, Size>
    {
        /**
         * @fn
         * @brief �����^���n���ꂽ�Ƃ�
         * @param f �ړI�̏���
         * @param arg ����
         * @param assign_offset �����̃C���f�b�N�X�ɂ��ꂪ����Ƃ��w��
        */
        template <std::size_t N>
        constexpr carry_over_container run(CarryOverCallable<T> auto f, const carry_over_container<T, N>& arg, const unsigned int assign_offset = 0) const
        {
            return run(f, arg, f, assign_offset);
        }

        /**
         * @fn
         * @brief �����^���n���ꂽ�Ƃ�
         * @param f �ړI�̏���
         * @param arg ����
         * @param carry_over_process ���オ��ɑ΂��Ď��{����鏈��
         * @param assign_offset �����̃C���f�b�N�X�ɂ��ꂪ����Ƃ��w��
        */
        template <std::size_t N>
        constexpr carry_over_container run(
            CarryOverCallable<T> auto f,
            const carry_over_container<T, N>& arg,
            CarryOverCallable<T> auto carry_over_process,
            const unsigned int assign_offset = 0
        ) const {
            auto result = (*this);
            for (unsigned int i = 0; i < (std::min)(Size, N); i++)
                result = result.run(f, arg[i], carry_over_process, i, assign_offset);
            return result;
        }

        /**
         * @fn
         * @brief ���オ��̌v�Z��buf�ɓK�p����(���オ��̌v�Z���A�ŏ��Ɠ����ꍇ �������Z�̂Ђ��Z�݂�����)
         * @param f �ړI�̏���
         * @param arg ����
         * @param offset �v�Z�Ώۂ̃C���f�b�N�X���w��
         * @param assign_offset �����̃C���f�b�N�X�ɂ��ꂪ����Ƃ��w��
        */
        constexpr carry_over_container run(CarryOverCallable<T> auto f, const T& arg, unsigned int offset = 0, const unsigned int assign_offset = 0) const
        {
            return run(f, arg, f, offset, assign_offset);
        }

        /**
         * @fn
         * @brief ���オ��̌v�Z��buf�ɓK�p����(���オ��̌v�Z���A�ŏ��ƈقȂ�ꍇ ���|���Z�̂Ђ��Z�݂�����)
         * @param f �ړI�̏���
         * @param arg ����
         * @param carry_over_process ���オ��ɑ΂��Ď��{����鏈��
         * @param offset �v�Z�Ώۂ̃C���f�b�N�X���w��
         * @param assign_offset �����̃C���f�b�N�X�ɂ��ꂪ����Ƃ��w��
        */
        constexpr carry_over_container run(
            CarryOverCallable<T> auto f,
            const T& arg,
            CarryOverCallable<T> auto carry_over_process,
            const unsigned int offset = 0,
            const unsigned int assign_offset = 0
        ) const {
            auto result = (*this);
            if (offset < Size) {
                if (assign_offset > 0)
                    return run(carry_over_process, arg, offset + assign_offset);

                auto calclated = f(this->data()[offset], arg);
                result[offset] = calclated.first[0];

                for (unsigned int i = offset + 1; i < Size && calclated.second; i++) {
                    calclated = carry_over_process(this->data()[i], calclated.first[1]);
                    result[i] = calclated.first[0];
                }
            }
            return result;
        }

        /**
         * @fn
         * @brief ���g�Ɠ����^�̃I�u�W�F�N�g�ɂ�邽�����|��
        */
        template <std::size_t N>
        constexpr carry_over_container run_all(CarryOverCallable<T> auto f, const carry_over_container<T, N>& arg) const
        {
            return run_all(f, arg, f);
        }

        /**
         * @fn
         * @brief ���g�Ɠ����^�̃I�u�W�F�N�g�ɂ�邽�����|��
        */
        template <std::size_t N>
        constexpr carry_over_container run_all(
            CarryOverCallable<T> auto f,
            const carry_over_container<T, N>& arg,
            CarryOverCallable<T> auto carry_over_process
        ) const {
            auto result = carry_over_container{};
            for (unsigned int i = 0; i < (std::min)(Size, N); i++)
                result = result.run(
                    carry_over_process,
                    run_all(f, arg[i], carry_over_process, i)
                );
            return result;
        }

        /**
         * @fn
         * @brief ���������ׂĂ̗v�f�ɓK�p����
        */
        constexpr carry_over_container run_all(CarryOverCallable<T> auto f, const T& arg, const unsigned int assign_offset = 0) const
        {
            return run_all(f, arg, f, assign_offset);
        }

        /**
         * @fn
         * @brief ���������ׂĂ̗v�f�ɓK�p����
        */
        constexpr carry_over_container run_all(
            CarryOverCallable<T> auto f,
            const T& arg,
            CarryOverCallable<T> auto carry_over_process,
            const unsigned int assign_offset = 0
        ) const {
            auto result = carry_over_container{};
            for (unsigned int i = 0; i < Size; i++) {
                const auto calclated = f(this->data()[i], arg);
                result = result.run(
                    carry_over_process,
                    carry_over_container<T, 2>{calclated.first[0], calclated.first[1]},
                    i + assign_offset
                );
            }
            return result;
        }
    };
}
