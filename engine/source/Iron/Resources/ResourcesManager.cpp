#include "ResourcesManager.h"
#include "AsepriteLoader.h"
#include "PngLoader.h"
#include "../Core/Log.h"
#include "../Audio/AudioCore.h"
#include "../Audio/WavLoader.h"
#include "../Rendering/BuiltInShaders.h"
#include "../Rendering/OpenGLAPI.h"
#include "../UI/FontManager.h"

#include <fstream>
// TODO: remove al dependency into AudioCore
#include <AL/al.h>

#ifdef DISTRIBUTE_BUILD
#define ENGINE_RESOURCES_PATH "Resources/"
#define RESOURCES_PATH "Resources/"
#else
#define ENGINE_RESOURCES_PATH "../../../engine/resources/"
#define RESOURCES_PATH "../../resources/"
#endif

ResourcesManager::ResourcesManager()
{
    FontManager::Init();
}

ResourcesManager::~ResourcesManager()
{
    for (auto image : images)
    {
        OpenGLAPI::DeleteTexture(image.second->TextureID);
        delete image.second;
    }

    for (auto audioTrack : audioTracks)
    {
        alDeleteBuffers(1, &audioTrack.second->BufferID);
        delete audioTrack.second;
    }

    for (auto animation : animations)
    {
        delete animation.second;
    }

    for (auto font : fonts)
    {
        delete font.second;
    }

    for (auto data : asepriteDatas)
    {
        delete data.second;
    }

    for (auto shader : shaders)
    {
        delete shader.second;
    }

    for (auto material : materials)
    {
        delete material.second;
    }
}

void ResourcesManager::LoadDefaultResources()
{
    defaultFont = LoadFont("font.ttf", true);
    if (defaultFont != nullptr)
        defaultFont->AddSizeIfNotExists(32);

    defaultSpriteShader = new Shader(BuiltInShaders::DefaultSpriteVS, BuiltInShaders::DefaultSpriteFS);
    AddShader(defaultSpriteShader);

    defaultUIShader = new Shader(BuiltInShaders::DefaultUIVS, BuiltInShaders::DefaultUIFS);
    AddShader(defaultUIShader);

    defaultSpriteMaterial = new Material();
    defaultSpriteMaterial->MainShader = defaultSpriteShader;
    AddMaterial(defaultSpriteMaterial);

    defaultUIMaterial = new Material();
    defaultUIMaterial->MainShader = defaultUIShader;
    AddMaterial(defaultUIMaterial);
}

const char* ResourcesManager::GetResourcesPath()
{
    return RESOURCES_PATH;
}

Sprite* ResourcesManager::LoadImage(const char* filePath, bool engineResource)
{
    std::string fullPathString = engineResource ? ENGINE_RESOURCES_PATH : RESOURCES_PATH;
    fullPathString += filePath;
    std::string fullPath = fullPathString;

    std::ifstream infile(fullPath);
    if (!infile.good())
    {
        Log::LogError("Error loading image: file {0} does not exist", fullPath);
        return nullptr;
    }

    Sprite* image;
    std::string extension = fullPath.substr(fullPath.find_last_of('.') + 1);
    if (extension == "png")
    {
        image = PngLoader::LoadImage(fullPath.c_str());
        if (image == nullptr)
            return nullptr;

        AddImage(image);
    }
    else if (extension == "aseprite")
    {
        AsepriteData data;
        if (!AsepriteLoader::LoadAsepriteData(fullPath.c_str(), false, data))
            return nullptr;

        if (data.Sprites.empty())
        {
            Log::LogError("Error loading image: no data in aseprite file");
            return nullptr;
        }
        if (data.Sprites.size() > 1)
            Log::LogWarning("Aseprite file contains multiple images but only first will be used");

        image = data.Sprites[0];
    }
    else
    {
        Log::LogError("Error loading image: .{0} files not supported", extension);
        return nullptr;
    }

    Log::LogDebug("Sprite loaded: {0}, {1}", fullPath, image->ID);

    return image;
}

void ResourcesManager::AddImage(Sprite* image)
{
    image->ID = GetNextResourceID();
    images[image->ID] = image;
}

