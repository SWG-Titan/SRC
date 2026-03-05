//===================================================================
//
// TangibleDynamics.cpp
// Copyright 2025, 2026 Titan SWG, All Rights Reserved
//
// Full implementation of TangibleDynamics providing push, spin,
// breathing, bounce, wobble, orbit, drag, and easing effects.
//
//===================================================================

#include "sharedObject/FirstSharedObject.h"
#include "sharedObject/TangibleDynamics.h"

#include "sharedMath/Vector.h"
#include "sharedObject/Object.h"
#include "sharedObject/AlterResult.h"
#include "sharedMath/Transform.h"

#include <cmath>

//===================================================================
// STATIC HELPERS
//===================================================================

namespace TangibleDynamicsNamespace
{
	static float const s_minimumElapsedTime = 0.001f;
	static float const s_bounceMinVelocity  = 0.05f;

	inline bool isExpired(float duration, float elapsed)
	{
		return (duration >= 0.0f && elapsed >= duration);
	}
}

using namespace TangibleDynamicsNamespace;

// Use global PI constants from FloatMath.h (included via FirstSharedObject.h)

//===================================================================
// CONSTRUCTOR / DESTRUCTOR
//===================================================================

TangibleDynamics::TangibleDynamics(Object* owner) :
	SimpleDynamics(owner),
	// Push
	m_pushVelocity(Vector::zero),
	m_pushDuration(-1.0f),
	m_pushElapsed(0.0f),
	m_pushDrag(0.0f),
	m_pushSpace(MS_world),
	m_pushForceActive(false),
	// Spin
	m_spinAngular(Vector::zero),
	m_spinDuration(-1.0f),
	m_spinElapsed(0.0f),
	m_spinForceActive(false),
	m_spinAroundAppearanceCenter(false),
	// Breathing
	m_baseScale(owner ? owner->getScale() : Vector::xyz111),
	m_breathingMin(1.0f),
	m_breathingMax(1.0f),
	m_breathingSpeed(1.0f),
	m_breathingDuration(-1.0f),
	m_breathingElapsed(0.0f),
	m_breathingPhase(0.0f),
	m_breathingEffectActive(false),
	// Bounce
	m_bounceGravity(9.8f),
	m_bounceElasticity(0.6f),
	m_bounceVerticalVelocity(0.0f),
	m_bounceFloorY(0.0f),
	m_bounceDuration(-1.0f),
	m_bounceElapsed(0.0f),
	m_bounceEffectActive(false),
	// Wobble
	m_wobbleAmplitude(Vector::zero),
	m_wobbleFrequency(Vector::zero),
	m_wobbleDuration(-1.0f),
	m_wobbleElapsed(0.0f),
	m_wobblePhase(0.0f),
	m_wobbleOrigin(Vector::zero),
	m_wobbleEffectActive(false),
	// Orbit
	m_orbitCenter(Vector::zero),
	m_orbitRadius(1.0f),
	m_orbitSpeed(1.0f),
	m_orbitAngle(0.0f),
	m_orbitDuration(-1.0f),
	m_orbitElapsed(0.0f),
	m_orbitEffectActive(false),
	// Easing
	m_easeType(ET_none),
	m_easeDuration(0.5f),
	// Overall
	m_activeForceMask(FM_none)
{
}

//-------------------------------------------------------------------

TangibleDynamics::~TangibleDynamics()
{
	Object* const owner = getOwner();
	if (owner != NULL && m_breathingEffectActive)
	{
		owner->setRecursiveScale(m_baseScale);
	}
}

//===================================================================
// EASING
//===================================================================

float TangibleDynamics::computeEaseFactor(EaseType easeType, float elapsed, float duration, float easeDuration)
{
	if (easeType == ET_none || easeDuration <= 0.0f)
		return 1.0f;

	float factor = 1.0f;

	switch (easeType)
	{
	case ET_easeIn:
		if (elapsed < easeDuration)
			factor = elapsed / easeDuration;
		break;

	case ET_easeOut:
		if (duration > 0.0f && elapsed > (duration - easeDuration))
		{
			float remaining = duration - elapsed;
			factor = (remaining > 0.0f) ? (remaining / easeDuration) : 0.0f;
		}
		break;

	case ET_easeInOut:
		if (elapsed < easeDuration)
		{
			factor = elapsed / easeDuration;
		}
		else if (duration > 0.0f && elapsed > (duration - easeDuration))
		{
			float remaining = duration - elapsed;
			factor = (remaining > 0.0f) ? (remaining / easeDuration) : 0.0f;
		}
		break;

	default:
		break;
	}

	// Smooth step (hermite interpolation)
	return factor * factor * (3.0f - 2.0f * factor);
}

