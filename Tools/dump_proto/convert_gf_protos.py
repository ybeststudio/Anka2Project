import os
import sys

def convert_item_proto(src_path, dst_path):
    print(f"Converting item_proto: {src_path} -> {dst_path}")
    apply_aliases = {
        'ATT_BONUS_TO_MONSTER': 'APPLY_ATTBONUS_MONSTER',
        'ATT_BONUS_TO_WARRIOR': 'APPLY_ATTBONUS_WARRIOR',
        'ATT_BONUS_TO_ASSASSIN': 'APPLY_ATTBONUS_ASSASSIN',
        'ATT_BONUS_TO_SURA': 'APPLY_ATTBONUS_SURA',
        'ATT_BONUS_TO_SHAMAN': 'APPLY_ATTBONUS_SHAMAN',
        'ATT_BONUS_TO_WOLFMAN': 'APPLY_ATTBONUS_WOLFMAN',
        'ATTBONUS_ELEC': 'APPLY_ENCHANT_ELECT',
        'ATTBONUS_ELECT': 'APPLY_ENCHANT_ELECT',
        'ATTBONUS_FIRE': 'APPLY_ENCHANT_FIRE',
        'ATTBONUS_ICE': 'APPLY_ENCHANT_ICE',
        'ATTBONUS_WIND': 'APPLY_ENCHANT_WIND',
        'ATTBONUS_EARTH': 'APPLY_ENCHANT_EARTH',
        'ATTBONUS_DARK': 'APPLY_ENCHANT_DARK',
        'ATTBONUS_METIN': 'APPLY_ATTBONUS_STONE',
        'RESIST_CRITICAL': 'APPLY_ANTI_CRITICAL_PCT',
        'RESIST_PENETRATE': 'APPLY_ANTI_PENETRATE_PCT',
        'MONSTER_DEFEND_BONUS': 'APPLY_RESIST_MONSTER',
        'ATT_BONUS': 'APPLY_ATT_GRADE_BONUS',
        'DEF_BONUS': 'APPLY_DEF_GRADE_BONUS',
        'MAGIC_ATT_GRADE_BONUS': 'APPLY_MAGIC_ATT_GRADE',
        'MAGIC_DEF_GRADE_BONUS': 'APPLY_MAGIC_DEF_GRADE',
        'MELEE_MAGIC_ATT_BONUS_PER': 'APPLY_MELEE_MAGIC_ATTBONUS_PER',
        'ATTACK_SPEED': 'APPLY_ATT_SPEED',
        'MOVE_SPEED': 'APPLY_MOV_SPEED',
    }

    def clean_apply(val):
        v = val.strip()
        if not v or v == '0' or v == 'NONE':
            return 'APPLY_NONE'
        raw = v[6:] if v.startswith('APPLY_') else v
        if raw in apply_aliases:
            return apply_aliases[raw]
        if v in apply_aliases:
            return apply_aliases[v]
        if not v.startswith('APPLY_') and not v.isdigit():
            return 'APPLY_' + v
        return v

    def clean_limit(val):
        v = val.strip()
        if v == 'NEWWORLD_LEVEL':
            return 'LEVEL'
        if v == 'DURATION':
            return 'REAL_TIME'
        return v

    target_header = [
        'Vnum', 'Name', 'Type', 'SubType', 'Size', 'AntiFlags', 'Flags',
        'WearFlags', 'ImmuneFlags', 'ShopBuyPrice', 'ShopSellPrice', 'RefinedVnum',
        'RefineSet', 'AlterToMagicItemPercent', 'LimitType0', 'LimitValue0',
        'LimitType1', 'LimitValue1', 'ApplyType0', 'ApplyValue0', 'ApplyType1',
        'ApplyValue1', 'ApplyType2', 'ApplyValue2', 'Value0', 'Value1', 'Value2',
        'Value3', 'Value4', 'Value5', 'Specular', 'GainSocketPercent', 'AddonType'
    ]

    count = 0
    with open(src_path, 'r', encoding='latin1') as fin, open(dst_path, 'wb') as fout:
        fin.readline()  # Skip GF header
        fout.write(('\t'.join(target_header) + '\r\n').encode('latin1'))

        for line in fin:
            line = line.strip('\r\n')
            if not line:
                continue
            cols = line.split('\t')
            if len(cols) < 42:
                continue

            vnum = cols[0]
            name = cols[1]
            itype = cols[2].strip()
            isub = cols[3].strip()

            if itype == 'ITEM_BELT':
                itype = 'ITEM_ARMOR'
                isub = 'ARMOR_BELT'
            elif itype == 'ITEM_FORCE':
                itype = 'ITEM_USE'
                isub = 'USE_SPECIAL'
            elif itype == 'ITEM_ALCHEMY':
                itype = 'ITEM_MATERIAL'
                isub = '0'
            elif itype == 'ITEM_TITLE':
                itype = 'ITEM_USE'
            elif itype == 'ITEM_MEDIUM':
                itype = 'ITEM_USE'
            elif itype == 'ITEM_MERCENARY':
                itype = 'ITEM_USE'
            elif itype == '29':
                itype = 'ITEM_DS'

            size = cols[4]
            antiflags = cols[5]
            flags = cols[6]
            wearflags = cols[7]
            immuneflags = cols[8]
            shop_buy = cols[9]
            shop_sell = cols[10]
            refined_vnum = cols[11]
            refine_set = cols[12]
            alter_magic = cols[14]  # cols[13] is 67AttrMaterial, skipped

            lim_type0 = clean_limit(cols[15])
            lim_val0 = cols[16]
            lim_type1 = clean_limit(cols[17])
            lim_val1 = cols[18]

            app_type0 = clean_apply(cols[19])
            app_val0 = cols[20]
            app_type1 = clean_apply(cols[21])
            app_val1 = cols[22]
            app_type2 = clean_apply(cols[23])
            app_val2 = cols[24]
            # cols[25..26] are ApplyType3, ApplyValue3, skipped

            val0 = cols[27]
            val1 = cols[28]
            val2 = cols[29]
            val3 = cols[30]
            val4 = cols[31]
            val5 = cols[32]
            # cols[33..38] are Value6..11, skipped

            specular = cols[39]
            gain_socket = cols[40]
            addon_type = cols[41]

            out_row = [
                vnum, name, itype, isub, size, antiflags, flags,
                wearflags, immuneflags, shop_buy, shop_sell, refined_vnum,
                refine_set, alter_magic, lim_type0, lim_val0,
                lim_type1, lim_val1, app_type0, app_val0, app_type1,
                app_val1, app_type2, app_val2, val0, val1, val2,
                val3, val4, val5, specular, gain_socket, addon_type
            ]
            fout.write(('\t'.join(out_row) + '\r\n').encode('latin1'))
            count += 1

    print(f"Converted {count} items.")