Sprite* ResourcesManager::GetImage(ResourceID imageID)
{
    if (imageID == NULL_RESOURCE)
        return nullptr;

    if (images.find(imageID) == images.end())
    {
        Log::LogError("Image {0} does not exist", imageID);
        return nullptr;
    }

    return images[imageID];
}

void ResourcesManager::UnloadImage(ResourceID imageID)
{
    auto sprite = GetImage(imageID);
    if (sprite == nullptr)
        return;

    OpenGLAPI::DeleteTexture(sprite->TextureID);
    images.erase(imageID);
    FreeResourceID(imageID);
    delete sprite;
}

AsepriteData* ResourcesManager::LoadAsepriteData(const char* filePath, bool loopAll)
{
    std::string fullPathString = RESOURCES_PATH;
    fullPathString += filePath;
    std::string fullPath = fullPathString;
    auto data = new AsepriteData();

    std::ifstream infile(fullPath);
    if (!infile.good())
    {
        Log::LogError("Error loading aseprite file: file {0} does not exist", fullPath);
        return nullptr;
    }

    std::string extension = fullPath.substr(fullPath.find_last_of('.') + 1);
    if (extension != "aseprite")
    {
        Log::LogError("Error loading aseprite file: .{0} files not supported", extension);
        return nullptr;
    }

    if (!AsepriteLoader::LoadAsepriteData(fullPath.c_str(), loopAll, *data))
    {
        delete data;
        return nullptr;
    }

    data->ID = GetNextResourceID();
    asepriteDatas[data->ID] = data;

    Log::LogDebug("Aseprite file loaded: {0}, {1}", fullPath, data->ID);

    return data;
}

AsepriteData* ResourcesManager::GetAsepriteData(ResourceID resourceID)
{
    if (resourceID == NULL_RESOURCE)
        return nullptr;

    if (asepriteDatas.find(resourceID) == asepriteDatas.end())
    {
        Log::LogError("Aseprite data {0} does not exist", resourceID);
        return nullptr;
    }

    return asepriteDatas[resourceID];
}

static inline ALenum ToALFormat(int channels, int samples)
{
    bool stereo = (channels > 1);

    switch (samples) {
        case 16:
            if (stereo)
                return AL_FORMAT_STEREO16;
            else
                return AL_FORMAT_MONO16;
        case 8:
            if (stereo)
                return AL_FORMAT_STEREO8;
            else
                return AL_FORMAT_MONO8;
        default:
            return -1;
    }
}

AudioTrack* ResourcesManager::LoadAudioTrack(const char* filePath)
{
    std::string fullPathString = RESOURCES_PATH;
    fullPathString += filePath;
    const char* fullPath = fullPathString.c_str();

    std::ifstream infile(fullPath);
    if (!infile.good())
    {
        Log::LogError("Error loading audio track: file does not exist");
        return nullptr;
    }

    if (!AudioCore::Initialized())
    {
        Log::LogError("Can't load audio: audio system is not initialized");
        return nullptr;
    }

    ALuint audioBuffer;
    char* data;
    auto audioTrack = WavLoader::LoadWav(fullPath, &data);
    if (audioTrack == nullptr)
        return nullptr;

    alGenBuffers((ALuint)1, &audioBuffer);
    if (AudioCore::CheckForErrors())
    {
        Log::LogError("Error generating audio buffer");
        return nullptr;
    }
    alBufferData(audioBuffer, ToALFormat(audioTrack->NumberOfChannels, audioTrack->BitsPerSample),
                 data, (ALsizei)audioTrack->NumberOfSamples, audioTrack->SampleRate);
    delete data;
    if (AudioCore::CheckForErrors())
    {
        Log::LogError("Error loading audio data to buffer");
        delete audioTrack;
        return nullptr;
    }

    audioTrack->ID = GetNextResourceID();
    audioTrack->BufferID = audioBuffer;
    audioTracks[audioTrack->BufferID] = audioTrack;

    Log::LogDebug("Audio track loaded: {0}, {1}", fullPath, audioTrack->ID);

    return audioTrack;
}

AudioTrack* ResourcesManager::GetAudioTrack(ResourceID audioID)
{
    if (audioID == NULL_RESOURCE)
        return nullptr;

    if (audioTracks.find(audioID) == audioTracks.end())
    {
        Log::LogError("Audio track {0} does not exist", audioID);
        return nullptr;
    }

    return audioTracks[audioID];
}

