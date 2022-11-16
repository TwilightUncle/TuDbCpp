#pragma once

namespace tudb
{
    // �O���錾
    template <class Head, class... Parameters> struct is_unique;

    /**
     * @brief �񋓌^�ł��邱�Ƃ���������R���Z�v�g(�ق�Ƃ�scoped_enum�𔻒肵������c++23����)
    */
    template <class T> concept enumeration = std::is_enum_v<T>;

    /**
     * @brief ������s�����^�֐��ł��邱�Ƃ̐���(bool_constant���p�����Ă���Βʉ�)
    */
    template <typename T>
    concept Testable = requires {
        T::value;
        typename T::value_type;
        requires std::is_same_v<bool, typename T::value_type>;
    };

    /**
     * @brief ��̌^�������󂯎�蔻�肷�郁�^�֐��ł��邱�Ƃ𐧖񂷂�
    */
    template <template <class, class> class T, class ArgT1, class ArgT2>
    concept TwoTypeArgsTestable = Testable<T<ArgT1, ArgT2>>;

    /**
     * @brief ��^�����ƌ^����������󂯎�蔻�肷�郁�^�֐��ł��邱�Ƃ𐧖񂷂�
    */
    template <template <auto, class> class T, auto ArgV, class ArgT>
    concept FirstValueSecondTypeArgsTestable = Testable<T<ArgV, ArgT>>;

    /**
     * @brief �^�擾���^�֐��ł��鐧��
    */
    template <typename T>
    concept TypeGetable = requires {
        typename T::type;
    };

    /**
     * @brief ��̌^�������󂯎��^��Ԃ����^�֐��ł��邱�Ƃ𐧖񂷂�
    */
    template <template <class, class> class T, class ArgT1, class ArgT2>
    concept TwoTypeArgsTypeGetable = TypeGetable<T<ArgT1, ArgT2>>;

    /**
     * @brief �d���Ȃ��̃p�����[�^�p�b�N��������
    */
    template <class... Types>
    concept TemplateTypesUniqueable = requires {
        requires (sizeof...(Types) > 0);
        requires is_unique<Types...>::value;
    };
}
