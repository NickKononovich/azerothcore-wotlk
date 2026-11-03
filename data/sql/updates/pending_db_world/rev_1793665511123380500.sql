-- Balance druid per Rawr 2.3.23 Rawr.Moonkin
-- Omen of Clarity: 6 % per spell cast (SpellRotation.cs:197), 3.5 PPM on white swings; rolled in spell_dru_omen_of_clarity
UPDATE `spell_proc` SET `ProcsPerMinute` = 0, `Chance` = 100 WHERE `SpellId` = 16864;
-- Moonkin Form: every spell crit returns 2 % mana (SpellRotation.cs:186, 256), was 15 PPM by cast time
UPDATE `spell_proc` SET `ProcsPerMinute` = 0, `Chance` = 100 WHERE `SpellId` = 24905;
-- Starfall splash also hits the star's own target (MoonkinSolver.cs:657-675)
DELETE FROM `spell_script_names` WHERE `spell_id` = -50294 AND `ScriptName` = 'spell_dru_starfall_aoe';