void ResourcesManager::UnloadAudioTrack(ResourceID audioID)
{
    auto audioTrack = GetAudioTrack(audioID);
    if (audioTrack == nullptr)
        return;

    alDeleteBuffers(1, &audioTrack->BufferID);
    audioTracks.erase(audioID);
    FreeResourceID(audioID);
    delete audioTrack;
}

void ResourcesManager::AddAnimation(Animation* animation)
{
    animation->ID = GetNextResourceID();
    if (animation->Name.empty())
    {
        animation->Name = std::to_string(animation->ID);
    }
    animations[animation->ID] = animation;
}

Animation* ResourcesManager::GetAnimation(ResourceID animationID)
{
    if (animationID == NULL_RESOURCE)
        return nullptr;

    if (animations.find(animationID) == animations.end())
    {
        Log::LogError("Animation {0} does not exist", animationID);
        return nullptr;
    }

    return animations[animationID];
}

void ResourcesManager::RemoveAnimation(ResourceID animationID)
{
    auto animation = GetAnimation(animationID);
    if (animation == nullptr)
        return;

    animations.erase(animationID);
    FreeResourceID(animationID);
    delete animation;
}

Font* ResourcesManager::LoadFont(const char* fontPath, bool engineResource)
{
    std::string fullPathString = engineResource ? ENGINE_RESOURCES_PATH : RESOURCES_PATH;
    fullPathString += fontPath;
    const char* fullPath = fullPathString.c_str();

    std::ifstream infile(fullPath);
    if (!infile.good())
    {
        Log::LogError("Error loading font: file does not exist");
        return nullptr;
    }

    if (!FontManager::IsInitialized())
    {
        Log::LogError("Font manager is not initialized");
        return nullptr;
    }

    auto font = FontManager::FontFromPath(fullPath);

    if (font == nullptr)
        return nullptr;

    font->ID = GetNextResourceID();
    fonts[font->ID] = font;

    Log::LogDebug("Font loaded: {0}, {1}", fullPath, font->ID);

    return font;
}

Font* ResourcesManager::GetFont(ResourceID fontID)
{
    if (fontID == NULL_RESOURCE)
        return nullptr;

    if (fonts.find(fontID) == fonts.end())
    {
        Log::LogError("Font {0} does not exist", fontID);
        return nullptr;
    }

    return fonts[fontID];
}

Font* ResourcesManager::DefaultFont()
{
    return defaultFont;
}

void ResourcesManager::AddShader(Shader* shader)
{
    shader->ID = GetNextResourceID();
    shaders[shader->ID] = shader;
}

Shader* ResourcesManager::GetShader(ResourceID shaderID)
{
    if (shaderID == NULL_RESOURCE)
        return nullptr;

    if (shaders.find(shaderID) == shaders.end())
    {
        Log::LogError("Shader {0} does not exist", shaderID);
        return nullptr;
    }

    return shaders[shaderID];
}

Shader* ResourcesManager::DefaultSpriteShader()
{
    return defaultSpriteShader;
}

Shader* ResourcesManager::DefaultUIShader()
{
    return defaultUIShader;
}

void ResourcesManager::AddMaterial(Material* material)
{
    material->ID = GetNextResourceID();
    materials[material->ID] = material;
}

Material* ResourcesManager::GetMaterial(ResourceID materialID)
{
    if (materialID == NULL_RESOURCE)
        return nullptr;

    if (materials.find(materialID) == materials.end())
    {
        Log::LogError("Material {0} does not exist", materialID);
        return nullptr;
    }

    return materials[materialID];
}

Material* ResourcesManager::DefaultSpriteMaterial()
{
    return defaultSpriteMaterial;
}

Material* ResourcesManager::DefaultUIMaterial()
{
    return defaultUIMaterial;
}

ResourceID ResourcesManager::GetNextResourceID()
{
    if (nextResourceID == (uint32_t)-1)
    {
        Log::LogError("Resource ID overflow");
    }
    return nextResourceID++;
}

void ResourcesManager::FreeResourceID(ResourceID resourceID)
{
    // TODO: recycle resource IDs
}
