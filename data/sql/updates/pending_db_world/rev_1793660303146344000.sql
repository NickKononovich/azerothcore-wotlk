-- Moonfire initial hit: 0.15 spell power (wowsims sim/druid/moonfire.go:79, Spell.dbc EffectBonusMultiplier of the direct effect); the ticks stay at 0.13
UPDATE `spell_bonus_data` SET `direct_bonus` = 0.15 WHERE `entry` IN (8926, 8927, 8928, 8929, 9833, 9834, 9835, 26987, 26988, 48462, 48463);
