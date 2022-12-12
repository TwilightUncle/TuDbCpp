#include <gtest/gtest.h>
#include <tustrcpp/regex.hpp>


using namespace std::string_view_literals;
TEST(tustrcpptest, RegexFunctionTest)
{
    using test_type = tustr::regex<"abcdef">;
    constexpr auto empty_inst = tustr::empty_regex{};
    // // �ǂ̕��@���g�p���邩�w�肪�Ȃ������ꍇ�AECMAScript�̃r�b�g���܂܂�Ă��邩�m�F
    // constexpr auto inst1 = tustr::regex{"abcde", std::regex_constants::syntax_option_type::icase};

    // EXPECT_EQ(inst1.syntax, std::regex_constants::syntax_option_type::ECMAScript | std::regex_constants::syntax_option_type::icase);
    constexpr auto case1 = tustr::is_collect_regex_back_slash("a\\0\\\\a\\\\");
    constexpr auto case2 = tustr::is_collect_regex_back_slash("a\\0\\\\a\\");
    constexpr auto case3 = tustr::is_collect_regex_back_slash("\\a\\0\\\\a\\\\");
    constexpr auto case4 = tustr::is_collect_regex_back_slash("a\\0\\\\\\a\\\\");
    constexpr auto case5 = tustr::is_allowed_string("aaaab", true, "ab", "");
    constexpr auto case6 = tustr::is_allowed_string("\\0\\n\\\\", true, "", "0n\\");
    constexpr auto case7 = tustr::is_allowed_string("a\\0\\\\a\\\\", true, "a", "\\0");
    constexpr auto case8 = tustr::is_allowed_string("a\\0\\\\a\\\\", false, "bc", "ds");
    constexpr auto case9 = tustr::is_allowed_string("ab\\0\\\\a\\\\", true, "a", "\\0");
    constexpr auto case10 = tustr::is_allowed_string("a\\d\\0\\\\a\\\\", false, "bc", "ds");
    ASSERT_TRUE(case1);
    ASSERT_FALSE(case2);
    ASSERT_FALSE(case3);
    ASSERT_FALSE(case4);
    ASSERT_TRUE(case5);
    ASSERT_TRUE(case6);
    ASSERT_TRUE(case7);
    ASSERT_TRUE(case8);
    ASSERT_FALSE(case9);
    ASSERT_FALSE(case10);
}

TEST(tustrcpptest, RegexExtractBrancketTest)
{
    // ����
    constexpr auto case11 = tustr::regex_bracket_inner<"abc[defg{hijk}lm\\](opq)]rs", 3>::value;
    constexpr auto case12 = tustr::regex_bracket_inner<"abc{defg\\{hijk\\}lm](opq)}rs", 3>::value;
    constexpr auto case13 = tustr::regex_bracket_inner<"abc(defg{hijk\\}lm](opq\\))rs", 3>::value;
    constexpr auto case14 = tustr::regex_bracket_inner<"abc<defg{hijk\\}lm](opq\\)>rs", 3>::value;
    EXPECT_STREQ(case11.data(), "defg{hijk}lm\\](opq)");
    EXPECT_STREQ(case12.data(), "defg\\{hijk\\}lm](opq)");
    EXPECT_STREQ(case13.data(), "defg{hijk\\}lm](opq\\)");
    EXPECT_STREQ(case14.data(), "defg{hijk\\}lm](opq\\)");

    // �ُ�(�Ή���������ʂ����݂��Ȃ�) �R�����g�A�E�g���O���ƃR���p�C���G���[������
    // constexpr auto case15 = tustr::regex_bracket_inner<"abc[defg{hijk}lm\\](opq)\\]rs", 3>::value;
    // constexpr auto case16 = tustr::regex_bracket_inner<"abc{defg\\{hijk\\}lm](opq)\\}rs", 3>::value;
    // constexpr auto case17 = tustr::regex_bracket_inner<"abc(defg{hijk\\}lm](opq\\)\\)rs", 3>::value;
    // constexpr auto case18 = tustr::regex_bracket_inner<"abc<defg{hijk\\}lm](opq\\)\\>rs", 3>::value;
}

