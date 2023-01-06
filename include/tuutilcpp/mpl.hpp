///----------------------------------
/// @file mpl.hpp 
/// @brief �I���I�����^�v���O���~���O�p���C�u����
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_META_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_META_HPP

#include <concepts>

namespace tuutil::mpl
{
    /**
     * @class
     * @brief ��������������󂾂����肵���ꍇ�ɓ���^
    */
    struct ignore_type {};
}

#include <tuutilcpp/mpl/meta_callback.hpp>
#include <tuutilcpp/mpl/map.hpp>
#include <tuutilcpp/mpl/fold.hpp>
#include <tuutilcpp/mpl/find_if.hpp>
#include <tuutilcpp/mpl/extract_if.hpp>

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_META_HPP
