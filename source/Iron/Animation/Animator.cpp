#include "Animator.h"
#include "../Core/Time.h"
#include "../Rendering/SpriteRenderer.h"

void Animator::Init()
{
    currentCurveFrame.clear();
    for (int i = 0; i < Animations[currentAnimation]->Curves.size(); ++i)
    {
        currentCurveFrame.push_back(0);
    }
    initialized = true;
}

void Animator::Play()
{
    if (!initialized)
        Init();

    IsPlaying = true;
}

void Animator::Play(int animation)
{
    if (animation == currentAnimation)
        Play();

    currentAnimation = animation;
    Restart();
}

void Animator::Pause()
{
    IsPlaying = false;
}

void Animator::Stop()
{
    initialized = false;
    NormalizedTime = 0.0f;
    IsPlaying = false;
}

void Animator::Restart()
{
    Stop();
    Play();
}

void ApplyFrame(Object* object, Keyframe keyframe)
{
    // TODO: cache component
    auto sr = object->GetComponent<SpriteRenderer>();
    if (sr->GetImage()->ID != keyframe.SpriteID)
        sr->SetImage(Application::Instance->GetResourcesManager()->GetImage(keyframe.SpriteID));
    sr->CurrentImageTileIndex = keyframe.TileIndex;
}

void Animator::OnUpdate()
{
    if (!IsPlaying)
        return;

    float animationTime = NormalizedTime * Animations[currentAnimation]->Length();
    animationTime += Time::DeltaTime() * Speed;
    NormalizedTime = animationTime / Animations[currentAnimation]->Length();

    for (int i = 0; i < currentCurveFrame.size(); ++i)
    {
        if (Speed > 0)
        {
            if (currentCurveFrame[i] < Animations[currentAnimation]->Curves[i].Keyframes.size() - 1)
            {
                if (animationTime >= Animations[currentAnimation]->Curves[i].Keyframes[currentCurveFrame[i] + 1].Time)
                {
                    currentCurveFrame[i]++;
                    ApplyFrame(ParentObject, Animations[currentAnimation]->Curves[i].Keyframes[currentCurveFrame[i]]);
                }
            }
        }
        else
        {
            if (currentCurveFrame[i] > 0)
            {
                if (animationTime <= Animations[currentAnimation]->Curves[i].Keyframes[currentCurveFrame[i] - 1].Time)
                {
                    currentCurveFrame[i]--;
                    ApplyFrame(ParentObject, Animations[currentAnimation]->Curves[i].Keyframes[currentCurveFrame[i]]);
                }
            }
        }
    }

    if (NormalizedTime < 0.0f && Speed < 0 || NormalizedTime > 1.0f && Speed > 0)
    {
        if (Animations[currentAnimation]->Loop)
        {
            // TODO: make normalized time show number of cycles for loop animations
            NormalizedTime = Speed < 0 ? NormalizedTime + 1.0f : NormalizedTime - 1.0f;
            for (int i = 0; i < Animations[currentAnimation]->Curves.size(); ++i)
            {
                currentCurveFrame[i] = Speed < 0 ? Animations[currentAnimation]->Curves[i].Keyframes.size() : 0;
                ApplyFrame(ParentObject, Animations[currentAnimation]->Curves[i].Keyframes[0]);
            }
        }
        else
        {
            Stop();
        }
    }
}