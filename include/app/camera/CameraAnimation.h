#ifndef CAMERA_ANIMATION_H
#define CAMERA_ANIMATION_H

#include <GSgfx.h>

enum CameraAnimationState {
    CAMERA_ANIMATION_ACCELERATING = 0,
    CAMERA_ANIMATION_CONSTANT_SPEED = 1,
    CAMERA_ANIMATION_DECELERATING = 2
};

/* Value animation the camera module moves its targets with. The camera keeps
 * one instance per animated quantity and drives it through start, stop,
 * update, updateAcceleration and setAcceleration.
 *
 * An animation runs either as a plain interpolation, where update moves
 * m_current from m_start to m_end over m_duration, or through a speed that
 * updateAcceleration ramps up and down again; the ramp is scaled to the video
 * frame rate for the animations that were set up for it. */
template <class T>
class CameraAnimation {
public:
    CameraAnimation();
    void start(const T* from, const T* to, f32 duration);
    void stop();
    /* Nonzero while the animation is running. */
    u8 isMove() const;
    /* Steps the animation by `delta`; returns zero while the animation is
     * stopped and nonzero once it has produced a current value. */
    u8 update(f32 delta);
    /* Steps the acceleration ramp by `delta`; returns zero while the
     * animation is stopped and nonzero once it has produced a current value. */
    u8 updateAcceleration(f32 delta);
    void setAcceleration(f32 acceleration);
    const T* getCurrentPos() const;

private:
    /* Current leg of the acceleration profile (CameraAnimationState). */
    u32 m_state;
    /* Set by start(), cleared by stop() and when an animation runs out;
     * update does nothing while it is clear. */
    u8 m_isRunning;
    /* The frame rate scaled ramp still has to be set up; the update that sets
     * it up clears it again. */
    u8 m_rampPending;
    /* Run the animation through updateAcceleration rather than the plain
     * interpolation. */
    u8 m_useAcceleration;
    /* Keeps the frame rate scaled ramp from being set up. */
    u8 m_useDirectRamp;
    /* Scale the ramp with the video frame rate, and allow the deceleration
     * leg once m_elapsed reaches m_decelerationTime. */
    u8 m_frameRateScaled;
    u8 pad[0x03];
    /* Current speed, in targets per tick. */
    f32 m_speed;
    /* Speed increment per tick while the speed ramps. */
    f32 m_acceleration;
    /* Elapsed time at which the deceleration leg starts. */
    f32 m_decelerationTime;
    /* Total duration of the animation. */
    f32 m_duration;
    /* Time the animation has run so far. */
    f32 m_elapsed;
    /* Start, end and current value the animation interpolates; m_current is
     * the one the camera reads. */
    T m_start;
    T m_end;
    T m_current;
};

#endif
