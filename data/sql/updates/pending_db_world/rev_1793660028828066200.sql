-- Rogue poisons follow wowsims (sim/rogue/poisons.go): Instant Poison 0.09 AP,
-- Deadly Poison 0.027 AP per tick per dose, Wound Poison 0.036 AP,
-- Instant Poison 0.2 / (1.4 / 60) = 8.57 PPM
UPDATE `spell_bonus_data` SET `ap_bonus` = 0.09 WHERE `entry` IN (8680, 57965);
UPDATE `spell_bonus_data` SET `ap_dot_bonus` = 0.027 WHERE `entry` IN (2818, 57970);
UPDATE `spell_bonus_data` SET `ap_bonus` = 0.036 WHERE `entry` = 57975;
UPDATE `spell_enchant_proc_data` SET `PPMChance` = 8.57 WHERE `entry` IN (323, 324, 325, 623, 624, 625, 2641, 3768, 3769);
