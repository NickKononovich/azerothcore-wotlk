-- Instant Poison back to 8.53 PPM: 3.3.5a value (Rawr 2.3.23 Rawr.Rogue/CalculationsRogue.cs:963);
-- the 8.57 PPM of core311 is the WotLK Classic (wowsims) value
UPDATE `spell_enchant_proc_data` SET `PPMChance` = 8.53 WHERE `entry` IN (323, 324, 325, 623, 624, 625, 2641, 3768, 3769);
