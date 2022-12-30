///----------------------------------
/// @file algorithm.hpp 
/// @brief �ėp���^�֐���`
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_ALGORITHM_HPP
#define TUUTILCPP_INCLUDE_GUARD_ALGORITHM_HPP

namespace tudb
{
    // type_list�֘A�̑O���錾
    struct ignore_type;
    template <class... Types> struct type_list;
    template <class T> struct pass_types;
    template <class T1, class T2> struct push_type;

    /**
     * @fn
     * @brief ���^�֐������̉��ς݂̌^�Ɋۂ߂�
    */
    template <template <class...> class F> struct quote {};

    /**
     * @fn
     * @brief �e���v���[�g�p�����[�^�̕����K�p
    */
    template <class RoundedF, class... PartialArgs> struct bind;
    template <template <class...> class F, class... PartialArgs>
    struct bind<quote<F>, PartialArgs...> {};
    // bind�������bind
    template <class RoundedF, class... PartialArgs, class... PartialArgs2>
    struct bind<bind<RoundedF, PartialArgs...>, PartialArgs2...> {};

    /**
     * @fn
     * @brief ���^�֐����ۂ߂��񂾂��̂�����
    */
    template <class T> struct is_rounded_meta_function : public std::false_type {};
    template <template <class...> class F>
    struct is_rounded_meta_function<quote<F>> : public std::true_type {};
    template <class RoundedF, class... PartialArgs>
    struct is_rounded_meta_function<bind<RoundedF, PartialArgs...>> : public std::true_type {};

    /**
     * @fn
     * @brief ���^�֐����ۂ߂��񂾂��̂�����
    */
    template <class T> constexpr auto is_rounded_meta_function_v = is_rounded_meta_function<T>::value;

    template <class T>
    concept MetaCallable = is_rounded_meta_function_v<T>;

    /**
     * @fn
     * @brief quote�܂���bind��n���B type�Ăяo���Ŏ��s
    */
    template <MetaCallable F, class... Args> struct apply;
    // ������n���Ď��s
    template <template <class...> class F, class... Args>
    struct apply<quote<F>, Args...> : public F<Args...> {};
    // �ċA�I�ɕ����K�p���ꂽ������W�J
    template <class RoundedF, class... PartialArgs, class... Args>
    struct apply<bind<RoundedF, PartialArgs...>, Args...> : public apply<RoundedF, PartialArgs..., Args...> {};

    template <MetaCallable F, class... Args> using apply_t = apply<F, Args...>::type; 

    /**
     * @fn
     * @brief �󂯎�����p�����[�^�p�b�N�̐擪���擾���郁�^�֐�
    */
    template <class Head, class... Parameters>
    struct get_first_type : public std::type_identity<Head> {};

    /**
     * @fn
     * @brief ������E�֏�ݍ���
    */
    template <MetaCallable F, class... Parameters> struct foldl;
    template <MetaCallable F, class Init, class Head, class... Parameters>
    struct foldl<F, Init, Head, Parameters...> : public foldl<F, apply<F, Init, Head>, Parameters...> {};
    template <MetaCallable F, class Init, class Head>
    struct foldl<F, Init, Head> : public apply<F, Init, Head> {};

    template <MetaCallable F, class... Parameters> using foldl_t = foldl<F, Parameters...>::type;

    /**
     * @fn
     * @brief �E���獶�֏�ݍ���
    */
    template <MetaCallable F, class... Parameters> struct foldr;
    template <MetaCallable F, class Head, class... Parameters>
    struct foldr<F, Head, Parameters...> : public apply<F, Head, foldr<F, Parameters...>> {};
    template <MetaCallable F, class Head, class Next>
    struct foldr<F, Head, Next> : public apply<F, Head, Next> {};

    template <MetaCallable F, class... Parameters> using foldr_t = foldr<F, Parameters...>::type;


    /**
     * @fn
     * @brief �p�����[�^�p�b�N�ɑ΂��A������E�֏�ݍ��݂��s���A��̌^��Ԃ�
    */
    template <template <class...> class Op, class... Parameters> struct foldl_by_type;
    // 2�����֐��̏ꍇ
    template <template <class, class> class Op, class Head, class Next, class... Parameters>
    requires TwoTypeArgsTypeGetable<Op, Head, Next>
    struct foldl_by_type<Op, Head, Next, Parameters...>
        : public foldl_by_type<Op, typename Op<Head, Next>::type, Parameters...> {};
    // ����
    template <template <class, class> class Op, class Param>
    struct foldl_by_type<Op, Param> : public std::type_identity<Param> {};

    /**
     * @fn
     * @brief �p�����[�^�p�b�N�ɑ΂��A�E���獶�֏�ݍ��݂��s���A��̌^��Ԃ�
    */
    template <template <class...> class Op, class... Parameters> struct foldr_by_type;
    template <template <class, class> class Op, class Head, class... Parameters>
    requires TwoTypeArgsTypeGetable<Op, Head, typename foldr_by_type<Op, Parameters...>::type>
    struct foldr_by_type<Op, Head, Parameters...> : public Op<typename foldr_by_type<Op, Parameters...>::type, Head> {};
    // �X�^�[�g
    template <template <class, class> class Op, class Param>
    struct foldr_by_type<Op, Param> : public std::type_identity<Param> {}; 