def convert_mob_proto(src_path, dst_path):
    print(f"Converting mob_proto: {src_path} -> {dst_path}")
    target_header = [
        'Vnum', 'Name', 'Rank', 'Type', 'BattleType', 'Level', 'Size',
        'AiFlags', 'MountCapacity', 'RaceFlags', 'ImmuneFlags', 'Empire',
        'Folder', 'OnClick', 'St', 'Dx', 'Ht', 'Iq', 'MinDamage',
        'MaxDamage', 'MaxHp', 'RegenCycle', 'RegenPercent', 'MinGold',
        'MaxGold', 'Exp', 'Def', 'AttackSpeed', 'MoveSpeed',
        'AggressiveHpPct', 'AggressiveSight', 'AttackRange', 'DropItemGroup',
        'ResurrectionVnum', 'EnchantCurse', 'EnchantSlow', 'EnchantPoison',
        'EnchantStun', 'EnchantCritical', 'EnchantPenetrate', 'ResistSword',
        'ResistTwoHanded', 'ResistDagger', 'ResistBell', 'ResistFan',
        'ResistBow', 'ResistFire', 'ResistElect', 'ResistMagic',
        'ResistWind', 'ResistPoison', 'DamMultiply', 'SummonVnum',
        'DrainSp', 'MobColor', 'PolymorphItem', 'SkillLevel0',
        'SkillVnum0', 'SkillLevel1', 'SkillVnum1', 'SkillLevel2',
        'SkillVnum2', 'SkillLevel3', 'SkillVnum3', 'SkillLevel4',
        'SkillVnum4', 'SpBerserk', 'SpStoneSkin', 'SpGodSpeed',
        'SpDeathBlow', 'SpRevive', 'HitRange'
    ]

    count = 0
    with open(src_path, 'r', encoding='latin1') as fin, open(dst_path, 'wb') as fout:
        fin.readline()  # Skip GF header
        fout.write(('\t'.join(target_header) + '\r\n').encode('latin1'))

        for line in fin:
            line = line.strip('\r\n')
            if not line:
                continue
            cols = line.split('\t')
            if len(cols) < 94:
                continue

            vnum = cols[0]
            name = cols[1]
            rank = cols[2]
            mtype = cols[3].strip()

            if mtype == 'PET_PAY':
                mtype = 'PET'
            elif mtype in ('SHOP', 'OBJECT'):
                mtype = 'NPC'

            battle = cols[4]
            level = cols[5]
            # cols[6] is ScalePct
            size = cols[7]
            ai_flags = cols[8]
            mount_cap = cols[9]
            race_flags = cols[10]
            immune_flags = cols[11]
            empire = cols[12]
            folder = cols[13]
            onclick = cols[14]
            st = cols[15]
            dx = cols[16]
            ht = cols[17]
            iq = cols[18]
            # cols[19..22] are SungMaSt..SungMaIq
            min_dam = cols[23]
            max_dam = cols[24]
            max_hp = cols[25]
            regen_cycle = cols[26]
            regen_pct = cols[27]
            min_gold = cols[28]
            max_gold = cols[29]
            exp = cols[30]
            # cols[31] is SungMaExp
            defense = cols[32]
            atk_speed = cols[33]
            mov_speed = cols[34]
            aggr_hp = cols[35]
            aggr_sight = cols[36]
            atk_range = cols[37]
            drop_group = cols[38]
            resurrect_vnum = cols[39]
            ench_curse = cols[40]
            ench_slow = cols[41]
            ench_poison = cols[42]
            ench_stun = cols[43]
            ench_crit = cols[44]
            ench_pen = cols[45]
            # cols[46] is ResistFist
            res_sword = cols[47]
            res_twohand = cols[48]
            res_dagger = cols[49]
            res_bell = cols[50]
            res_fan = cols[51]
            res_bow = cols[52]
            # cols[53] is ResistClaw
            res_fire = cols[54]
            res_elect = cols[55]
            res_magic = cols[56]
            res_wind = cols[57]
            res_poison = cols[58]
            # cols[59..68] are ResistBleeding..ResistEarth
            dam_mult = cols[69]
            summon_vnum = cols[70]
            drain_sp = cols[71]
            mob_color = cols[72]
            polymorph = cols[73]
            sk_lvl0 = cols[74]
            sk_vnum0 = cols[75]
            sk_lvl1 = cols[76]
            sk_vnum1 = cols[77]
            sk_lvl2 = cols[78]
            sk_vnum2 = cols[79]
            sk_lvl3 = cols[80]
            sk_vnum3 = cols[81]
            sk_lvl4 = cols[82]
            sk_vnum4 = cols[83]
            sp_berserk = cols[84]
            sp_stoneskin = cols[85]
            sp_godspeed = cols[86]
            sp_deathblow = cols[87]
            sp_revive = cols[88]
            # cols[89..92] are SpHealer..SpRHpRegen
            hit_range = cols[93]

            out_row = [
                vnum, name, rank, mtype, battle, level, size,
                ai_flags, mount_cap, race_flags, immune_flags, empire,
                folder, onclick, st, dx, ht, iq, min_dam,
                max_dam, max_hp, regen_cycle, regen_pct, min_gold,
                max_gold, exp, defense, atk_speed, mov_speed,
                aggr_hp, aggr_sight, atk_range, drop_group,
                resurrect_vnum, ench_curse, ench_slow, ench_poison,
                ench_stun, ench_crit, ench_pen, res_sword,
                res_twohand, res_dagger, res_bell, res_fan,
                res_bow, res_fire, res_elect, res_magic,
                res_wind, res_poison, dam_mult, summon_vnum,
                drain_sp, mob_color, polymorph, sk_lvl0,
                sk_vnum0, sk_lvl1, sk_vnum1, sk_lvl2,
                sk_vnum2, sk_lvl3, sk_vnum3, sk_lvl4,
                sk_vnum4, sp_berserk, sp_stoneskin, sp_godspeed,
                sp_deathblow, sp_revive, hit_range
            ]
            fout.write(('\t'.join(out_row) + '\r\n').encode('latin1'))
            count += 1

    print(f"Converted {count} mobs.")