//===================================================================
// PUSH / SHOVE
//===================================================================

void TangibleDynamics::setPushForce(const Vector& velocity, float duration, MovementSpace space)
{
	m_pushVelocity = velocity;
	m_pushDuration = duration;
	m_pushElapsed = 0.0f;
	m_pushDrag = 0.0f;
	m_pushSpace = space;
	m_pushForceActive = true;
	recalculateMode();
}

//-------------------------------------------------------------------

void TangibleDynamics::setPushForceWithDrag(const Vector& velocity, float drag, float duration, MovementSpace space)
{
	m_pushVelocity = velocity;
	m_pushDuration = duration;
	m_pushElapsed = 0.0f;
	m_pushDrag = drag;
	m_pushSpace = space;
	m_pushForceActive = true;
	recalculateMode();
}

//-------------------------------------------------------------------

void TangibleDynamics::clearPushForce()
{
	m_pushVelocity = Vector::zero;
	m_pushDuration = -1.0f;
	m_pushElapsed = 0.0f;
	m_pushDrag = 0.0f;
	m_pushForceActive = false;
	setCurrentVelocity_w(Vector::zero);
	recalculateMode();
}

//-------------------------------------------------------------------

Vector TangibleDynamics::getPushForce() const
{
	return m_pushVelocity;
}

//-------------------------------------------------------------------

float TangibleDynamics::getPushForceDuration() const
{
	if (m_pushDuration < 0.0f)
		return -1.0f;
	return m_pushDuration - m_pushElapsed;
}

//-------------------------------------------------------------------

void TangibleDynamics::setPushDrag(float drag)
{
	m_pushDrag = drag;
}

//-------------------------------------------------------------------

float TangibleDynamics::getPushDrag() const
{
	return m_pushDrag;
}

//===================================================================
// SPIN / ROTATION
//===================================================================

void TangibleDynamics::setSpinForce(const Vector& rotationRadiansPerSecond, float duration)
{
	m_spinAngular = rotationRadiansPerSecond;
	m_spinDuration = duration;
	m_spinElapsed = 0.0f;
	m_spinForceActive = true;
	recalculateMode();
}

//-------------------------------------------------------------------

void TangibleDynamics::clearSpinForce()
{
	m_spinAngular = Vector::zero;
	m_spinDuration = -1.0f;
	m_spinElapsed = 0.0f;
	m_spinForceActive = false;
	recalculateMode();
}

//-------------------------------------------------------------------

Vector TangibleDynamics::getSpinForce() const
{
	return m_spinAngular;
}

//-------------------------------------------------------------------

float TangibleDynamics::getSpinForceDuration() const
{
	if (m_spinDuration < 0.0f)
		return -1.0f;
	return m_spinDuration - m_spinElapsed;
}

//-------------------------------------------------------------------

bool TangibleDynamics::getSpinAroundAppearanceCenter() const
{
	return m_spinAroundAppearanceCenter;
}

//-------------------------------------------------------------------

void TangibleDynamics::setSpinAroundAppearanceCenter(bool spinAroundAppearanceCenter)
{
	m_spinAroundAppearanceCenter = spinAroundAppearanceCenter;
}

//===================================================================
// BREATHING (sinusoidal)
//===================================================================

void TangibleDynamics::setBreathingEffect(float minimumScale, float maximumScale, float speed, float duration)
{
	m_breathingMin = minimumScale;
	m_breathingMax = maximumScale;
	m_breathingSpeed = speed;
	m_breathingDuration = duration;
	m_breathingElapsed = 0.0f;
	m_breathingPhase = 0.0f;
	m_breathingEffectActive = true;

	Object* const owner = getOwner();
	if (owner != NULL)
	{
		m_baseScale = owner->getScale();
	}

	recalculateMode();
}

//-------------------------------------------------------------------

void TangibleDynamics::clearBreathingEffect()
{
	Object* const owner = getOwner();
	if (owner != NULL && m_breathingEffectActive)
	{
		owner->setRecursiveScale(m_baseScale);
	}

	m_breathingMin = 1.0f;
	m_breathingMax = 1.0f;
	m_breathingSpeed = 1.0f;
	m_breathingDuration = -1.0f;
	m_breathingElapsed = 0.0f;
	m_breathingPhase = 0.0f;
	m_breathingEffectActive = false;
	recalculateMode();
}

