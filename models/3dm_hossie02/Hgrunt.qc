// Created by Crowbar 0.68

// Stored modelname (without quotes): "valve/models/player/Hgrunt/Hgrunt.mdl"
$modelname "3dm_hossie02.mdl"
$cd "."
$cdtexture "."
$cliptotextures
$scale 1.0

$bodygroup "body"
{
	studio "grunt(reference)"
}

$flags 0


$attachment 0 "Bip01 R Hand" 20 2 5
$attachment 1 "Bip01 R Hand" 15 1.5 3.75
$attachment 2 "Bip01 R Hand" 30 3 7.5

$cbox 0 0 0 0 0 0

$bbox 0 0 0 0 0 0

$hbox 3 "Bip01 Pelvis" -4.69 -4.44 -6.75 4 5.56 6.75
$hbox 6 "Bip01 L Leg" 2.66 -3.69 -3.09 18.16 4.88 3.31
$hbox 6 "Bip01 L Leg1" 0.38 -3.97 -2.84 17.6 4 2.94
$hbox 6 "Bip01 L Foot" -0.59 -2.34 -2.63 3.79 8 2.19
$hbox 7 "Bip01 R Leg" 2.47 -3.69 -3.16 18.129999 4.88 3.38
$hbox 7 "Bip01 R Leg1" 0.31 -3.97 -2.84 17.6 3.94 2.97
$hbox 7 "Bip01 R Foot" -0.56 -2.34 -2.19 3.81 8 2.66
$hbox 3 "Bip01 Spine1" -3.25 -5.5 -5.5 4.83 5.5 5.5
$hbox 2 "Bip01 Spine2" -0.06 -5.53 -7.59 8 7 7.59
$hbox 2 "Bip01 Spine3" -2.25 -6.81 -6.31 6.5 5.09 6.31
$hbox 2 "Bip01 Neck" -3.11 -1.5 -3 2.05 3.5 3
$hbox 1 "Bip01 Head" 0.09 -3.66 -3 8.41 5.09 3
$hbox 4 "Bip01 L Arm" 0.94 -2.88 -4.13 5.44 4.28 3.5
$hbox 4 "Bip01 L Arm1" -2.16 -2.34 -2.56 11.56 3.41 2.38
$hbox 4 "Bip01 L Arm2" 0.59 -1.81 -2.19 10.75 2.84 2.41
$hbox 4 "Bip01 L Hand" 0 -1 -2 3 1.5 3.5
$hbox 5 "Bip01 R Arm" 1.38 -2.84 -3.56 5.69 4.31 4.31
$hbox 5 "Bip01 R Arm1" -1.88 -2.47 -2.16 11.81 3.28 2.81
$hbox 5 "Bip01 R Arm2" 0.44 -1.97 -2.34 10.59 2.72 2.22
$hbox 5 "Bip01 R Hand" 0 -1 -2 3 1.5 3.5

$controller 0 "Bip01 Spine" XR -30 30
$controller 1 "Bip01 Spine1" XR -30 30
$controller 2 "Bip01 Spine2" XR -30 30
$controller 3 "Bip01 Spine3" XR -30 30

