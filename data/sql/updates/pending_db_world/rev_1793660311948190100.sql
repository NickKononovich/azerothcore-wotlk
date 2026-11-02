-- Regrowth direct heal: 0.538 spell power as in Spell.dbc EffectBonusMultiplier (ranks 3-12); the TBC-era 0.3 is gone
UPDATE `spell_bonus_data` SET `direct_bonus` = 0.538 WHERE `entry` IN (8939, 8940, 8941, 9750, 9856, 9857, 9858, 26980, 48442, 48443);
