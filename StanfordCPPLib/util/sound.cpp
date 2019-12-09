/*
 * File: sound.cpp
 * ---------------
 * Implementation of the Sound class.
 * 
 * @version 2015/07/05
 * - removed static global Platform variable, replaced by getPlatform as needed
 * @version 2014/10/08
 * - removed 'using namespace' statement
 * - removed unused include statements
 */

#include "sound.h"
#include <QUrl>
#include <utility>
#include <io/filelib.h>
#include <graphics/gthread.h>
#include <util/require.h>

/*static*/ std::unique_ptr<QMediaPlayer> Sound::_qmediaPlayer = nullptr;

/*static*/ long Sound::getDuration() {
    initialize();
    return _qmediaPlayer->duration();
}

/*static*/ int Sound::getVolume() {
    initialize();
    return _qmediaPlayer->volume();
}

/*static*/ void Sound::initialize() {
    if (!_qmediaPlayer) {
        GThread::runOnQtGuiThread([]() {
            if (!_qmediaPlayer) {
                _qmediaPlayer = std::make_unique<QMediaPlayer>();
            }
        });
    }
}

/*static*/ void Sound::pause() {
    initialize();
    _qmediaPlayer->pause();
}

/*static*/ void Sound::playSound(const std::string &filename) {
    initialize();
    std::string absPath = getAbsolutePath(filename);
    if (!fileExists(absPath)) {
        error("Sound::playSound: file not found: " + filename);
    }

    GThread::runOnQtGuiThreadAsync([absPath]() {
        _qmediaPlayer->setMedia(QUrl::fromLocalFile(QString::fromStdString(absPath)));
        _qmediaPlayer->play();
    });
}

/*static*/ void Sound::stop() {
    initialize();
    _qmediaPlayer->stop();
}

/*static*/ void Sound::setVolume(int volume) {
    initialize();
    require::inRange(volume, 0, 100, "Sound::setVolume", "volume");
    _qmediaPlayer->setVolume(volume);
}

Sound::Sound(std::string filename)
        : _filename(std::move(filename)) {
    initialize();
}

Sound::~Sound() = default;

void Sound::play() {
    playSound(_filename);
}
