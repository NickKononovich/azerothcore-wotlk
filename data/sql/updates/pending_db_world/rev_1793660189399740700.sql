-- Devouring Plague: the periodic spell power coefficient is 18.5% per tick (Spell.dbc 3.3.5a
-- EffectBonusMultiplier 0.185 on every rank), the rows held the older 0.18.
UPDATE `spell_bonus_data` SET `dot_bonus` = 0.185 WHERE `entry` IN (2944, 48300);
