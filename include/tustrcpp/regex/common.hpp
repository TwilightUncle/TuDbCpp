///----------------------------------
/// @file regex/common.hpp 
/// @brief regex.hpp�̊֘A
///----------------------------------
#ifndef TUSTRCPP_INCLUDE_GUARD_REGEX_COMMON_HPP
#define TUSTRCPP_INCLUDE_GUARD_REGEX_COMMON_HPP

namespace tustr
{
    enum {
        // �ʏ�͈͂ɒu������ꕶ���̐���
        REGEX_SPECIAL       = 0x0001u,  // ���ꕶ��(�o�b�N�X���b�V���ŃG�X�P�[�v)
        REGEX_BK_SPECIAL     = 0x0002u,  // ���ꕶ��(�o�b�N�X���b�V����t�^�����ꍇ)
        REGEX_BRANCKET_BE   = 0x0004u,  // ���ʎn�܂�
        REGEX_BRANCKET_EN   = 0x0008u,  // ���ʏI���
        REGEX_PREV_REF      = 0x0010u,  // �O�̕������Q�Ƃ���
        REGEX_NEXT_REF      = 0x0020u,  // ���̕������Q�Ƃ���
        REGEX_BEGIN         = 0x0040u,  // �擪�ɂ����w��ł��Ȃ�
        REGEX_END           = 0x0080u,  // �����ɂ����w��ł��Ȃ�

        // []���ɂ�������ꕶ���̐���
        // REGEX_CLASSES_SPECIAL�ȊO�Ƀr�b�g�������Ă��Ȃ���΃o�b�N�X���b�V���ŃG�X�P�[�v���K�v�A
        // REGEX_CLASSES_SPECIAL�ȊO�Ƀr�b�g�������Ă���ꍇ�A�Y������S�Ă𖞂����Ă��Ȃ��ꍇ�ł͓��ꕶ���Ƃ��ĔF�����Ă͂����Ȃ�
        REGEX_CLASSES_SPECIAL       = 0x0100u,  // ���ꕶ��([]���ɂ�����)
        REGEX_CLASSES_BK_SPECIAL    = 0x0200u,
        REGEX_CLASSES_PREV_REF      = 0x0400u,
        REGEX_CLASSES_NEXT_REF      = 0x0800u,
        REGEX_CLASSES_BEGIN         = 0x1000u,  // �擪
        REGEX_CLASSES_END           = 0x2000u,

        // ���ꕶ�����ǂ����̔���p�}�X�N
        REGEX_SPECIAL_MASK = REGEX_SPECIAL | REGEX_BK_SPECIAL | REGEX_CLASSES_SPECIAL | REGEX_CLASSES_BK_SPECIAL,
    };

    /**
     * @fn
     * @brief �w�肵�������̐��K�\���Ƃ��Ă̕��@�I�Ȑ��������������̃r�b�g����擾����
     * @see https://developer.mozilla.org/en-US/docs/Web/JavaScript/Guide/Regular_Expressions/Cheatsheet
    */
    inline constexpr auto get_regex_character_attribute(char ch)
    {
        std::uint32_t attrs[(std::numeric_limits<char>::max)() + 1] = {};
        // Character classes
        attrs['['] |= REGEX_SPECIAL | REGEX_BRANCKET_BE | REGEX_CLASSES_SPECIAL;
        attrs[']'] |= REGEX_SPECIAL | REGEX_BRANCKET_EN | REGEX_CLASSES_SPECIAL;
        attrs['-'] |= REGEX_CLASSES_SPECIAL | REGEX_CLASSES_PREV_REF | REGEX_CLASSES_NEXT_REF;
        attrs['^'] |= REGEX_CLASSES_SPECIAL | REGEX_CLASSES_BEGIN;
        attrs['.'] |= REGEX_SPECIAL;
        attrs['d'] |= REGEX_BK_SPECIAL;
        attrs['D'] |= REGEX_BK_SPECIAL;
        attrs['w'] |= REGEX_BK_SPECIAL;
        attrs['W'] |= REGEX_BK_SPECIAL;
        attrs['s'] |= REGEX_BK_SPECIAL;
        attrs['S'] |= REGEX_BK_SPECIAL;
        attrs['t'] |= REGEX_BK_SPECIAL;
        attrs['r'] |= REGEX_BK_SPECIAL;
        attrs['n'] |= REGEX_BK_SPECIAL;
        attrs['v'] |= REGEX_BK_SPECIAL;
        attrs['f'] |= REGEX_BK_SPECIAL;
        attrs['b'] |= REGEX_CLASSES_BK_SPECIAL;
        attrs['0'] |= REGEX_BK_SPECIAL;
        attrs['c'] |= REGEX_BK_SPECIAL | REGEX_NEXT_REF;
        attrs['x'] |= REGEX_BK_SPECIAL | REGEX_NEXT_REF;
        attrs['u'] |= REGEX_BK_SPECIAL | REGEX_NEXT_REF;
        attrs['\\'] |= REGEX_SPECIAL | REGEX_NEXT_REF | REGEX_CLASSES_SPECIAL | REGEX_CLASSES_NEXT_REF;
        attrs['|'] |= REGEX_SPECIAL | REGEX_PREV_REF | REGEX_NEXT_REF;
        // Assertions
        attrs['^'] |= REGEX_SPECIAL | REGEX_BEGIN;
        attrs['$'] |= REGEX_SPECIAL | REGEX_END;
        attrs['b'] |= REGEX_BK_SPECIAL | REGEX_PREV_REF | REGEX_NEXT_REF;
        attrs['B'] |= REGEX_BK_SPECIAL | REGEX_PREV_REF | REGEX_NEXT_REF;
        attrs['?'] |= REGEX_SPECIAL | REGEX_PREV_REF | REGEX_NEXT_REF;
        // Groups and backreferences
        attrs['('] |= REGEX_SPECIAL | REGEX_BRANCKET_BE;
        attrs[')'] |= REGEX_SPECIAL | REGEX_BRANCKET_BE;
        for (const auto c : std::views::iota('1', '9'))
            attrs[c] |= REGEX_BK_SPECIAL;
        attrs['k'] |= REGEX_BK_SPECIAL | REGEX_NEXT_REF;
        // Quantifiers
        attrs['*'] |= REGEX_SPECIAL | REGEX_PREV_REF;
        attrs['+'] |= REGEX_SPECIAL | REGEX_PREV_REF;
        attrs['?'] |= REGEX_SPECIAL | REGEX_PREV_REF;
        attrs['{'] |= REGEX_SPECIAL | REGEX_BRANCKET_BE;
        attrs['}'] |= REGEX_SPECIAL | REGEX_BRANCKET_EN;
        // Unicode property escapes
        attrs['p'] |= REGEX_BK_SPECIAL | REGEX_NEXT_REF;
        attrs['P'] |= REGEX_BK_SPECIAL | REGEX_NEXT_REF;

        return attrs[ch];
    }

