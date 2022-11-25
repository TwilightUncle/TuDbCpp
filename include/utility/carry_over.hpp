///----------------------------------
/// @file carry_over.hpp
/// @brief 二項の引数を受け取る処理に対して繰り上げ処理を適用できるようなコンテナ
///----------------------------------
#pragma once

namespace tudb
{
    /**
     * @brief 二引数を渡して実行した結果、次のフォーマットで結果が返る -> tuple{(T)桁上がりしていない部分, (T)桁上がりした部分, (bool)桁上がりしたかどうか}
    */
    template <class F, class T>
    concept CarryOverCallable = std::is_invocable_r_v<std::tuple<T, T, bool>, F, T, T>;

    template <class T, std::size_t Size>
    struct carry_over_container : std::array<T, Size>
    {
        /**
         * @fn
         * @brief runメンバの戻り値から、継承先での型キャストがめんどくさそうなので用意
        */
        constexpr void set(const carry_over_container& v) { (*this) = v; }

        /**
         * @fn
         * @brief 同じ型が渡されたとき
         * @param f 目的の処理
         * @param arg 引数
         * @param offset 代入先のインデックスにずれがあるとき指定
        */
        template <std::size_t N, std::convertible_to<T> Type>
        constexpr carry_over_container with_carry_up(CarryOverCallable<T> auto f, const carry_over_container<Type, N>& arg, const int offset = 0) const
        {
            return with_carry_up(f, arg, f, offset);
        }

        /**
         * @fn
         * @brief 同じ型が渡されたとき
         * @param f 目的の処理
         * @param arg 引数
         * @param carry_over_process 桁上がりに対して実施される処理
         * @param offset 代入先のインデックスにずれがあるとき指定
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
         * @brief 桁上がりの計算をbufに適用する(桁上がりの計算が、最初と同じ場合 ※足し算のひっ算みたいな)
         * @param f 目的の処理
         * @param arg 引数
         * @param offset 計算対象のインデックスを指定
         * @param offset 代入先のインデックスにずれがあるとき指定
        */
        constexpr carry_over_container with_carry_up(CarryOverCallable<T> auto f, const T& arg, unsigned int offset = 0) const
        {
            return with_carry_up(f, arg, f, offset);
        }

        /**
         * @fn
         * @brief 桁上がりの計算をbufに適用する(桁上がりの計算が、最初と異なる場合 ※掛け算のひっ算みたいな)
         * @param f 目的の処理
         * @param arg 引数
         * @param carry_over_process 桁上がりに対して実施される処理
         * @param offset 計算対象のインデックスを指定
         * @param offset 代入先のインデックスにずれがあるとき指定
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
         * @brief 自身と同じ型のオブジェクトによるたすき掛け
        */
        template <std::size_t N, std::convertible_to<T> Type>
        constexpr carry_over_container with_carry_up_all(CarryOverCallable<T> auto f, const carry_over_container<Type, N>& arg) const
        {
            return with_carry_up_all(f, arg, f);
        }

        /**
         * @fn
         * @brief 自身と同じ型のオブジェクトによるたすき掛け
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
         * @brief 引数をすべての要素に適用する
        */
        constexpr carry_over_container with_carry_up_all(CarryOverCallable<T> auto f, const T& arg, const int offset = 0) const
        {
            return with_carry_up_all(f, arg, f, offset);
        }

        /**
         * @fn
         * @brief 引数をすべての要素に適用する
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
}
