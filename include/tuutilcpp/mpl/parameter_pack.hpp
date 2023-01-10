///----------------------------------
/// @file mpl/parameter_pack.hpp 
/// @brief �^�p�����[�^�p�b�N�̑���┻��
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_REVERSE_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_REVERSE_HPP

namespace tuutil::mpl
{
    /**
     * @fn
     * @brief �e���v���[�g�N���X���^�p�����[�^�Ƃ��ė��p�ł���悤�ɂ���
     * @tparam List �p�����[�^�p�b�N���w��̃e���v���[�g�^
    */
    template <template <class...> class List> struct lift {};

    template <template <class...> class List, class T>
    struct push_back<lift<List>, T> : public std::type_identity<List<T>> {};

    template <template <class...> class List, class T>
    struct push_front<lift<List>, T> : public std::type_identity<List<T>> {};

    /**
     * @fn
     * @brief lift�ŕ�܂ꂽ�e���v���[�g�^������
     * @tparam T �����Ώ�
    */
    template <class T> struct is_lifted : public std::false_type {};
    template <template <class...> class List>
    struct is_lifted<lift<List>> : public std::true_type {};

    template <class T>
    concept LiftedList = is_lifted<T>::value;

    /**
     * @fn
     * @brief lift�ŕ�܂ꂽ�e���v���[�g�N���X�Ƀp�����[�^�p�b�N��K�p����
    */
    template <template <class...> class List, class... Parameters>
    struct apply<lift<List>, Parameters...> : public std::type_identity<List<Parameters...>> {};

    /**
     * @fn
     * @brief �^�̃p�����[�^�p�b�N�̕��т𔽓]������
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <class TypeList> struct reverse;
    template <template <class...> class List, class Head, class... Parameters>
    struct reverse<List<Head, Parameters...>> : public foldl<quote<push_front>, List<Head>, List<Parameters...>> {}; 

    /**
     * @fn
     * @brief �^�̃p�����[�^�p�b�N�̕��т𔽓]������
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <class TypeList> using reverse_t = reverse<TypeList>::type;

    /**
     * @fn
     * @brief �^�̃p�����[�^�p�b�N�̕��т��������Ɉ���炵�A�擪�̌^���Ō���Ɉڂ�
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <class TypeList> struct rotatel;
    template <template <class...> class List, class Head, class... Parameters>
    struct rotatel<List<Head, Parameters...>> : public std::type_identity<List<Parameters..., Head>> {};

    /**
     * @fn
     * @brief �^�̃p�����[�^�p�b�N�̕��т��������Ɉ���炵�A�擪�̌^���Ō���Ɉڂ�
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <class TypeList> using rotatel_t = rotatel<TypeList>::type;

    /**
     * @fn
     * @brief �^�̃p�����[�^�p�b�N�̕��т��E�����Ɉ���炵�A�擪�̌^���Ō���Ɉڂ�
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <class TypeList> struct rotater;
    template <template <class...> class List, class... Parameters>
    struct rotater<List<Parameters...>> : public reverse<rotatel_t<reverse_t<List<Parameters...>>>> {};

    /**
     * @fn
     * @brief �^�̃p�����[�^�p�b�N�̕��т��E�����Ɉ���炵�A�擪�̌^���Ō���Ɉڂ�
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <class TypeList> using rotater_t = rotater<TypeList>::type;

    /**
     * @fn
     * @brief �^�̃p�����[�^�p�b�N����������
     * @tparam T �Ώ�
    */
    template <class T> struct has_type_parameters : public std::false_type {};
    template <template <class...> class List, class... Parameters>
    struct has_type_parameters<List<Parameters...>> : public std::true_type {};

    /**
     * @fn
     * @brief ��^�̃p�����[�^�p�b�N����������
     * @tparam T �Ώ�
    */
    template <class T> struct has_value_parameters : public std::false_type {};
    template <template <auto...> class List, auto... Parameters>
    struct has_value_parameters<List<Parameters...>> : public std::true_type {};