    struct regex_char_attribute
    {
        // ���ʎq�̋N�_
        static constexpr auto quantifier_chars = cstr{"*+?{"};

        // �A���J�[�̋N�_
        static constexpr auto anchor_chars = cstr{"^$("};
        static constexpr auto bk_anchor_chars = cstr{"bB"};

        // �����N���X
        static constexpr auto class_chars = cstr{"."};
        static constexpr auto bk_class_chars = cstr{"dDwWsStrnvf0\\"};

        // (�L���v�`��/��L���v�`��)�O���[�v
        static constexpr auto capture_chars = cstr{"("};

        // or
        static constexpr auto or_match_chars = cstr{"|"};

        // �����W��
        static constexpr auto char_set = cstr{"["};

        // �L���v�`���O���[�v�Q��
        static constexpr auto bk_reference_chars = cstr{"123456789"};
    };

    /**
     * @fn
     * @brief �������Ȃ�������������Ă���o�b�N�X���b�V�������݂���ꍇ�U���Ԃ�
    */
    inline constexpr bool is_collect_regex_back_slash(const std::string_view& target)
    {
        for (int i = 0; i < target.size(); i++)
            if (target[i] == '\\' && (i == target.size() - 1 || !get_regex_character_attribute(target[++i])))
                return false;
        return true;
    }

    /**
     * @fn
     * @brief �����X�g�܂��͋��ۃ��X�g�Ƃ��Ďw�肵��char_list���Q�Ƃ��Atarget���L�������؂���B�o�b�N�X���b�V�����܂ޏꍇ�A��ňꕶ���ƔF������
     * @param target ���؂��镶����
     * @param allow_or_deny allow�̏ꍇtrue���w��Adeny�̏ꍇfalse���w��
     * @param char_list ���܂��͋��ە����̃��X�g�Ballow_or_deny�ɂ���ċ��ۃ��X�g�������X�g�����䂷��
    */
    inline constexpr bool is_allowed_string(
        const std::string& target,
        bool allow_or_deny,
        const std::string& char_list,
        const std::string& bk_char_list,
        bool is_bk_escape = true
    ) {
        const auto collate_list = [&allow_or_deny](const std::string& list, char ch)->bool {
            // �����X�g�̒����疢�����̂��̂��������ꍇ�܂��́A
            // ���ۃ��X�g�̒����甭�����ꂽ���̂��������ꍇ�A���s
            return (list.find_first_of(ch) == std::string::npos) == allow_or_deny;
        };

        for (int i = 0; i < target.size(); i++) {
            if ((target[i] != '\\' || !is_bk_escape) && collate_list(char_list, target[i])) return false;
            else if (target[i] == '\\' && i < target.size() - 1 && collate_list(bk_char_list, target[++i])) return false;
        }
        return true;
    }

    /**
     * @fn
     * @brief index�̈ʒu�̈�O�̕������w��̕������m�F����
    */
    inline constexpr bool eq_before_char(const std::string_view& target, int index, char ch)
    {
        return index > 0 && target[(std::max)(index - 1, 0)] == ch;
    }
}

#endif // TUSTRCPP_INCLUDE_GUARD_REGEX_COMMON_HPP
