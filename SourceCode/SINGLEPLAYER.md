# Singleplayer mode
AG 6.7 introduces a command to enable playing singleplayer maps/campaigns. It's not enabled by default, so those that are not speedrunners don't get confused or unexpected behaviours in their listenserver.
So now you can just type `sv_singleplayer 1`, reload a map with `map <name>` and there you go: triggers and other entities will work as in HL Steam.
You have to be in a map (ingame) when setting `sv_singleplayer` for it to work, and then reload the map. Alternatively, you can also set it in `ag/startup_server.cfg`
which is automatically executed right before the first map loads, thus you don't have to reload it.

Singleplayer in AG has some quirks. It uses most of the multiplayer features, like vertical gauss boosting, constant 10hp falldamage...
and some more stuff not related to multiplayer, like dead corpses being non-solid, fixes for some common crashes... you can find the changes below.

If you want to run the AG% speedrunning category, there's this [AG speedrunning package](https://drive.google.com/open?id=1hSErqgUqDmbJCYJZLAHsrfC6NziW1494) that includes some fixes to the HL1 campaign maps to make them more compatible with AG and have nicer speedruns. You need AG installed, and then replace the files in your installation with the ones from the package, but make sure you backup your files just in case. You'll also need to inject the BunnymodXT included in the speedrunning package (backup your current one or put this somewhere else), which is not
merged yet with the [official BunnymodXT](https://github.com/YaLTeR/BunnymodXT), but it will be in the future after more testing and some fixes that are pending (e.g.: `bxt_autopause` not working on AG apparently).
You inject it the same way we do for HL1 speedrunning. There's a `speedrun.cfg` in there for the required cvars to run AG%, an `example.cfg` to check some useful cvars and aliases, and some demos to learn the first chapter of the game, that you can replay with `startdemos start_1 start_2 start_3 start_4 start_5 start_6`.

## Changelog
These are the changes to singleplayer in AG 6.7 when compared to HL Steam, and some are specific to the HL campaign in the speedrunning package.
- Fixed hornet-related crash.
- Fixed weapon-drop-related crash.
- Fixed satchel softlock.
- Fixed getting stuck on slopes at high FPS.
- Fixed On A Rail collisions (c2a2a ~ c2a2f).
- Fixed the collisions at the start of Residue Processing (at c2a4).
- Fixed collision with the top of slopes (at c2a4d).
- Fixed getting too much damage when hurt during changelevel.
- Fixed scientist on QE disappearing when going through changelevel.
- Fixed scientist on QE going too far upstairs (at c2a4e).
- Fixed grunt on QE going downstairs and scaring the scientist (at c2a4f).
- Fixed WGH door getting closed.
- Made it harder for scientists to get scared of enemies while following you, even with high HP (>50).
- Made it possible to hold the gauss charge when going through changelevel triggers.
- Made it possible to hold the gauss charge through saveloads, in a tricky way that takes some practice to perform correctly, but works.
- Removed Game Over triggered by damaging a scientist (at c3a2 and c3a2d).
- Added transparency to the fade in effect at the start of UC.
- Moved further the scientist that may get into your path when finishing the first map of AM (at c1a0).
- Removed some scripted sequences for the scientist at the start of the 2nd map of AM (c1a0d), so it doesn't get in your way.
- Removed glass on the doors at the end of the second map of AM (c1a0d), as starting with HEV suit disables doing that part normally.
- Leechs (worms in Apprehension) are now less annoying.
- Dead corpses are now non-solid.
- Now grunts always drop their items upon death, even when killed with a direct hit of grenade.
- Gman is now killable (it has no death animation though).
- Sped up TC cutscene.
- Cockroaches are now non-solid.
- Slightly improved accuracy with crowbar.
- Decreased crowbar attack delay on pickup.
- Decreased satchel deploy time.
- Removed direct self-gauss.
- Fall damage is now always 10hp, so it doesn't depend on fall height.
- Overall increased damage dealt with weapons.
- Enabled vertical gauss boost.
- Lowered the time to charge gauss fully from 4s to 1.5s.
- Lowered ammo burning factor for gauss, so it takes a bit less charge to get the max. boost.
- Now crowbar always deals full damage (subsequent swings after the first one dealt half the damage before).
- Now items reappear after a while and chargers get refilled too.
- Halved the spread for 9mmAR.
- Halved the vertical spread for shotgun.
- Enabled 357 zoom.
- Lowered egon ammo usage from 3 ammo/s to 1 ammo/s.
- Increased egon damage radius.
- Enabled crossbow zoomed hitscan and explosive projectile attacks.
- Doubled the RPG ammo received upon pickup.
- Now weapons can be dropped.
- HEV chargers now give at maximum 30 AP.
- Now you start with crowbar and glock in the campaign or map.
- Now scientists and barneys have +50 and +30 hp respectively, to be balanced with the increased weapon damage.

## Known issues
Some of these are specific to the HL campaign.
- Prestrafing is slower. This is because in AG we use `sv_maxspeed 300` by default. This won't change, as this is AG after all, not HL.
- Ladder physics is different, it's a bit harder to stop climbing ladders, but it's a matter of getting used to it. This won't change, as this is AG after all, not HL.
- It's hard to keep the gauss charge after a saveload. Well, after some practice it's easy. All you have to stop holding right click during the load and when it's done loading you start holding it again, and only then you can start moving or pressing any other key. So within a few frames you can already be moving normally while also holding the charge. This will probably not be fixed.
- Charging gauss makes no sound after a saveload. This will probably not be fixed.
- If you go through a load pressing some key and you keep holding the same key after the load, it will keep that state even if you stop pressing that key. This bug exists because of the fix for keeping gauss charge through changelevels. It's not critical because you can just press some other key to avoid that from happening, and you won't notice it if you're bhopping because you're pressing A/D to keep strafing, which cancels the bug as you're pressing and releasing different keys. So, if you notice that this is happening, just press some other key that you weren't pressing during the load. This issue will continue to exist, probably until we find a fix for the issue related to keeping gauss charge during loads.
- The scientist at the start of Lambda Core, where you kill 4 alien grunts, hardly ever will cancel his scanner reading animation when shot with gauss through the door. This might be fixed in the future, but for the moment you can avoid shooting the gauss, and instead shoot him with MP5 as soon as the door opens, so he cancels the animation and goes to open the back door, only losing 1-2 seconds compared to HL speedruns.
- Some voices in TC overlap, I don't like them overlapping, but it's not that important. This will probably be fixed.
- Banners in the 3rd map of AM don't rotate, like the banner where you can see "PLAY AG - PLAY IT NOW". This will probably not be fixed.
