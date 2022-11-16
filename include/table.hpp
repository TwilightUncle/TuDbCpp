///----------------------------------
/// @file table.hpp
/// @brief DB�e�[�u����`
///----------------------------------
#pragma once
#include <column.hpp>

namespace tudb
{
    /**
     * @fn
     * @brief �e�[�u����`�p���^�֐�
     * @param ETableType �e�[�u���Ɨ�̎��ʎq�Ƃ��Ē�`�����X�R�[�v�t���񋓌^
     * @param Name cstr�ŕ��œn���������񃊃e����
     * @param ColumnDefinitionList define_column�ɂ����`���w�肷��
    */
    template <
        enumeration ETableType,
        StringLiteralSpecificable auto Name,
        ColumnListDefinitionable ColumnDefinitionList
    >
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
