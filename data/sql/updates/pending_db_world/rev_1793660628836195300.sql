-- Faerie Fire (Feral) damage: 632 flat bonus threat (Rawr 2.3.23 Rawr.Bear/CalculationsBear.cs:616, wowsims sim/druid/faerie_fire.go:32)
DELETE FROM `spell_threat` WHERE `entry` = 60089;
INSERT INTO `spell_threat` (`entry`, `flatMod`, `pctMod`, `apPctMod`) VALUES
(60089, 632, 1, 0);
