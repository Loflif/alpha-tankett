// alpha_openal.h

#ifndef ALPHA_OPENAL_H_INCLUDED
#define ALPHA_OPENAL_H_INCLUDED

// note: a super limited openal implementation!

// note: alc.h
typedef int ALCint;
typedef char ALCboolean;
typedef char ALCchar;
   
struct ALCdevice;
struct ALCcontext;

ALCdevice  *alcOpenDevice(const ALCchar *devicename);
ALCboolean  alcCloseDevice(ALCdevice *device);

ALCcontext *alcCreateContext(ALCdevice *device, const ALCint *attrlist);
ALCboolean  alcMakeContextCurrent(ALCcontext *context);
void        alcDestroyContext(ALCcontext *context);

// note: al.h
typedef unsigned int ALuint;
typedef int ALint;
typedef int ALsizei;
typedef int ALenum;
typedef float ALfloat;
typedef void ALvoid;

constexpr int AL_NONE                                  = 0;
constexpr int AL_FALSE                                 = 0;
constexpr int AL_TRUE                                  = 1;

constexpr int AL_BUFFER                                = 0x1009;
constexpr int AL_GAIN                                  = 0x100A;

constexpr int AL_SOURCE_STATE                          = 0x1010;
constexpr int AL_INITIAL                               = 0x1011;
constexpr int AL_PLAYING                               = 0x1012;
constexpr int AL_PAUSED                                = 0x1013;
constexpr int AL_STOPPED                               = 0x1014;

constexpr int AL_FORMAT_MONO8                          = 0x1100;
constexpr int AL_FORMAT_MONO16                         = 0x1101;
constexpr int AL_FORMAT_STEREO8                        = 0x1102;
constexpr int AL_FORMAT_STEREO16                       = 0x1103;

constexpr int AL_NO_ERROR                              = 0;
constexpr int AL_INVALID_NAME                          = 0xA001;
constexpr int AL_INVALID_ENUM                          = 0xA002;
constexpr int AL_INVALID_VALUE                         = 0xA003;
constexpr int AL_INVALID_OPERATION                     = 0xA004;
constexpr int AL_OUT_OF_MEMORY                         = 0xA005;

constexpr int AL_VENDOR                                = 0xB001;
constexpr int AL_VERSION                               = 0xB002;
constexpr int AL_RENDERER                              = 0xB003;
constexpr int AL_EXTENSIONS                            = 0xB004;

ALenum alGetError(void);

void   alGenSources(ALsizei n, ALuint *sources);
void   alDeleteSources(ALsizei n, const ALuint *sources);

void   alSourcef(ALuint source, ALenum param, ALfloat value);
void   alSourcei(ALuint source, ALenum param, ALint value);
void   alGetSourcei(ALuint source, ALenum param, ALint *value);

void   alSourcePlay(ALuint source);
void   alSourceStop(ALuint source);
void   alSourceRewind(ALuint source);
void   alSourcePause(ALuint source);

void   alGenBuffers(ALsizei n, ALuint *buffers);
void   alDeleteBuffers(ALsizei n, const ALuint *buffers);

void   alBufferData(ALuint buffer, ALenum format, const ALvoid *data, ALsizei size, ALsizei freq);

#endif // !ALPHA_OPENAL_H_INCLUDED