    /**
     * @fn
     * @brief �S�Ă�Parameters��Fn��K�p�������ʂ̌^���X�g��Ԃ�
    */
    template <template <class> class Fn, class... Parameters>
    requires (TypeGetable<Fn<Parameters>> && ...)
    struct map_types : std::type_identity<type_list<typename Fn<Parameters>::type...>> {};

    /**
     * @fn
     * @brief ���^�����Ƒ�O�����œn���ꂽ�^�̃p�����[�^�p�b�N�����Ԃ�Cond�Ŕ�r���A�ŏ��Ɍ��ʂ��^�ł������p�����[�^���擾���郁�^�֐�
     * @param T ���������̈���
     * @param Cond ����Ɏg�p���郁�^�֐�
     * @param Head �����Ƀp�����[�^�p�b�N���w��
     * @return �ŏ��ɔ��������^��ԋp�B������Ȃ������ꍇvoid��ԋp
     * @details (e.g ���L�̏ꍇ�Ares��std::true_type�ƂȂ�
     * @details using res = find_if_by_type<std::bool_constant<true>, std::is_base_of, int, float, std::true_type, double>::type;
    */
    template <class T, template <class, class> class Cond, class... Parameters> struct find_if_by_type;
    template <class T, template <class, class> class Cond, class Head, class... Parameters>
    requires TwoTypeArgsTestable<Cond, T, Head>
    struct find_if_by_type<T, Cond, Head, Parameters...>
        : public std::conditional_t<Cond<T, Head>::value,
            std::type_identity<Head>,
            find_if_by_type<T, Cond, Parameters...>>
    {};
    // �ċA�̏I�_(�Ȃ������̂�void)
    template <class T, template <class, class> class Cond>
    struct find_if_by_type<T, Cond> : public std::type_identity<void> {};

    /**
     * @fn
     * @brief ���^�����Ƒ�O�����œn���ꂽ�^�̃p�����[�^�p�b�N�����Ԃ�Cond�Ŕ�r���A�ŏ��Ɍ��ʂ��^�ł������p�����[�^���擾���郁�^�֐�
     * @param V ���������̈���
     * @param Cond ����Ɏg�p���郁�^�֐�
     * @param Head �����Ƀp�����[�^�p�b�N���w��
     * @return �Ȃ�������void
    */
    template <auto V, template <auto, class> class Cond, class... Parameters> struct find_if_by_value;
    template <auto V, template <auto, class> class Cond, class Head, class... Parameters>
    requires FirstValueSecondTypeArgsTestable<Cond, V, Head>
    struct find_if_by_value<V, Cond, Head, Parameters...>
        : public std::conditional_t<Cond<V, Head>::value,
            std::type_identity<Head>,
            find_if_by_value<V, Cond, Parameters...>>
    {};
    // �ċA�̏I�_(�����ł���void��ԋp)
    template <auto V, template <auto, class> class Cond>
    struct find_if_by_value<V, Cond> : public std::type_identity<void> {};

    /**
     * @fn
     * @brief �����ɍ��v����^����菜��
     * @tparam T ��r�Ώۂ̌^
     * @tparam Cond ������(�������ɂ�T������)
     * @tparam Parameters �����Ώۂ̃p�����[�^�p�b�N
    */
    template <class T, template <class, class> class Cond, class... Parameters>
    struct remove_if_by_type
        : public foldl_by_type<
            push_type,
            type_list<ignore_type>, // init
            std::conditional_t<Cond<T, Parameters>::value, ignore_type, Parameters>... // �����ɍ��v������̂�ignore_type�֒u��
        >
    {};

    /**
     * @fn
     * @brief 2�l�̔�^�e���v���[�g�������r���郁�^�֐��B�^��v���A�l��v�̏ꍇ�^
    */
    template <auto V1, auto V2> struct equal_values : public std::false_type {};
    template <auto V1, auto V2>
    requires std::same_as<decltype(V1), decltype(V2)>
    struct equal_values<V1, V2> : public std::bool_constant<V1 == V2> {};

    /**
     * @fn
     * @brief �����������ȍ~�̈����̂����ꂩ�Ɉ�v���Ă���ꍇ�^
    */
    template <class Test, class... Parameters>
    struct include_type : public std::bool_constant<(std::is_same_v<Test, Parameters> || ...)> {};

    /**
     * @fn
     * @brief �����������ȍ~�̈����̂����ꂩ�Ɉ�v���Ă���ꍇ�^
    */
    template <auto Test, auto... Parameters>
    struct include_value : public std::bool_constant<(equal_values<Test, Parameters>::value || ...)> {};