TEST(tustrcpptest, RegexCharRangeParseTest)
{
    // �����N���X,�o�b�N�X���b�V��,�����͈͎w��Ȃ�
    constexpr auto case19 = tustr::get_regex_char_range_matcher<"ab[-abc.]", 2>()(".c-aba");
    constexpr auto case20 = tustr::get_regex_char_range_matcher<"[abc.-]", 0>()(".c-aba");
    constexpr auto case21 = tustr::get_regex_char_range_matcher<"[-abc.]", 0>()("b.c-d");
    constexpr auto case22 = tustr::get_regex_char_range_matcher<"[-abc.]", 0>()("def");
    constexpr auto case23 = tustr::get_regex_char_range_matcher<"[^-abc]", 0>()("cab");
    constexpr auto case24 = tustr::get_regex_char_range_matcher<"[^-abc]", 0>()("bcd");
    constexpr auto case25 = tustr::get_regex_char_range_matcher<"[^-abc]", 0>()("def^");
    EXPECT_TRUE(case19);
    EXPECT_TRUE(case20);
    EXPECT_FALSE(case21);
    EXPECT_FALSE(case22);
    EXPECT_FALSE(case23);
    EXPECT_FALSE(case24);
    EXPECT_TRUE(case25);

    // �����N���X�w�肠��
    constexpr auto case26 = tustr::get_regex_char_range_matcher<"[\\d]", 0>()("0123456789");
    constexpr auto case27 = tustr::get_regex_char_range_matcher<"[\\d]", 0>()("0123456789a");
    constexpr auto case28 = tustr::get_regex_char_range_matcher<"[^\\d]", 0>()("0aabc_%&\t\\[");
    constexpr auto case29 = tustr::get_regex_char_range_matcher<"[^\\d]", 0>()("abc_%&\t\\[");
    constexpr auto case30 = tustr::get_regex_char_range_matcher<"[\\D]", 0>()("0aabc_%&\t\\[");
    constexpr auto case31 = tustr::get_regex_char_range_matcher<"[\\D]", 0>()("aabc_%&\t\\[");
    constexpr auto case32 = tustr::get_regex_char_range_matcher<"[^\\D]", 0>()("0123456789");
    constexpr auto case33 = tustr::get_regex_char_range_matcher<"[^\\D]", 0>()("0123456789a");

    // ���������֐���ێ��ł��邩���m�F(�����W�̃e�X�g)
    constexpr auto range_matcher1 = tustr::get_regex_char_range_matcher<"[0-5a-dA\\-E\\t]", 0>();

    EXPECT_TRUE(case26);
    EXPECT_FALSE(case27);
    EXPECT_FALSE(case28);
    EXPECT_TRUE(case29);
    EXPECT_FALSE(case30);
    EXPECT_TRUE(case31);
    EXPECT_TRUE(case32);
    EXPECT_FALSE(case33);
    EXPECT_TRUE(range_matcher1("0123451bcdA-E\t"));
    EXPECT_FALSE(range_matcher1("0123451bcdA-Et"));
    EXPECT_FALSE(range_matcher1("0123451bcdABCDE"));
}

TEST(tustrcpptest, RegexQuantifierParseTest)
{
    using type1 = tustr::regex_quantifier_perser<"abcd*e", 4>;
    using type2 = tustr::regex_quantifier_perser<"abcd*", 4>;
    using type3 = tustr::regex_quantifier_perser<"abcd*?", 4>;
    using type4 = tustr::regex_quantifier_perser<"abcd+", 4>;
    using type5 = tustr::regex_quantifier_perser<"abcd?", 4>;
    using type6 = tustr::regex_quantifier_perser<"abcd{20}e", 4>;
    using type7 = tustr::regex_quantifier_perser<"abcd{2,}", 4>;
    using type8 = tustr::regex_quantifier_perser<"abcd{25,512}", 4>;
    using type9 = tustr::regex_quantifier_perser<"abcd{20}?e", 4>;
    using type10 = tustr::regex_quantifier_perser<"abcd{2,}?", 4>;
    using type11 = tustr::regex_quantifier_perser<"abcd{25,512}?", 4>;
    EXPECT_FALSE(type1::negative);
    EXPECT_FALSE(type2::negative);
    EXPECT_TRUE(type3::negative);
    EXPECT_FALSE(type4::negative);
    EXPECT_FALSE(type5::negative);
    EXPECT_FALSE(type6::negative);
    EXPECT_FALSE(type7::negative);
    EXPECT_FALSE(type8::negative);
    EXPECT_TRUE(type9::negative);
    EXPECT_TRUE(type10::negative);
    EXPECT_TRUE(type11::negative);
    EXPECT_EQ(type1::min_count, 0);
    EXPECT_EQ(type1::max_count, std::string_view::npos);
    EXPECT_EQ(type2::min_count, 0);
    EXPECT_EQ(type3::max_count, std::string_view::npos);
    EXPECT_EQ(type4::min_count, 1);
    EXPECT_EQ(type4::max_count, std::string_view::npos);
    EXPECT_EQ(type5::min_count, 0);
    EXPECT_EQ(type5::max_count, 1);
    EXPECT_EQ(type6::min_count, 20);
    EXPECT_EQ(type6::max_count, 20);
    EXPECT_EQ(type7::min_count, 2);
    EXPECT_EQ(type7::max_count, std::string_view::npos);
    EXPECT_EQ(type8::min_count, 25);
    EXPECT_EQ(type8::max_count, 512);
}

TEST(tustrcpptest, RegexCaptureParserTest)
{
    using type1 = tustr::regex_capture_parser<"ab(cdefg)", 2>;
    using type2 = tustr::regex_capture_parser<"ab(?:cdefg)", 2>;
    using type3 = tustr::regex_capture_parser<"ab(?<a_1>cdefg)", 2>;

    EXPECT_TRUE(type1::is_capture);
    EXPECT_FALSE(type2::is_capture);
    EXPECT_TRUE(type3::is_capture);
    EXPECT_STREQ(type1::name.data(), "");
    EXPECT_STREQ(type2::name.data(), "");
    EXPECT_STREQ(type3::name.data(), "a_1");
    EXPECT_FALSE(type1::is_named);
    EXPECT_FALSE(type2::is_named);
    EXPECT_TRUE(type3::is_named);
    EXPECT_STREQ(type1::capture_pattern.data(), "cdefg");
    EXPECT_STREQ(type2::capture_pattern.data(), "cdefg");
    EXPECT_STREQ(type3::capture_pattern.data(), "cdefg");
}