def copy_locale_names(src_dir, dst_dir):
    langs = ['cz', 'de', 'en', 'es', 'fr', 'hu', 'it', 'pl', 'pt', 'ro', 'tr']
    for lang in langs:
        s_lang = os.path.join(src_dir, lang)
        d_lang = os.path.join(dst_dir, lang)
        os.makedirs(d_lang, exist_ok=True)
        for fn in ('item_names.txt', 'mob_names.txt'):
            sf = os.path.join(s_lang, fn)
            df = os.path.join(d_lang, fn)
            if os.path.isfile(sf):
                with open(sf, 'rb') as f_in, open(df, 'wb') as f_out:
                    f_out.write(f_in.read())
                print(f"Copied {sf} -> {df}")

if __name__ == '__main__':
    gf_base = r'c:\Anka2Project\Reference\GF-26.1.11\GF-26.1.11_Protos'
    dump_proto_dir = r'c:\Anka2Project\Tools\dump_proto'

    convert_item_proto(
        os.path.join(gf_base, 'item_proto.txt'),
        os.path.join(dump_proto_dir, 'item_proto.txt')
    )

    convert_mob_proto(
        os.path.join(gf_base, 'mob_proto.txt'),
        os.path.join(dump_proto_dir, 'mob_proto.txt')
    )

    copy_locale_names(gf_base, dump_proto_dir)
    print("All conversions and copies complete!")
