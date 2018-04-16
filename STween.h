// Simple Tween
// A simple, template, cross-platform, header-only tweening library
/*
MIT License

Copyright (c) 2018 Rivero Sato

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef _S_TWEEN_H_
#define _S_TWEEN_H_
#pragma once

//These can easily be replaced with custom libraries instead of STL
#include <vector> //std::vector
#include <algorithm> //std::find
#include <functional> //std::function

namespace STween
{

// Available Easing functions built-in
// *Custom easing function not supported yet
// Function format for custom easing:
// template<typename T> 
// T YourEasingFunction(float position, T start, T end)
enum EasingFunction
{
	Linear,
	QuadranticIn,
	QuadranticOut,
	QuadranticInOut,
	CubicIn,
	CubicOut,
	CubicInOut,
	QuintIn,
	QuintOut,
	QuintInOut,
	BackIn,
	BackOut,
	BackInOut
};

// Stores tweening data
// *Mostly used internally
// Can be created individually to later be added into STween if needed
template <class T>
struct TweenData
{
public:
	TweenData(int tID)
		:tweenID(tID),
		reversed(false),
		easing(Linear)
	{}

	inline bool operator==(const TweenData& td)
	{
		return this->tweenID == td.tweenID && this->initialValue == td.initialValue && this->finalValue == td.finalValue
			&& this->timeCounter == td.timeCounter && this->reversed == td.reversed && this->easing == td.easing;
	}

	int tweenID;
	bool fromReady;
	bool byReference;
	bool reversed;
	T* initialValue;
	T initialCpy;
	T finalValue;
	float duration;
	EasingFunction easing;
	float timeCounter;
	std::function<void()> finishCallback;
	std::function<void(T&)> stepCallback;
	std::vector<TweenData<T>> endTween;
};

// Main class
// Creates and processes TweenData
// Can be used as many times for a specific type 'T'
// *Must-to-call functions to create a tween: From, To, Time
template <class T>
class STween
{
public:
	STween();
	~STween();
	
	// Creates a Tween starting from the initial value given
	// Also sets the value each frame to the variable it points to
	// *Must-to-call
	STween& From(T* initVal);
	// Sets the desired final value
	// *Must-to-call
	STween& To(T finalVal);
	// Sets the duration of the tween
	// *Must-to-call
	STween& Time(float sec);
	// Sets a callback once the tween is finished
	// *Optional
	STween& OnFinish(std::function<void()> endCallback);
	// Sets a callback for each frame the value is changing
	// Use this method as a setter if From() argument is const
	// *Optional
	STween& OnStep(std::function<void(T&)> callabck);
	// Chains another tween after this one ends
	// *Optional
	STween& Chain(STween<T>* chain);
	// Reverses the current tween
	// Values will be tweened from the final value to the initial value if set to true
	// Useful when creating a 'Loop' tween
	// *Optional
	STween& Reversed(bool isReversed);
	// Sets the easing function
	// Linear is set by default
	// *Optional
	STween& Easing(EasingFunction easingType);
	// Returns all the tweens registered
	// Helper function in case it is needed
	// Normally used with AddTweens()
	// a.AddTweens(b.GetTweens());
	std::vector<TweenData<T>> GetTweens();
	// Resets the STween object
	void ReleaseTweens();
	// Adds all the tweens from the container
	// Helper function in case it is needed
	// Normally used with GetTweens()
	// a.AddTweens(b.GetTweens());
	void AddTweens(std::vector<TweenData<T>> tweens);
	// Processes every running tween
	// deltaTime used for frame-rate independent tweening
	void Update(float deltaTime);
	// Adds a single TweenData
	// Helper function in case it is needed
	void AddTween(TweenData<T> STween);
private:
	T Linear(float position, T start, T end);
	T QuadIn(float position, T start, T end);
	T QuadOut(float position, T start, T end);
	T QuadInOut(float position, T start, T end);
	T CubicIn(float position, T start, T end);
	T CubicOut(float position, T start, T end);
	T CubicInOut(float position, T start, T end);
	T QuintIn(float position, T start, T end);
	T QuintOut(float position, T start, T end);
	T QuintInOut(float position, T start, T end);
	T BackIn(float position, T start, T end);
	T BackOut(float position, T start, T end);
	T BackInOut(float position, T start, T end);

private:
	int m_lastTweenIndex;
	std::vector<TweenData<T>> m_tweenVec;
};

template<class T>STween<T>::STween()
	:m_lastTweenIndex(-1)
{}

template<class T>STween<T>::~STween()
{}

template<class T> void STween<T>::ReleaseTweens()
{
	m_tweenVec.clear();
	m_lastTweenIndex = -1;
}

template<class T>STween<T>& STween<T>::From(T* initVal)
{
	TweenData<T> STween(m_lastTweenIndex);

	STween.fromReady = true;
	STween.byReference = true;
	STween.initialValue = initVal;
	STween.initialCpy = *initVal;
	STween.duration = 0;
	STween.timeCounter = 0;
	STween.endTween = std::vector<TweenData<T>>();

	m_tweenVec.push_back(STween);

	m_lastTweenIndex++;

	return *this;
}

template<class T>STween<T>& STween<T>::To(T finalVal)
{
	m_tweenVec[m_lastTweenIndex].finalValue = finalVal;

	return *this;
}

template<class T>STween<T>& STween<T>::Time(float sec)
{
	m_tweenVec[m_lastTweenIndex].duration = sec;

	return *this;
}

template<class T> void STween<T>::Update(float deltaTime)
{
	std::vector<TweenData<T>> tweensToAdd;
	std::vector<TweenData<T>> tweensToDelete;

	for (auto &STween : m_tweenVec)
	{
		if (STween.fromReady)
		{
			float timePos = STween.timeCounter / STween.duration;

			T value, start, end;

			if (STween.reversed)
			{
				start = STween.finalValue;
				end = STween.initialCpy;
			}
			else
			{
				start = STween.initialCpy;
				end = STween.finalValue;
			}

			switch (STween.easing)
			{
			case EasingFunction::Linear:
				value = Linear(timePos, start, end);
				break;
			case EasingFunction::QuadranticIn:
				value = QuadIn(timePos, start, end);
				break;

			case EasingFunction::QuadranticOut:
				value = QuadOut(timePos, start, end);
				break;

			case EasingFunction::QuadranticInOut:
				value = QuadInOut(timePos, start, end);
				break;

			case EasingFunction::CubicIn:
				value = CubicIn(timePos, start, end);
				break;

			case EasingFunction::CubicOut:
				value = CubicOut(timePos, start, end);
				break;

			case EasingFunction::CubicInOut:
				value = CubicInOut(timePos, start, end);
				break;

			case EasingFunction::QuintIn:
				value = QuintIn(timePos, start, end);
				break;

			case EasingFunction::QuintOut:
				value = QuintOut(timePos, start, end);
				break;

			case EasingFunction::QuintInOut:
				value = QuintInOut(timePos, start, end);
				break;

			case EasingFunction::BackIn:
				value = BackIn(timePos, start, end);
				break;

			case EasingFunction::BackOut:
				value = BackOut(timePos, start, end);
				break;

			case EasingFunction::BackInOut:
				value = BackInOut(timePos, start, end);
				break;

			default:
				value = Linear(timePos, start, end);
				break;
			}

			*STween.initialValue = value;
			if (STween.stepCallback)
			{
				STween.stepCallback(value);
			}

			if (STween.timeCounter >= STween.duration)
			{
				if (STween.reversed)
					*STween.initialValue = STween.initialCpy;
				else
					*STween.initialValue = STween.finalValue;

				STween.fromReady = false;
				if (STween.finishCallback)
				{
					STween.finishCallback();
				}

				for (auto &callbackTw : STween.endTween)
				{
					auto endTween = callbackTw;
					tweensToAdd.push_back(endTween);
				}

				tweensToDelete.push_back(STween);
			}

			STween.timeCounter += deltaTime;
		}
	}

	for (TweenData<T> &oldTween : tweensToDelete)
	{
		auto position = std::find(m_tweenVec.begin(), m_tweenVec.end(), oldTween);
		if (position != m_tweenVec.end())
		{
			m_tweenVec.erase(position);
			m_lastTweenIndex--;
		}
	}

	for (auto &newTween : tweensToAdd)
	{
		AddTween(newTween);
	}
}

template<class T>STween<T>& STween<T>::OnFinish(std::function<void()> endCallback)
{
	m_tweenVec[m_lastTweenIndex].finishCallback = endCallback;

	return *this;
}

template<class T>STween<T>& STween<T>::OnStep(std::function<void(T&)> callback)
{

	m_tweenVec[m_lastTweenIndex].stepCallback = callback;

	return *this;
}

template<class T>STween<T>& STween<T>::Chain(STween<T>* chain)
{
	m_tweenVec[m_lastTweenIndex].endTween = chain->GetTweens();

	return *this;
}

template<class T>STween<T>& STween<T>::Reversed(bool isReversed)
{
	m_tweenVec[m_lastTweenIndex].reversed = isReversed;

	return *this;
}

template<class T>STween<T>& STween<T>::Easing(EasingFunction easingType)
{
	m_tweenVec[m_lastTweenIndex].easing = easingType;

	return *this;
}

template<class T>std::vector<TweenData<T>> STween<T>::GetTweens()
{
	return m_tweenVec;
}

template<class T>void STween<T>::AddTween(TweenData<T> STween)
{
	STween.tweenID = m_lastTweenIndex;
	m_tweenVec.push_back(STween);
	m_lastTweenIndex++;
}

template<class T>void STween<T>::AddTweens(std::vector<TweenData<T>> tweens)
{
	for (auto &STween : tweens)
	{
		AddTween(STween);
	}
}

template<class T>T STween<T>::Linear(float position, T start, T end)
{
	return static_cast<T>((end - start) * position + start);
}

template<class T>T STween<T>::QuadIn(float position, T start, T end)
{
	return static_cast<T>((end - start) * position * position + start);
}

template<class T>T STween<T>::QuadOut(float position, T start, T end)
{
	return static_cast<T>(((end - start)* -1) * position * (position - 2) + start);
}

template<class T>T STween<T>::QuadInOut(float position, T start, T end)
{
	position *= 2;
	if (position < 1)
		return static_cast<T>(((end - start) / 2) * position * position + start);

	--position;
	return static_cast<T>((((end - start) * 1) / 2)* (position * (position - 2) - 1) + start);
}

template<class T>T STween<T>::CubicIn(float position, T start, T end)
{
	return static_cast<T>((end - start) * position * position * position + start);
}

template<class T>T STween<T>::CubicOut(float position, T start, T end)
{
	--position;
	return static_cast<T>((end - start) * (position * position * position + 1) + start);
}

template<class T>T STween<T>::CubicInOut(float position, T start, T end)
{
	position *= 2;
	if (position < 1)
		return static_cast<T>(((end - start) / 2) * position * position * position + start);

	position -= 2;
	return static_cast<T>(((end - start) / 2) * (position * position * position + 2) + start);
}

template<class T>T STween<T>::QuintIn(float position, T start, T end)
{
	return static_cast<T>((end - start) * position * position * position * position * position + start);
}

template<class T>T STween<T>::QuintOut(float position, T start, T end)
{
	--position;
	return static_cast<T>((end - start) * (position * position * position * position * position + 1) + start);
}

template<class T>T STween<T>::QuintInOut(float position, T start, T end)
{
	position *= 2;
	if (position < 1)
		return static_cast<T>(((end - start) / 2) * (position * position * position * position * position) + start);

	position -= 2;
	return static_cast<T>(((end - start) / 2) * (position * position * position * position * position + 2) + start);
}

template<class T>T STween<T>::BackIn(float position, T start, T end)
{
	float s = 1.70158f;
	float postFix = position;
	return static_cast<T>((end - start) * postFix * position * ((s + 1) * position - s) + start);
}

template<class T>T STween<T>::BackOut(float position, T start, T end)
{
	float s = 1.70158f;
	position--;
	return static_cast<T>((end - start) * (position * position * ((s + 1) * position + s) + 1) + start);
}

template<class T>T STween<T>::BackInOut(float position, T start, T end)
{
	float s = 1.70158f;
	float t = position;
	T b = start;
	T c = end - start;
	float d = 1;
	s *= 1.525f;
	if ((t /= d / 2) < 1)
		return static_cast<T>(c / 2 * (t * t * ((s + 1) * t - s)) + b);

	float postFix = t -= 2;
	return static_cast<T>(c / 2 * (postFix * t * ((s + 1) * t + s) + 2) + b);
}
}

#endif //_S_TWEEN_H_