    /**
     * @fn
     * @brief �^�̃p�����[�^�p�b�N����������
     * @tparam T �����Ώ�
    */
    template <class T> constexpr auto has_type_parameters_v = has_type_parameters<T>::value;

    /**
     * @fn
     * @brief ��^�̃p�����[�^�p�b�N����������
     * @tparam T �Ώ�
    */
    template <class T> constexpr auto has_value_parameters_v = has_value_parameters<T>::value;

    /**
     * @fn
     * @brief �p�����[�^�p�b�N�̓��e��Dest�փR�s�[
     * @tparam Src �R�s�[���̌^�p�����[�^�p�b�N�����^
     * @tparam Dest �R�s�[�Ώۂ̃e���v���[�g�N���X
    */
    template <class Src, LiftedList Dest> struct copy;
    template <template <class...> class Src, class Dest, class... Parameters>
    struct copy<Src<Parameters...>, Dest> : public apply<Dest, Parameters...> {};

    /**
     * @fn
     * @brief �p�����[�^�p�b�N�̓��e��Dest�փR�s�[
     * @tparam Src �R�s�[���̌^�p�����[�^�p�b�N�����^
     * @tparam Dest �R�s�[�Ώۂ̃e���v���[�g�N���X
    */
    template <class Src, LiftedList Dest> using copy_t = copy<Src, Dest>::type;

    /**
     * @fn
     * @brief �p�����[�^�p�b�N�̐��𐔂���
     * @tparam T �^�p�����[�^�p�b�N�����^
    */
    template <class T> struct count;
    template <template <class...> class List, class... Parameters>
    struct count<List<Parameters...>> : public std::integral_constant<std::size_t, sizeof...(Parameters)> {};
    template <template <auto...> class List, auto... Parameters>
    struct count<List<Parameters...>> : public std::integral_constant<std::size_t, sizeof...(Parameters)> {};

    /**
     * @fn
     * @brief �p�����[�^�p�b�N�̐��𐔂���
     * @tparam T �^�p�����[�^�p�b�N�����^
    */
    template <class T> constexpr auto count_v = count<T>::value;

    /**
     * @fn
     * @brief �p�����[�^�p�b�N�̌^���S�ē����Ƃ��^
     * @tparam T �����Ώۂ̌^�p�����[�^�p�b�N�����^
    */
    template <class T> struct is_same_types;
    template <template <class...> class List, class Head, class... Parameters>
    struct is_same_types<List<Head, Parameters...>> : public std::bool_constant<(std::is_same_v<Head, Parameters> && ...)> {};
    // ��^�e���v���[�g�p�����[�^�̌^�����݂̂̔�r(��̓I�Ȓl�ɂ��Ă͊֒m���Ȃ�)
    template <template <auto...> class List, auto... Parameters>
    struct is_same_types<List<Parameters...>> : public is_same_types<type_list<decltype(Parameters)...>> {};

    /**
     * @fn
     * @brief �p�����[�^�p�b�N�̌^���S�ē����Ƃ��^
     * @tparam T �����Ώۂ̌^�p�����[�^�p�b�N�����^
    */
    template <class T> constexpr auto is_same_types_v = is_same_types<T>::value;

    /**
     * @fn
     * @brief �^T��List�̃p�����[�^�p�b�N�Ɋ܂܂�Ă���Ƃ��^
     * @tparam T �܂܂�Ă��邩�m�F�������^
     * @tparam List �p�����[�^�p�b�N�����^
    */
    template <class T, class List> struct contains;
    template <class T, template <class...> class List, class... Parameters>
    struct contains<T, List<Parameters...>> : public std::bool_constant<(std::is_same_v<T, Parameters> || ...)> {};

    /**
     * @fn
     * @brief �^T��List�̃p�����[�^�p�b�N�Ɋ܂܂�Ă���Ƃ��^
     * @tparam T �܂܂�Ă��邩�m�F�������^
     * @tparam List �p�����[�^�p�b�N�����^
    */
    template <class T, class List> constexpr auto contains_v = contains<T, List>::value;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_REVERSE_HPP
