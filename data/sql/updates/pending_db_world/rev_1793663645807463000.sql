-- Warrior / bear bonus threat per Rawr 2.3.23
-- Shield Slam: Rawr.ProtWarr AbilityModel.cs:166-168 threat = (damage + 770) x 1.3, so the flat bonus takes the x1.3 too
UPDATE `spell_threat` SET `flatMod` = 296 WHERE `entry` = 23922;
UPDATE `spell_threat` SET `flatMod` = 554 WHERE `entry` = 30356;
UPDATE `spell_threat` SET `flatMod` = 845 WHERE `entry` = 47487;
UPDATE `spell_threat` SET `flatMod` = 1001 WHERE `entry` = 47488;
-- Cleave rank 8: Rawr.ProtWarr AbilityModel.cs:132 +225 (was 112)
UPDATE `spell_threat` SET `flatMod` = 225 WHERE `entry` = 47520;
-- Maul rank 10: Rawr.Bear CalculationsBear.cs:613 +424 (was 422)
UPDATE `spell_threat` SET `flatMod` = 424 WHERE `entry` = 48480;
-- Devastate rank 5: Rawr.ProtWarr AbilityModel.cs:137-141 +315 + 5 % AP (rank 1 row had no flat bonus)
DELETE FROM `spell_threat` WHERE `entry` = 47498;
INSERT INTO `spell_threat` (`entry`, `flatMod`, `pctMod`, `apPctMod`) VALUES
(47498, 315, 1, 0.05);
