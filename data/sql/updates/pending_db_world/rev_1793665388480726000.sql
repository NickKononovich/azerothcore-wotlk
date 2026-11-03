-- Death knight per Rawr 2.3.23 Rawr.TankDK
-- Icy Touch threat: damage x7 (CombatTable.cs:357, :539)
DELETE FROM `spell_threat` WHERE `entry` IN (45477, 49896, 49903, 49904, 49909);
INSERT INTO `spell_threat` (`entry`, `flatMod`, `pctMod`, `apPctMod`) VALUES
(45477, 0, 7, 0),
(49896, 0, 7, 0),
(49903, 0, 7, 0),
(49904, 0, 7, 0),
(49909, 0, 7, 0);
-- Death and Decay: 0.0475 AP per tick (CombatTable.cs:294, :725), was 0.04805
UPDATE `spell_bonus_data` SET `ap_bonus` = 0.0475 WHERE `entry` = 52212;
