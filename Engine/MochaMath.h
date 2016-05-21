#pragma once
#include "stdafx.h"
#include "MathUtilities.h"

namespace Math
{

	/*! Cosine interpolate a number to a value over time */
	float Cerp(float start, float end, float percent);

	/*! Linearly interpolate a number to a value over time */
	float Lerp(float start, float end, float percent);

	/*! Keep value between two numbers */
	float Clamp(float num, float min, float max);

	//Represents a range between two numbers, with DataNode support and random facilities
	class Range
	{
		float start, end;
	public:
		Range( ) : start(0.0f), end(0.0f) { }
		Range( float _start, float _end ) : start(_start), end(_end) { }
		virtual ~Range() { }

		virtual float GetStart( ) const		{ return start;			}
		virtual float GetEnd( )	const		{ return end;			}
		virtual float GetDifference() const	{ return end - start;	}
		virtual float GetRandomInRange() const
		{
			return RandomFloat(start,end);
		}

		virtual float SetStart( float _v )	{ start = _v; return start;	}
		virtual float SetEnd( float _v )	{ end	= _v; return end;	}
	};
	//Represents a range with a value contained in the range, the value is clamped to the range bounds
	class RangeValue : public Range
	{
		float value;
	public:
		RangeValue( ) : Range( ), value(0.0f) { }
		RangeValue( float _v, float _start, float _end ) : Range(_start,_end), value(_v) { }

		float GetValue( ) const			{ return value; }
		float SetValue( float _v )	{ value = Clamp(_v, GetStart(), GetEnd());	}
	};

}