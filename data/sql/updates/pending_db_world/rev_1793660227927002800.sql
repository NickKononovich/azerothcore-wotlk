-- Holy Shield (all ranks): 3.3.5a Spell.dbc carries a 0.117 spell power coefficient on the damage effect (= 0.09 * 1.3);
-- Rawr 2.3.23 (Rawr.ProtPaladin/AbilityModel.cs) uses (0.056 AP + 0.09 SP) * 1.3
UPDATE `spell_bonus_data` SET `direct_bonus` = 0.117, `ap_bonus` = 0.0728 WHERE `entry` IN (20925, 20927, 20928, 27179, 48951, 48952);