$sequence "idle1" {
	"Hgrunt_anims\look_idle"
	ACT_IDLE 2
	fps 14
	loop
}
$sequence "idle" {
	"Hgrunt_anims\idle"
	ACT_IDLE 1
	fps 14
	loop
}
$sequence "deep_idle" {
	"Hgrunt_anims\deep_idle"
	ACT_IDLE 4
	fps 12
	loop
}
$sequence "run2" {
	"Hgrunt_anims\run2"
	ACT_RUN 1
	fps 40
	loop
	LX
}
$sequence "walk2handed" {
	"Hgrunt_anims\walk2handed"
	ACT_WALK 1
	fps 26
	loop
	LX
}
$sequence "2handshoot" {
	"Hgrunt_anims\2handshoot"
	ACT_RANGE_ATTACK1 1
	fps 20
}
$sequence "crawl" {
	"Hgrunt_anims\crawl"
	ACT_CROUCH 1
	fps 22
	loop
	LX
}
$sequence "crouch_idle" {
	"Hgrunt_anims\crouch_idle"
	ACT_CROUCHIDLE 1
	fps 12
	loop
}
$sequence "jump" {
	"Hgrunt_anims\jump"
	ACT_HOP 1
	fps 30
}
$sequence "long_jump" {
	"Hgrunt_anims\long_jump"
	ACT_LEAP 1
	fps 24
	LX
}
$sequence "swim" {
	"Hgrunt_anims\swim"
	ACT_SWIM 1
	fps 14
	loop
}
$sequence "treadwater" {
	"Hgrunt_anims\treadwater"
	ACT_HOVER 1
	fps 14
	loop
}
$sequence "run" {
	"Hgrunt_anims\run"
	fps 31
	loop
	LX
}
$sequence "walk" {
	"Hgrunt_anims\walk"
	fps 31
	loop
	LX
}
$sequence "aim_2" {
	"Hgrunt_anims\aim_2_blend01"
	"Hgrunt_anims\aim_2_blend02"
	blend XR -45 45
	fps 14
	loop
}
$sequence "shoot_2" {
	"Hgrunt_anims\shoot_2_blend01"
	"Hgrunt_anims\shoot_2_blend02"
	blend XR -45 45
	fps 14
	loop
}
$sequence "aim_1" {
	"Hgrunt_anims\aim_1_blend01"
	"Hgrunt_anims\aim_1_blend02"
	blend XR -45 45
	fps 14
	loop
}
$sequence "shoot_1" {
	"Hgrunt_anims\shoot_1_blend01"
	"Hgrunt_anims\shoot_1_blend02"
	blend XR -45 45
	fps 14
	loop
}
$sequence "die_simple" {
	"Hgrunt_anims\die_simple"
	ACT_DIESIMPLE 1
	{ event 2001 9 }
	fps 22
}
$sequence "die_backwards1" {
	"Hgrunt_anims\die_backwards1"
	ACT_DIEBACKWARD 4
	{ event 2001 13 }
	fps 26
}
$sequence "die_backwards" {
	"Hgrunt_anims\die_backwards"
	ACT_DIEBACKWARD 1
	{ event 2001 15 }
	fps 30
}
$sequence "die_forwards" {
	"Hgrunt_anims\die_forwards"
	ACT_DIEFORWARD 1
	{ event 2001 8 }
	fps 26
}
$sequence "headshot" {
	"Hgrunt_anims\headshot"
	ACT_DIE_HEADSHOT 4
	{ event 2001 22 }
	fps 24
}
$sequence "die_spin" {
	"Hgrunt_anims\die_spin"
	ACT_DIE_HEADSHOT 1
	{ event 2001 16 }
	fps 30
}
$sequence "gutshot" {
	"Hgrunt_anims\gutshot"
	ACT_DIE_GUTSHOT 1
	{ event 2001 22 }
	fps 26
}
$sequence "ref_aim_crowbar" {
	"Hgrunt_anims\ref_aim_crowbar_blend01"
	"Hgrunt_anims\ref_aim_crowbar_blend02"
	blend XR -45 45
	fps 12
	loop
}
$sequence "ref_shoot_crowbar" {
	"Hgrunt_anims\ref_shoot_crowbar_blend01"
	"Hgrunt_anims\ref_shoot_crowbar_blend02"
	blend XR -45 45
	fps 24
}
$sequence "crouch_aim_crowbar" {
	"Hgrunt_anims\crouch_aim_crowbar_blend01"
	"Hgrunt_anims\crouch_aim_crowbar_blend02"
	blend XR -45 45
	fps 12
	loop
}
$sequence "crouch_shoot_crowbar" {
	"Hgrunt_anims\crouch_shoot_crowbar_blend01"
	"Hgrunt_anims\crouch_shoot_crowbar_blend02"
	blend XR -45 45
	fps 24
}
$sequence "ref_aim_trip" {
	"Hgrunt_anims\ref_aim_trip_blend01"
	"Hgrunt_anims\ref_aim_trip_blend02"
	blend XR -45 45
	fps 12
	loop
}
$sequence "ref_shoot_trip" {
	"Hgrunt_anims\ref_shoot_trip_blend01"
	"Hgrunt_anims\ref_shoot_trip_blend02"
	blend XR -45 45
	fps 18
}
$sequence "crouch_aim_trip" {
	"Hgrunt_anims\crouch_aim_trip_blend01"
	"Hgrunt_anims\crouch_aim_trip_blend02"
	blend XR -45 45
	fps 12
	loop
}
$sequence "crouch_shoot_trip" {
	"Hgrunt_anims\crouch_shoot_trip_blend01"
	"Hgrunt_anims\crouch_shoot_trip_blend02"
	blend XR -45 45
	fps 16
}
$sequence "ref_aim_onehanded" {
	"Hgrunt_anims\ref_aim_onehanded_blend01"
	"Hgrunt_anims\ref_aim_onehanded_blend02"
	blend XR -50 35
	fps 12
	loop
}
$sequence "ref_shoot_onehanded" {
	"Hgrunt_anims\ref_shoot_onehanded_blend01"
	"Hgrunt_anims\ref_shoot_onehanded_blend02"
	blend XR -50 35
	{ event 5011 0 "21" }
	fps 16
}
$sequence "crouch_aim_onehanded" {
	"Hgrunt_anims\crouch_aim_onehanded_blend01"
	"Hgrunt_anims\crouch_aim_onehanded_blend02"
	blend XR -50 35
	fps 12
	loop
}
$sequence "crouch_shoot_onehanded" {
	"Hgrunt_anims\crouch_shoot_onehanded_blend01"
	"Hgrunt_anims\crouch_shoot_onehanded_blend02"
	blend XR -50 35
	{ event 5011 0 "21" }
	fps 16
}
$sequence "ref_aim_python" {
	"Hgrunt_anims\ref_aim_python_blend01"
	"Hgrunt_anims\ref_aim_python_blend02"
	blend XR -50 35
	fps 12
	loop
}
$sequence "ref_shoot_python" {
	"Hgrunt_anims\ref_shoot_python_blend01"
	"Hgrunt_anims\ref_shoot_python_blend02"
	blend XR -50 35
	{ event 5011 0 "31" }
	fps 16
}
$sequence "crouch_aim_python" {
	"Hgrunt_anims\crouch_aim_python_blend01"
	"Hgrunt_anims\crouch_aim_python_blend02"
	blend XR -50 35
	fps 12
	loop
}
$sequence "crouch_shoot_python" {
	"Hgrunt_anims\crouch_shoot_python_blend01"
	"Hgrunt_anims\crouch_shoot_python_blend02"
	blend XR -50 35
	{ event 5011 0 "31" }
	fps 16
}
$sequence "ref_aim_shotgun" {
	"Hgrunt_anims\ref_aim_shotgun_blend01"
	"Hgrunt_anims\ref_aim_shotgun_blend02"
	blend XR -45 45
	fps 12
	loop
}
$sequence "ref_shoot_shotgun" {
	"Hgrunt_anims\ref_shoot_shotgun_blend01"
	"Hgrunt_anims\ref_shoot_shotgun_blend02"
	blend XR -45 45
	{ event 5021 0 "51" }
	fps 15
}
$sequence "crouch_aim_shotgun" {
	"Hgrunt_anims\crouch_aim_shotgun_blend01"
	"Hgrunt_anims\crouch_aim_shotgun_blend02"
	blend XR -45 45
	fps 12
	loop
}
$sequence "crouch_shoot_shotgun" {
	"Hgrunt_anims\crouch_shoot_shotgun_blend01"
	"Hgrunt_anims\crouch_shoot_shotgun_blend02"
	blend XR -45 45
	{ event 5021 0 "51" }
	fps 15
}
$sequence "ref_aim_gauss" {
	"Hgrunt_anims\ref_aim_gauss_blend01"
	"Hgrunt_anims\ref_aim_gauss_blend02"
	blend XR -45 45
	fps 12
	loop
}
$sequence "ref_shoot_gauss" {
	"Hgrunt_anims\ref_shoot_gauss_blend01"
	"Hgrunt_anims\ref_shoot_gauss_blend02"
	blend XR -45 45
	fps 15
}
$sequence "crouch_aim_gauss" {
	"Hgrunt_anims\crouch_aim_gauss_blend01"
	"Hgrunt_anims\crouch_aim_gauss_blend02"
	blend XR -45 45
	fps 12
	loop
}
$sequence "crouch_shoot_gauss" {
	"Hgrunt_anims\crouch_shoot_gauss_blend01"
	"Hgrunt_anims\crouch_shoot_gauss_blend02"
	blend XR -45 45
	fps 15
}
$sequence "ref_aim_mp5" {
	"Hgrunt_anims\ref_aim_mp5_blend01"
	"Hgrunt_anims\ref_aim_mp5_blend02"
	blend XR -45 45
	fps 12
	loop
}
$sequence "ref_shoot_mp5" {
	"Hgrunt_anims\ref_shoot_mp5_blend01"
	"Hgrunt_anims\ref_shoot_mp5_blend02"
	blend XR -45 45
	{ event 5001 0 "40" }
	fps 20
	loop
}
$sequence "crouch_aim_mp5" {
	"Hgrunt_anims\crouch_aim_mp5_blend01"
	"Hgrunt_anims\crouch_aim_mp5_blend02"
	blend XR -30 40
	fps 12
	loop
}
$sequence "crouch_shoot_mp5" {
	"Hgrunt_anims\crouch_shoot_mp5_blend01"
	"Hgrunt_anims\crouch_shoot_mp5_blend02"
	blend XR -30 40
	{ event 5001 0 "40" }
	fps 20
	loop
}
$sequence "ref_aim_rpg" {
	"Hgrunt_anims\ref_aim_rpg_blend01"
	"Hgrunt_anims\ref_aim_rpg_blend02"
	blend XR -45 40
	fps 12
	loop
}
$sequence "ref_shoot_rpg" {
	"Hgrunt_anims\ref_shoot_rpg_blend01"
	"Hgrunt_anims\ref_shoot_rpg_blend02"
	blend XR -45 40
	fps 18
}
$sequence "crouch_aim_rpg" {
	"Hgrunt_anims\crouch_aim_rpg_blend01"
	"Hgrunt_anims\crouch_aim_rpg_blend02"
	blend XR -45 40
	fps 12
	loop
}
$sequence "crouch_shoot_rpg" {
	"Hgrunt_anims\crouch_shoot_rpg_blend01"
	"Hgrunt_anims\crouch_shoot_rpg_blend02"
	blend XR -45 40
	fps 18
}
$sequence "ref_aim_egon" {
	"Hgrunt_anims\ref_aim_egon_blend01"
	"Hgrunt_anims\ref_aim_egon_blend02"
	blend XR -45 35
	fps 12
	loop
}
$sequence "ref_shoot_egon" {
	"Hgrunt_anims\ref_shoot_egon_blend01"
	"Hgrunt_anims\ref_shoot_egon_blend02"
	blend XR -45 35
	fps 18
	loop
}
$sequence "crouch_aim_egon" {
	"Hgrunt_anims\crouch_aim_egon_blend01"
	"Hgrunt_anims\crouch_aim_egon_blend02"
	blend XR -45 45
	fps 12
	loop
}
$sequence "crouch_shoot_egon" {
	"Hgrunt_anims\crouch_shoot_egon_blend01"
	"Hgrunt_anims\crouch_shoot_egon_blend02"
	blend XR -45 45
	fps 18
	loop
}
$sequence "ref_aim_squeak" {
	"Hgrunt_anims\ref_aim_squeak_blend01"
	"Hgrunt_anims\ref_aim_squeak_blend02"
	blend XR -45 45
	fps 12
	loop
}
$sequence "ref_shoot_squeak" {
	"Hgrunt_anims\ref_shoot_squeak_blend01"
	"Hgrunt_anims\ref_shoot_squeak_blend02"
	blend XR -45 45
	fps 14
}
$sequence "crouch_aim_squeak" {
	"Hgrunt_anims\crouch_aim_squeak_blend01"
	"Hgrunt_anims\crouch_aim_squeak_blend02"
	blend XR -45 45
	fps 12
	loop
}
$sequence "crouch_shoot_squeak" {
	"Hgrunt_anims\crouch_shoot_squeak_blend01"
	"Hgrunt_anims\crouch_shoot_squeak_blend02"
	blend XR -45 45
	fps 14
}
$sequence "ref_aim_hive" {
	"Hgrunt_anims\ref_aim_hive_blend01"
	"Hgrunt_anims\ref_aim_hive_blend02"
	blend XR -45 45
	fps 12
	loop
}
$sequence "ref_shoot_hive" {
	"Hgrunt_anims\ref_shoot_hive_blend01"
	"Hgrunt_anims\ref_shoot_hive_blend02"
	blend XR -45 45
	fps 15
}
$sequence "crouch_aim_hive" {
	"Hgrunt_anims\crouch_aim_hive_blend01"
	"Hgrunt_anims\crouch_aim_hive_blend02"
	blend XR -45 45
	fps 12
	loop
}
$sequence "crouch_shoot_hive" {
	"Hgrunt_anims\crouch_shoot_hive_blend01"
	"Hgrunt_anims\crouch_shoot_hive_blend02"
	blend XR -45 45
	fps 15
}
$sequence "ref_aim_bow" {
	"Hgrunt_anims\ref_aim_bow_blend01"
	"Hgrunt_anims\ref_aim_bow_blend02"
	blend XR -45 45
	fps 12
	loop
}
$sequence "ref_shoot_bow" {
	"Hgrunt_anims\ref_shoot_bow_blend01"
	"Hgrunt_anims\ref_shoot_bow_blend02"
	blend XR -45 45
	fps 16
}
$sequence "crouch_aim_bow" {
	"Hgrunt_anims\crouch_aim_bow_blend01"
	"Hgrunt_anims\crouch_aim_bow_blend02"
	blend XR -45 45
	fps 12
	loop
}
$sequence "crouch_shoot_bow" {
	"Hgrunt_anims\crouch_shoot_bow_blend01"
	"Hgrunt_anims\crouch_shoot_bow_blend02"
	blend XR -45 45
	fps 16
}
$sequence "deadback" {
	"Hgrunt_anims\deadback"
	fps 10
}
$sequence "deadsitting" {
	"Hgrunt_anims\deadsitting"
	fps 10
}
$sequence "deadstomach" {
	"Hgrunt_anims\deadstomach"
	fps 10
}
$sequence "deadtable" {
	"Hgrunt_anims\deadtable"
	fps 10
}
