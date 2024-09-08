#include "AudioManager.h"
#include "DebugOptions.h"
#include <iostream>
//#include "../NCLCoreClasses/Window.h"
//#include"../CSC8503CoreClasses/GameWorld.h"

using namespace irrklang;
//using namespace NCL;
//using namespace CSC8503;
//using namespace NCL::CSC8503;
// setVolume(0.5f);
AudioManager::AudioManager() {
    int coutFlag = 0x20;
    if (DEBUG_PRINT_TO_CONSOLE) { coutFlag = 0; }
    engine = createIrrKlangDevice(ESOD_AUTO_DETECT,61- coutFlag);

    if (!engine) {
        //std::cout << "Failed to initialize irrKlang engine!" << std::endl;
        return;
    }
    runningSound = nullptr;
    isRunningSoundPlaying = false;
}

AudioManager::~AudioManager() {
    if (engine) {
        engine->drop();
    }
}

void AudioManager::playMenuSound() {
    return;
}

void AudioManager::playBackgroundMusic() {
    //TODO remove this
    return;
}

void AudioManager::playRunSound() {
    //TODO remove this
    return;
}

void AudioManager::stopRunSound() {
    if (isRunningSoundPlaying && runningSound != nullptr) {
        runningSound->setIsPaused(true);
        runningSound = nullptr;
        isRunningSoundPlaying = false;
    }
}


void AudioManager::playJumpSound() {
    return;
    
}

void AudioManager::playRushSound() {
    return;
}

void AudioManager::setVolume(float vol) {
    engine->setSoundVolume(vol);
}

//void AudioManager::playJumpSound(PlayerCharacter& player) {
//    if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE) && player.Jump()) {
//        engine->play2D("../Assets/Sounds/Jump.mp3", false);
//    }
//}

//void AudioManager::playJumpSound() {
//    PlayerCharacter playerCharacter;
//
//    if (playerCharacter.Jump()) {
//        engine->play2D("../Assets/Sounds/Jump.mp3", false);
//    }
//}