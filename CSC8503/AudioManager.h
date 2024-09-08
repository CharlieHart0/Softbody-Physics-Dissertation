#pragma once
#include <irrKlang.h>
//#include "PlayerCharacter.h"
//#include "GameObject.h"

using namespace irrklang;
//using namespace NCL;

class AudioManager {
public:
    AudioManager();
    ~AudioManager();
    void playBackgroundMusic();
    void playRunSound();
    //void playJumpSound(PlayerCharacter& player);
    void playJumpSound();
    void stopRunSound();
    void playRushSound();
    void playMenuSound();


    void setVolume(float vol);
    //void playJumpSound(PlayerCharacter& player);
    //void playJumpSound();

    /*
    void setVolume(float Volume) {
        *volume = Volume;
    }
    float getVolume() {
        return *volume;
    };
    float* volume;
    */

private:
    ISoundEngine* engine;
    ISound* runningSound;
    ISound* backgroundMusic;
    ISound* rushSound;
    bool isRunningSoundPlaying;
    /*ISoundSource* backgroundMusic;
    ISoundSource* walkSound;
    ISoundSource* jumpSound;*/
};