    /**
     * @fn
     * @brief �󂯎�����e���v���[�g�p�����[�^�p�b�N�̗v�f�ɏd�������݂��Ȃ������肷�郁�^�֐�
    */
    template <class Head, class... Parameters>
    struct is_unique
        : public std::bool_constant<!include_type<Head, Parameters...>::value && is_unique<Parameters...>::value>
    {};
    template <class Param> struct is_unique<Param> : public std::true_type {};

    /**
     * @fn
     * @brief �󂯎������^�e���v���[�g�p�����[�^�p�b�N�̗v�f�ɏd�������݂��Ȃ������肷�郁�^�֐�
     * @details �^���قȂ��Ă���Εs��v�Ƃ���B�^����v���Ă����ꍇ�͒l���r���A�قȂ��Ă���Εs��v�Ƃ���
    */
    template <auto Head, auto... Parameters>
    struct is_unique_values
        : public std::bool_constant<!include_value<Head, Parameters...>::value && is_unique_values<Parameters...>::value>
    {};
    template <auto Param> struct is_unique_values<Param> : public std::true_type {};

    /**
     * @fn
     * @brief �p�����[�^�p�b�N�̌^���S�ē����Ƃ��^
    */
    template <class Head, class... Parameters>
    struct is_same_types
        : public std::bool_constant<(std::is_same_v<Head, Parameters> && ...)> {};

    /**
     * @fn
     * @brief �p�����[�^�p�b�N�̒l�̌^���S�ē������^
    */
    template <auto... Parameters>
    struct is_same_value_types : public is_same_types<decltype(Parameters)...> {};

    /**
     * @fn
     * @brief �e���v���[�g�����̐��𐔂���
    */
    template <class... Types>
    struct count_parameters : public std::integral_constant<std::size_t, sizeof...(Types)> {};

    /**
     * @fn
     * @brief �e���v���[�g�����̐��𐔂���
    */
    template <auto... Values>
    struct count_value_parameters : public std::integral_constant<std::size_t, sizeof...(Values)> {};

    /**
     * @fn
     * @brief ��̌^�̗̈�̃T�C�Y��������������
    */
    template <class T1, class T2>
    struct equal_type_size : public std::bool_constant<sizeof(T1) == sizeof(T2)> {};

    /**
     * @fn
     * @brief �e���v���[�g�����œn���������l�̌������擾����B�������͔C�ӂ̐i�����w�肷��
    */
    template <auto V, std::size_t Hex = 10>
    requires (Hex >= 2 && (std::integral<decltype(V)> || is_big_int<decltype(V)>::value))
    constexpr auto get_digit()
    {
        std::size_t digit = 1;
        for (auto val = V; val /= static_cast<decltype(V)>(Hex); digit++);
        return digit;
    }

    //-----------------------------------------------------
    // �ȉ��A_t��`
    //-----------------------------------------------------

    template <class... Parameters>
    using get_first_type_t = get_first_type<Parameters...>::type;

    template <template <class...> class Op, class... Parameters>
    using foldl_by_type_t = foldl_by_type<Op, Parameters...>::type;

    template <template <class...> class Op, class... Parameters>
    using foldr_by_type_t = foldr_by_type<Op, Parameters...>::type;

    template <template <class> class Fn, class... Parameters>
    using map_types_t = map_types<Fn, Parameters...>::type;

    template <class T, template <class, class> class Cond, class... Parameters>
    using find_if_by_type_t = find_if_by_type<T, Cond, Parameters...>::type;

    template <auto V, template <auto, class> class Cond, class... Parameters>
    using find_if_by_value_t = find_if_by_value<V, Cond, Parameters...>::type;

    template <class T, template <class, class> class Cond, class... Parameters>
    using remove_if_by_type_t = remove_if_by_type<T, Cond, Parameters...>::type;

    //-----------------------------------------------------
    // �ȉ��A_v��`
    //-----------------------------------------------------

    template <auto V1, auto V2>
    constexpr auto equal_values_v = equal_values<V1, V2>::value;

    template <class Test, class...Parameters>
    constexpr auto include_type_v = include_type<Test, Parameters...>::value;

    template <auto Test, auto... Parameters>
    constexpr auto include_value_v = include_value<Test, Parameters...>::value;

    template <class... Parameters>
    constexpr auto is_unique_v = is_unique<Parameters...>::value;

    template <auto... Parameters>
    constexpr auto is_unique_values_v = is_unique_values<Parameters...>::value;

    template <class... Parameters>
    constexpr auto is_same_types_v = is_same_types<Parameters...>::value;

    template <auto... Parameters>
    constexpr auto is_same_value_types_v = is_same_value_types<Parameters...>::value;

    template <class... Types>
    constexpr auto count_parameters_v = count_parameters<Types...>::value;

    template <auto... Values>
    constexpr auto count_value_parameters_v = count_value_parameters<Values...>::value;
}

#endif // TUUTILCPP_INCLUDE_GUARD_ALGORITHM_HPP
