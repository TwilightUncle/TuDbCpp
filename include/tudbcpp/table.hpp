///----------------------------------
/// @file table.hpp
/// @brief DB�e�[�u����`
///----------------------------------
#ifndef TUDBCPP_INCLUDE_GUARD_TABLE_HPP
#define TUDBCPP_INCLUDE_GUARD_TABLE_HPP

#include <tudbcpp/column.hpp>

namespace tudb
{
    /**
     * @fn
     * @brief �e���v���[�g�������ؗp�̃��^�֐�
    */
    template <
        ColumnListDefinitionable ColumnDefinitionList,
        ConstraintListDefinable ConstraintDefinitionList
    >
    struct validate_define_table_constraint_arg : public std::bool_constant<
        ConstraintListDefinable<
            concat_type_list_t<
                ConstraintDefinitionList,
                extract_constraints_t<ColumnDefinitionList>
            >
        >
    > {};

    /**
     * @fn
     * @brief �e�[�u����`�p���^�֐�
     * @tparam ETableType �e�[�u���Ɨ�̎��ʎq�Ƃ��Ē�`�����X�R�[�v�t���񋓌^
     * @tparam Name tustr::cstr�ŕ��œn���������񃊃e����
     * @tparam ColumnDefinitionList define_column�ɂ����`���w�肷��
     * @tparam ConstraintDefinitionList define_column�ɂ����`���w�肷��
    */
    template <
        enumeration ETableType,
        tustr::cstr Name,
        ColumnListDefinitionable ColumnDefinitionList,
        ConstraintListDefinable ConstraintDefinitionList = constraint_unspecified
    >
    requires validate_define_table_constraint_arg<ColumnDefinitionList, ConstraintDefinitionList>::value
    struct define_table
    {
    private:
        template <auto V, ColumnDefinable T>
        requires std::same_as<ETableType, decltype(V)>
        struct is_match_column_id : public std::bool_constant<V == T::id> {};

    public:
        static constexpr auto name = Name;

        /**
         * @fn
         * @brief V�ɍ��v������`���擾����
        */
        template <ETableType V>
        using get_column_definition_t = find_if_by_value_t<V, is_match_column_id, pass_types<ColumnDefinitionList>>;

        /**
         * @fn
         * @brief V�ɍ��v���������擾����
        */
        template <ETableType V>
        static constexpr auto get_column_info()
        {
            return get_column_definition_t<V>{};
        }
    };
}

#endif // TUDBCPP_INCLUDE_GUARD_TABLE_HPP