//-------------------------------------------------------------------

float TangibleDynamics::getBreathingMinScale() const   { return m_breathingMin; }
float TangibleDynamics::getBreathingMaxScale() const   { return m_breathingMax; }
float TangibleDynamics::getBreathingSpeed() const      { return m_breathingSpeed; }

float TangibleDynamics::getBreathingDuration() const
{
	if (m_breathingDuration < 0.0f)
		return -1.0f;
	return m_breathingDuration - m_breathingElapsed;
}

//===================================================================
// BOUNCE (gravity + elasticity)
//===================================================================

void TangibleDynamics::setBounceEffect(float gravity, float elasticity, float initialUpVelocity, float duration)
{
	Object* const owner = getOwner();

	m_bounceGravity = gravity;
	m_bounceElasticity = clamp(0.0f, elasticity, 1.0f);
	m_bounceVerticalVelocity = initialUpVelocity;
	m_bounceFloorY = owner ? owner->getPosition_w().y : 0.0f;
	m_bounceDuration = duration;
	m_bounceElapsed = 0.0f;
	m_bounceEffectActive = true;
	recalculateMode();
}

//-------------------------------------------------------------------

void TangibleDynamics::clearBounceEffect()
{
	m_bounceGravity = 9.8f;
	m_bounceElasticity = 0.6f;
	m_bounceVerticalVelocity = 0.0f;
	m_bounceDuration = -1.0f;
	m_bounceElapsed = 0.0f;
	m_bounceEffectActive = false;
	recalculateMode();
}

//-------------------------------------------------------------------

float TangibleDynamics::getBounceGravity() const     { return m_bounceGravity; }
float TangibleDynamics::getBounceElasticity() const  { return m_bounceElasticity; }

//===================================================================
// WOBBLE (sinusoidal position oscillation)
//===================================================================

void TangibleDynamics::setWobbleEffect(const Vector& amplitude, const Vector& frequency, float duration)
{
	Object* const owner = getOwner();

	m_wobbleAmplitude = amplitude;
	m_wobbleFrequency = frequency;
	m_wobbleDuration = duration;
	m_wobbleElapsed = 0.0f;
	m_wobblePhase = 0.0f;
	m_wobbleOrigin = owner ? owner->getPosition_w() : Vector::zero;
	m_wobbleEffectActive = true;
	recalculateMode();
}

//-------------------------------------------------------------------

void TangibleDynamics::clearWobbleEffect()
{
	Object* const owner = getOwner();
	if (owner != NULL && m_wobbleEffectActive)
	{
		owner->setPosition_w(m_wobbleOrigin);
	}

	m_wobbleAmplitude = Vector::zero;
	m_wobbleFrequency = Vector::zero;
	m_wobbleDuration = -1.0f;
	m_wobbleElapsed = 0.0f;
	m_wobblePhase = 0.0f;
	m_wobbleEffectActive = false;
	recalculateMode();
}

//-------------------------------------------------------------------

Vector TangibleDynamics::getWobbleAmplitude() const   { return m_wobbleAmplitude; }
Vector TangibleDynamics::getWobbleFrequency() const   { return m_wobbleFrequency; }

//===================================================================
// ORBIT (circular motion around point)
//===================================================================

void TangibleDynamics::setOrbitEffect(const Vector& center, float radius, float radiansPerSecond, float duration)
{
	Object* const owner = getOwner();

	m_orbitCenter = center;
	m_orbitRadius = radius;
	m_orbitSpeed = radiansPerSecond;
	m_orbitDuration = duration;
	m_orbitElapsed = 0.0f;
	m_orbitEffectActive = true;

	// Compute initial angle from current position
	if (owner != NULL)
	{
		Vector delta = owner->getPosition_w() - center;
		m_orbitAngle = atan2(delta.x, delta.z);
	}
	else
	{
		m_orbitAngle = 0.0f;
	}

	recalculateMode();
}

//-------------------------------------------------------------------

void TangibleDynamics::clearOrbitEffect()
{
	m_orbitCenter = Vector::zero;
	m_orbitRadius = 1.0f;
	m_orbitSpeed = 1.0f;
	m_orbitAngle = 0.0f;
	m_orbitDuration = -1.0f;
	m_orbitElapsed = 0.0f;
	m_orbitEffectActive = false;
	recalculateMode();
}

