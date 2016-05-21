using Microsoft.Xna.Framework;
using System;

//Mocha Math
public class MMath
{
	public static float Lerp(float start, float end, float t)
	{
		return start + (end-start)*t;
	}

	public static float Clamp(float value, float min, float max)
	{
		if (value < min)
			return min;
		if (value > max)
			return max;
		return value;
	}

	public static float Clamp01(float value)
	{
		return Clamp(value,0.0f,1.0f);
	}

	public static float Cerp(float start, float end, float percent)
	{
		double a = (double)(start);
		double b = (double)(end);
		double mu = (double)(percent);
		double mu2;
		mu2 = (1 - Math.Cos(mu*Math.PI)) / 2;
		return (float)((a*(1 - mu2) + b*mu2));
	}

	public static Vector3 CerpVector(Vector3 start, Vector3 end, float percent)
	{
		Vector3 result = new Vector3();
		result.X = MMath.Cerp(start.X, end.X, percent);
		result.Y = MMath.Cerp(start.Y, end.Y, percent);
		result.Z = MMath.Cerp(start.Z, end.Z, percent);
		return result;
	}
	public static Vector3 LerpVector(Vector3 start, Vector3 end, float percent)
	{
		Vector3 result = new Vector3();
		result.X = MMath.Lerp(start.X, end.X, percent);
		result.Y = MMath.Lerp(start.Y, end.Y, percent);
		result.Z = MMath.Lerp(start.Z, end.Z, percent);
		return result;
	}

	public static float Angle(Vector3 a, Vector3 b)
	{
		float dotf = Vector3.Dot(a,b);
		dotf = dotf/(a.Length()*b.Length());
		float acos = (float)Math.Acos(dotf);
		return acos*180.0f/3.1415f;
	}

	public static Vector4 Vec3ToVec4(Vector3 v, float w = 0.0f)
	{
		return new Vector4(v.X, v.Y, v.Z, w);
	}

	static Random _random = new Random();

	public static Vector3 GetRandomPointInCircle(Vector3 position, float radius)
	{
		float randomRadius = radius * (float)Math.Sqrt(_random.NextDouble());

		double randomAngle = _random.NextDouble() * MathHelper.TwoPi;

		float x = randomRadius * (float)Math.Cos(randomAngle);
		float z = randomRadius * (float)Math.Sin(randomAngle);

		return new Vector3(position.X + x, position.Y, position.Z + z);
	}

	public static int GetRandomInt(int inc_lower, int exc_higher)
	{
		return _random.Next(inc_lower, exc_higher);
	}

	public static float GetRandomFloat()
	{
		return (float)_random.NextDouble();
	}

    public static float GetRandomLimitedFloat(float minValue, float maxValue)
    {
		//if(minValue == 0.0f && maxValue == 0.0f)
		//	return 0.0f;
	
        float randValue = (float)_random.NextDouble(); // [0, 1)
        randValue *= (maxValue - minValue);
        randValue += minValue;
        return randValue;
    }

    public static Vector3 Slerp(Vector3 a, Vector3 b, float t)
    {
        return (1 - t) * a + t * b;
    }

    public static float GetAngleBetweenVectors(Vector3 a, Vector3 b)
    {
        a.Normalize();
        b.Normalize();
        return (float)Math.Acos((Vector3.Dot(a,b)) / (a.Length() * b.Length()));
    }

    public static bool RayToPlaneIntersection(Vector3 planeNormal, 
        Vector3 planePoint, Vector3 rayVector, Vector3 rayPoint, ref Vector3 returnPosition)
    {
        Vector3 rayPtToPlane = planePoint - rayPoint;
        float numerator = Vector3.Dot(planeNormal, rayPtToPlane);
        float denominator = Vector3.Dot(planeNormal, rayVector);
        if (denominator == 0.0f)
        {
            return false;
        }
        float t = numerator / denominator;
        if (t <= 0.0f)
        {
            return false;
        }

        returnPosition = rayPoint + t * rayVector;
        return true;
    }
}
