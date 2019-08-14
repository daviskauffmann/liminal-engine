#ifndef GAME_SOURCE_H
#define GAME_SOURCE_H

class Source
{
public:
    Source();
    ~Source();

    void setLoop(bool loop) const;
    void setGain(float gain) const;
    void setPitch(float pitch) const;
    void setPosition(vec3 position) const;
    void setVelocity(vec3 velocity) const;
    bool isPlaying() const;
    void play(Sound *sound) const;
    void pause() const;
    void resume() const;
    void stop() const;
private:
    ALuint source_id;
};

#endif