//-------------------------------------------------------------------

Vector TangibleDynamics::getOrbitCenter() const  { return m_orbitCenter; }
float  TangibleDynamics::getOrbitRadius() const  { return m_orbitRadius; }

//===================================================================
// EASING
//===================================================================

void TangibleDynamics::setEasing(EaseType easeType, float easeDuration)
{
	m_easeType = easeType;
	m_easeDuration = easeDuration;
}

//===================================================================
// COMBINED
//===================================================================

void TangibleDynamics::setCombinedForces(const Vector& pushVelocity, const Vector& spinAngular,
	float minScale, float maxScale, float breatheSpeed, float duration)
{
	setPushForce(pushVelocity, duration, MS_world);
	setSpinForce(spinAngular, duration);
	setBreathingEffect(minScale, maxScale, breatheSpeed, duration);
}

//-------------------------------------------------------------------

void TangibleDynamics::clearAllForces()
{
	clearPushForce();
	clearSpinForce();
	clearBreathingEffect();
	clearBounceEffect();
	clearWobbleEffect();
	clearOrbitEffect();
}

//===================================================================
// QUERY
//===================================================================

int TangibleDynamics::getActiveForceMask() const
{
	return m_activeForceMask;
}

//-------------------------------------------------------------------

bool TangibleDynamics::isActive() const
{
	return m_activeForceMask != FM_none;
}

//-------------------------------------------------------------------

bool TangibleDynamics::isForceActive(ForceMode mode) const
{
	return (m_activeForceMask & static_cast<int>(mode)) != 0;
}

//===================================================================
// ALTER
//===================================================================

float TangibleDynamics::alter(float elapsedTime)
{
	float const baseAlterResult = SimpleDynamics::alter(elapsedTime);

	if (m_activeForceMask != FM_none && elapsedTime > s_minimumElapsedTime)
	{
		realAlter(elapsedTime);
		return AlterResult::cms_alterNextFrame;
	}

	if (m_activeForceMask != FM_none)
		return AlterResult::cms_alterNextFrame;

	return baseAlterResult;
}

//-------------------------------------------------------------------

void TangibleDynamics::realAlter(float elapsedTime)
{
	if (m_pushForceActive)
		updatePushForce(elapsedTime);

	if (m_spinForceActive)
		updateSpinForce(elapsedTime);

	if (m_breathingEffectActive)
		updateBreathingEffect(elapsedTime);

	if (m_bounceEffectActive)
		updateBounceEffect(elapsedTime);

	if (m_wobbleEffectActive)
		updateWobbleEffect(elapsedTime);

	if (m_orbitEffectActive)
		updateOrbitEffect(elapsedTime);
}

//===================================================================
// UPDATE HELPERS
//===================================================================

void TangibleDynamics::updatePushForce(float elapsedTime)
{
	Object* const owner = getOwner();
	if (owner == NULL)
	{
		clearPushForce();
		return;
	}

	// Duration check
	if (m_pushDuration >= 0.0f)
	{
		m_pushElapsed += elapsedTime;
		if (m_pushElapsed >= m_pushDuration)
		{
			clearPushForce();
			return;
		}
	}

	// Easing
	float const ease = computeEaseFactor(m_easeType, m_pushElapsed, m_pushDuration, m_easeDuration);

	// Apply drag (exponential decay)
	if (m_pushDrag > 0.0f)
	{
		float const dragFactor = exp(-m_pushDrag * elapsedTime);
		m_pushVelocity *= dragFactor;

		// Stop if velocity is negligible
		if (m_pushVelocity.magnitudeSquared() < 0.001f)
		{
			clearPushForce();
			return;
		}
	}

	// Apply velocity based on space
	Vector const effectiveVelocity = m_pushVelocity * ease;

	switch (m_pushSpace)
	{
	case MS_world:
		setCurrentVelocity_w(effectiveVelocity);
		break;
	case MS_parent:
		setCurrentVelocity_p(effectiveVelocity);
		break;
	case MS_object:
		setCurrentVelocity_o(effectiveVelocity);
		break;
	}
}

//-------------------------------------------------------------------

void TangibleDynamics::updateSpinForce(float elapsedTime)
{
	// Server-side: only track timing for duration expiration
	// Actual rotation is handled client-side for smooth visuals

	if (m_spinDuration >= 0.0f)
	{
		m_spinElapsed += elapsedTime;
		if (m_spinElapsed >= m_spinDuration)
		{
			clearSpinForce();
			return;
		}
	}

	// Note: Transform rotation is intentionally NOT applied server-side
	// to prevent choppy updates. Client receives spin params via
	// CM_tangibleDynamicsData and renders smoothly at frame rate.
}

