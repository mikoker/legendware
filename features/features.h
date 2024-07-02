#pragma once
#include "..\protect\crypt_ptr.h"

class Aim;
class Animations;
class AntiAim;
class BuildBones;
class Chams;
class Dormant_esp;
class BulletTracer;
class Esp;
class Exploits;
class FakeLag;
class CGrenadePrediction;
class KnifeAim;
class LegitAim;
class LocalAnimations;
class Logs;
class Misc;
class Movement;
class Hit_chams;
class Penetration;
class Prediction;
class Resolver;
class SoundEsp;
class SpectatorList;
class ThirdPerson;
class Visuals;
class WorldColor;
class WorldEsp;
class Preview;
class Changer;
class CheatCommunication;
class Skin_Preview;

extern crypt_ptr <Aim> aim;
extern crypt_ptr <Animations> animations;
extern crypt_ptr <AntiAim> anti_aim;
extern crypt_ptr <BuildBones> build_bones;
extern crypt_ptr <Chams> chams;
extern crypt_ptr <Dormant_esp> dormant;
extern crypt_ptr <BulletTracer> bullet_tracer;
extern crypt_ptr <Esp> esp;
extern crypt_ptr <Exploits> exploits;
extern crypt_ptr <FakeLag> fake_lag;
extern crypt_ptr <CGrenadePrediction> grenade_prediction;
extern crypt_ptr <KnifeAim> knife_aim;
extern crypt_ptr <LegitAim> legit_aim;
extern crypt_ptr <LocalAnimations> local_animations;
extern crypt_ptr <Logs> logs;
extern crypt_ptr <Misc> misc;
extern crypt_ptr <Movement> movement_system;
extern crypt_ptr <Hit_chams> hit_chams;
extern crypt_ptr <Penetration> penetration;
extern crypt_ptr <Prediction> engine_prediction;
extern crypt_ptr <Resolver> resolver[65];
extern crypt_ptr <SoundEsp> sound_esp;
extern crypt_ptr <SpectatorList> spectator_list;
extern crypt_ptr <ThirdPerson> third_person;
extern crypt_ptr <Visuals> visuals;
extern crypt_ptr <WorldColor> world_color;
extern crypt_ptr <WorldEsp> world_esp;
extern crypt_ptr <Preview> preview;

extern crypt_ptr <Skin_Preview> skins_preview;

extern crypt_ptr <Changer> skinchanger;
extern crypt_ptr <CheatCommunication> cheat_communication;