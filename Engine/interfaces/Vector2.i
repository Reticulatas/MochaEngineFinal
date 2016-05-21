%{
#include "Math/Vector2.h"
	%}

namespace Math {
	typedef const Vector2& Vec2Param;
	typedef Vector2& Vec2Ref;
	typedef Vector2* Vec2Ptr;

	struct Vector2
	{
		Vector2(void);
		explicit Vector2(float x, float y);
		explicit Vector2(ConstRealPointer data);

		float* ToFloats();

		%rename(Add) operator+;
		%rename(Sub) operator-;
		%rename(Mul) operator*;
		%rename(GetE) operator[];
		%rename(Div) operator/;
		%rename(Comp) operator==;
		%rename(NegComp) operator!=;

		float operator[](unsigned index) const;

		//Unary Operators
		Vector2 operator-(void) const;

		//Binary Operators (Reals)
		Vector2 operator*(float rhs) const;
		Vector2 operator/(float rhs) const;

		//Binary Operators (vectors)
		Vector2 operator+(Vec2Param rhs) const;
		Vector2 operator-(Vec2Param rhs) const;

		//Binary Vector Comparisons
		bool operator==(Vec2Param rhs) const;
		bool operator!=(Vec2Param rhs) const;

		//Vector component-wise multiply and divide
		Vector2 operator*(Vec2Param rhs) const;
		Vector2 operator/(Vec2Param rhs) const;

		///Set all of the values of this vector at once.
		void Set(float x, float y);

		///Set all of this vector's elements to 0.
		void ZeroOut(void);

		///Calculate and return this vector reflected about the given vector.
		Vector2 Reflect(Vec2Param reflectionAxis) const;

		///Add a vector multiplied by a scalar to this vector. A commonly done 
		///operation and this reduces temporaries.
		void AddScaledVector(Vec2Param vector, float scalar);

		///Compute the dot product of this vector with the given vector.
		float Dot(Vec2Param rhs) const;

		///Get the length of this vector.
		float Length(void) const;

		///Get the squared length of this vector.
		float LengthSq(void) const;

		///Calculate and return a unit-length copy of this vector.
		Vector2 Normalized(void) const;

		///Make this vector have a length of 1, returns the original length.
		float Normalize(void);

		///Attempt to give this vector a length of 1, but checks if it's possible.
		///Instead of crashing, will return 0 if the vector was not able to be 
		///normalized.
		float AttemptNormalize(void);

		///Flips this vector so it's pointing in the opposite direction.
		Vec2Ref Negate(void);

		///Checks to see if the values of this vector's elements are usable.
		bool Valid(void) const;

		void Splat(float value);

		union
		{
			struct
			{
				float x, y;
			};
			float array[2];
		};

		static const Vector2 cZero;
		static const Vector2 cXAxis;
		static const Vector2 cYAxis;
	};
}