//-------------------------------------------------------------------

void TangibleDynamics::updateBreathingEffect(float elapsedTime)
{
	// Server-side: only track timing for duration expiration
	// Actual scale animation is handled client-side for smooth visuals

	if (m_breathingDuration >= 0.0f)
	{
		m_breathingElapsed += elapsedTime;
		if (m_breathingElapsed >= m_breathingDuration)
		{
			clearBreathingEffect();
			return;
		}
	}

	// Update phase for sync tracking (but don't apply scale here)
	m_breathingPhase += elapsedTime * m_breathingSpeed;

	// Note: Scale changes are intentionally NOT applied server-side
	// to prevent choppy updates. Client receives breathing params via
	// CM_tangibleDynamicsData and renders smoothly at frame rate.
}

//-------------------------------------------------------------------

void TangibleDynamics::updateBounceEffect(float elapsedTime)
{
	// Server-side: only track timing for duration expiration
	// Actual bounce physics is handled client-side for smooth visuals

	if (m_bounceDuration >= 0.0f)
	{
		m_bounceElapsed += elapsedTime;
		if (m_bounceElapsed >= m_bounceDuration)
		{
			clearBounceEffect();
			return;
		}
	}

	// Track velocity decay for auto-clear detection
	m_bounceVerticalVelocity -= m_bounceGravity * elapsedTime;

	// Simulate floor collision for timing (no actual position change)
	if (m_bounceVerticalVelocity < -m_bounceGravity)
	{
		m_bounceVerticalVelocity = -m_bounceVerticalVelocity * m_bounceElasticity;
		if (fabs(m_bounceVerticalVelocity) < s_bounceMinVelocity)
		{
			clearBounceEffect();
			return;
		}
	}

	// Note: Position changes are intentionally NOT applied server-side
	// to prevent choppy updates. Client receives bounce params via
	// CM_tangibleDynamicsData and renders smoothly at frame rate.
}

//-------------------------------------------------------------------

void TangibleDynamics::updateWobbleEffect(float elapsedTime)
{
	// Server-side: only track timing for duration expiration
	// Actual position wobble is handled client-side for smooth visuals

	if (m_wobbleDuration >= 0.0f)
	{
		m_wobbleElapsed += elapsedTime;
		if (m_wobbleElapsed >= m_wobbleDuration)
		{
			clearWobbleEffect();
			return;
		}
	}

	// Update phase for sync tracking
	m_wobblePhase += elapsedTime;

	// Note: Position changes are intentionally NOT applied server-side
	// to prevent choppy updates. Client receives wobble params via
	// CM_tangibleDynamicsData and renders smoothly at frame rate.
}

//-------------------------------------------------------------------

void TangibleDynamics::updateOrbitEffect(float elapsedTime)
{
	// Server-side: only track timing for duration expiration
	// Actual orbit position is handled client-side for smooth visuals

	if (m_orbitDuration >= 0.0f)
	{
		m_orbitElapsed += elapsedTime;
		if (m_orbitElapsed >= m_orbitDuration)
		{
			clearOrbitEffect();
			return;
		}
	}

	// Update angle for sync tracking
	float const ease = computeEaseFactor(m_easeType, m_orbitElapsed, m_orbitDuration, m_easeDuration);
	m_orbitAngle += m_orbitSpeed * elapsedTime * ease;

	// Note: Position changes are intentionally NOT applied server-side
	// to prevent choppy updates. Client receives orbit params via
	// CM_tangibleDynamicsData and renders smoothly at frame rate.
}

//===================================================================
// RECALCULATE
//===================================================================

void TangibleDynamics::recalculateMode()
{
	m_activeForceMask = FM_none;

	if (m_pushForceActive)       m_activeForceMask |= FM_push;
	if (m_spinForceActive)       m_activeForceMask |= FM_spin;
	if (m_breathingEffectActive) m_activeForceMask |= FM_breathing;
	if (m_bounceEffectActive)    m_activeForceMask |= FM_bounce;
	if (m_wobbleEffectActive)    m_activeForceMask |= FM_wobble;
	if (m_orbitEffectActive)     m_activeForceMask |= FM_orbit;
}

//===================================================================
