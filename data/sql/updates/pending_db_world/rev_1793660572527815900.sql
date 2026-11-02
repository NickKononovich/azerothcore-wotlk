-- Revert core301 Black Arrow part: 3.3.5a Black Arrow scales with 0.1 RAP over 5 ticks (0.02 per tick)
-- (DBC 63672 tooltip $RAP*0.1, Rawr 2.3.23 Rawr.Hunter/Skills.Shots.cs:266); wowsims' 0.023 is Wrath Classic testing
UPDATE `spell_bonus_data` SET `ap_dot_bonus` = 0.02 WHERE `entry` IN (3674, 63668, 63669, 63670, 63671, 63672);
