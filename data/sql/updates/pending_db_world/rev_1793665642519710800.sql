-- Hunter pet level 80 base stats per Rawr 2.3.23 Rawr.Hunter Pet/PetCalculations.cs:56-62
-- (331 str / 113 agi / 361 sta / 65 int / 10 spi; was 192 / 158 / 356 / 70 / 115)
UPDATE `pet_levelstats` SET `str` = 331, `agi` = 113, `sta` = 361, `inte` = 65, `spi` = 10 WHERE `creature_entry` = 1 AND `level` = 80;
