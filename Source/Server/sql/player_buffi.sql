-- Buffi (support shaman) state, one row per owner. Loaded by CBuffiActor::Load, saved with REPLACE INTO.
CREATE TABLE IF NOT EXISTS `player`.`player_buffi` (
  `pid` int(10) unsigned NOT NULL,
  `name` varchar(24) NOT NULL DEFAULT 'Buffi',
  `sex` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `level` tinyint(3) unsigned NOT NULL DEFAULT 1,
  `exp` int(10) unsigned NOT NULL DEFAULT 0,
  `evolution` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `duration_end` int(11) NOT NULL DEFAULT 0,
  `duration_perm` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `skill_points` smallint(5) unsigned NOT NULL DEFAULT 0,
  `rune0` int(11) NOT NULL DEFAULT 0,
  `rune1` int(11) NOT NULL DEFAULT 0,
  `rune2` int(11) NOT NULL DEFAULT 0,
  `skill0` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `skill1` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `skill2` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `skill3` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `skill4` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `costume_body` int(10) unsigned NOT NULL DEFAULT 0,
  `costume_hair` int(10) unsigned NOT NULL DEFAULT 0,
  `costume_weapon` int(10) unsigned NOT NULL DEFAULT 0,
  `costume_sash` int(10) unsigned NOT NULL DEFAULT 0,
  `skill_color` varchar(320) NOT NULL DEFAULT '',
  PRIMARY KEY (`pid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- added later (skill colour system):
-- ALTER TABLE `player`.`player_buffi` ADD COLUMN `skill_color` varchar(320) NOT NULL DEFAULT '' AFTER `costume_sash`;
ALTER TABLE `player`.`player_buffi` ADD COLUMN `skill_book` varchar(32) NOT NULL DEFAULT '' AFTER `skill_color`;
