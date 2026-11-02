-- Holy Light / Flash of Light (max-level-coefficient ranks): use the 3.3.5a Spell.dbc coefficients (1.679 / 1.009)
-- instead of the rounded 1.66 / 1.0; Rawr 2.3.23 Rawr.Healadin/Spell.cs: 2.5/3.5*66/35*1.25 = 1.684, 1.5/3.5*66/35*1.25 = 1.010
UPDATE `spell_bonus_data` SET `direct_bonus` = 1.679 WHERE `entry` IN (1026, 1042, 3472, 10328, 10329, 25292, 27135, 27136, 48781, 48782);
UPDATE `spell_bonus_data` SET `direct_bonus` = 1.009 WHERE `entry` IN (19750, 19939, 19940, 19941, 19942, 19943, 27137, 48784, 48785);
