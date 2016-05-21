%{
#include "Math/Vector3.h"
	%}

namespace Math {
	typedef const Math::Vector3& Vector3Param;
	typedef Math::Vector3& Vector3Ref;
	typedef Math::Vector3* Vector3Ptr;
	typedef const Math::Vector4& Vector4Param;
	typedef Math::Vector4& Vector4Ref;
	typedef Math::Vector4* Vector4Ptr;
	typedef const Math::Vector2& Vector2Param;
	typedef Math::Vector2& Vector2Ref;
	typedef Math::Vector2* Vector2Ptr;

    %rename(MochaVec2) Vector2;
	struct Vector2
	{
		Vector2(void);
		explicit Vector2(float x, float y);
		explicit Vector2(ConstRealPointer data);

		%rename(Add) operator+;
		%rename(Sub) operator-;
		%rename(Mul) operator*;
		%rename(GetE) operator[];
		%rename(Div) operator/;
		%rename(Comp) operator==;
		%rename(NegComp) operator!=;

		float& operator[](unsigned index);

		//Unary Operators
		Vector2 operator-(void) const;

		//Binary Operators (Reals)
		Vector2 operator*(float rhs) const;
		Vector2 operator/(float rhs) const;

		//Binary Operators (Vectors)
		Vector2 operator+(Vector2Param rhs) const;
		Vector2 operator-(Vector2Param rhs) const;

		//Binary Vector Comparisons
		bool operator==(Vector2Param rhs) const;
		bool operator!=(Vector2Param rhs) const;

		//Vector component-wise multiply and divide
		Vector2 operator*(Vector2Param rhs) const;
		Vector2 operator/(Vector2Param rhs) const;

		///Set all of the values of this Vector at once.
		void Set(float x, float y);

		///Set all of this Vector's elements to 0.
		void ZeroOut(void);

		///Calculate and return this Vector reflected about the given Vector.
		Vector2 Reflect(Vector2Param reflectionAxis) const;

		///Add a Vector multiplied by a scalar to this Vector. A commonly done 
		///operation and this reduces temporaries.
		void AddScaledVector(Vector2Param Vector, float scalar);

		///Compute the dot product of this Vector with the given Vector.
		float Dot(Vector2Param rhs) const;

		///Get the length of this Vector.
		float Length(void) const;

		///Get the squared length of this Vector.
		float LengthSq(void) const;

		///Calculate and return a unit-length copy of this Vector.
		Vector2 Normalized(void) const;

		///Make this Vector have a length of 1, returns the original length.
		float Normalize(void);

		///Attempt to give this Vector a length of 1, but checks if it's possible.
		///Instead of crashing, will return 0 if the Vector was not able to be 
		///normalized.
		float AttemptNormalize(void);

		///Flips this Vector so it's pointing in the opposite direction.
		Vector2Ref Negate(void);

		///Checks to see if the values of this Vector's elements are usable.
		bool Valid(void) const;

		void Splat(float value);
		
		%typemap(cscode) Vector2 %{
		public MochaVec2(Microsoft.Xna.Framework.Vector2 vec)
		{
			x = vec.X;
            y = vec.Y;
		}
		public static implicit operator MochaVec2(Microsoft.Xna.Framework.Vector2 vec)  // explicit byte to digit conversion operator
		{
			return new MochaVec2(vec);
		}
		public static implicit operator Microsoft.Xna.Framework.Vector2(MochaVec2 vec)  // explicit byte to digit conversion operator
		{
			Microsoft.Xna.Framework.Vector2 tmp = new Microsoft.Xna.Framework.Vector2();
			
			tmp.X = vec.x;
			tmp.Y = vec.y;

			return tmp;
		}
	    %}
		union
		{
			struct
			{
				float x, y;
			};
			float array[2];
		};
	};
    %rename(MochaVec4) Vector4;
	struct Vector4
	{
		Vector4(void);
		explicit Vector4(float x, float y, float z, float w = 0);
		explicit Vector4(ConstRealPointer data);
		//Splat all elements
		explicit Vector4(float xyzw);

		%rename(Add) operator+;
		%rename(Sub) operator-;
		%rename(Mul) operator*;
		%rename(GetE) operator[];
		%rename(Div) operator/;
		%rename(Comp) operator==;
		%rename(NegComp) operator!=;

		float& operator[](unsigned index);

		//Unary Operators
		Vector4 operator-(void) const;

		//Binary Operators (Reals)
		Vector4 operator*(float rhs) const;
		Vector4 operator/(float rhs) const;

		//Binary Operators (Vectors)
		Vector4 operator+(Vector4Param rhs) const;
		Vector4 operator-(Vector4Param rhs) const;

		//Binary Vector Comparisons
		bool operator==(Vector4Param rhs) const;
		bool operator!=(Vector4Param rhs) const;

		//Vector component wise multiply and divide
		Vector4 operator*(Vector4Param rhs) const;
		Vector4 operator/(Vector4Param rhs) const;

		void Set(float x, float y, float z, float w = 0);

		///Set all of the values of the Vector to the passed in value.
		void Splat(float xyzw);

		///Do a component-wise scaling of this Vector with the given Vector.
		void ScaleByVector(Vector4Param rhs);

		Vector4 ScaledByVector(Vector4Param rhs) const;

		void ZeroOut(void);
		void AddScaledVector(Vector4Param Vector, float scalar);

		float Dot(Vector4Param rhs) const;
		float Length(void) const;
		float LengthSq(void) const;
		Vector4 Normalized(void) const;
		float Normalize(void);
		float AttemptNormalize(void);
		Vector4Ref Negate(void);
		bool Valid(void) const;
		Vector4 Vector4::Cross(Vector4Param rhs) const;
		
		%typemap(cscode) Vector4 %{
		public MochaVec4(Microsoft.Xna.Framework.Vector4 vec)
		{
			x = vec.X;
            y = vec.Y;
            z = vec.Z;
			w = vec.W;
		}
		public static implicit operator MochaVec4(Microsoft.Xna.Framework.Vector4 vec)  // explicit byte to digit conversion operator
		{
			return new MochaVec4(vec);
		}
		public static implicit operator Microsoft.Xna.Framework.Vector4(MochaVec4 vec)  // explicit byte to digit conversion operator
		{
			Microsoft.Xna.Framework.Vector4 tmp = new Microsoft.Xna.Framework.Vector4();
			
			tmp.X = vec.x;
			tmp.Y = vec.y;
			tmp.Z = vec.z;
			tmp.W = vec.w;
			
			return tmp;
		}
	    %}
		
		union
		{
			struct
			{
				float x, y, z, w;
			};
			float array[4];
		};
	};
	

    %rename(MochaVec3) Vector3;
	%typemap(cscode) Vector3 %{

		public override string ToString()
		{
			return "(" + x.ToString() + ", " + y.ToString() + ", " + z.ToString() + ")";
		}
		public MochaVec3(Microsoft.Xna.Framework.Vector3 vec) : this(vec.X, vec.Y, vec.Z)
		{
		}
		public static implicit operator MochaVec3(Microsoft.Xna.Framework.Vector3 vec)  // explicit byte to digit conversion operator
		{
			return new MochaVec3(vec);
		}
		public static implicit operator Microsoft.Xna.Framework.Vector3(MochaVec3 vec)  // explicit byte to digit conversion operator
		{
			Microsoft.Xna.Framework.Vector3 tmp = new Microsoft.Xna.Framework.Vector3();

			tmp.X = vec.x;
			tmp.Y = vec.y;
			tmp.Z = vec.z;

			return tmp;
		}
	%}

	struct Vector3
	{
	public:
		Vector3(void);
		explicit Vector3(float x, float y, float z);
		//Splat all elements
		explicit Vector3(float xyz);
		explicit Vector3(Vector2Param Vector2, float z = 0.0f);
		explicit Vector3(ConstRealPointer data);
		Vector3(Vector3Param Vector3);
		
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
		Vector3 operator-(void) const;

		//Binary Operators (Reals)
		Vector3 operator*(float rhs) const;
		Vector3 operator/(float rhs) const;

		//Binary Operators (Vectors)
		Vector3 operator+(Vector3Param rhs) const;
		Vector3 operator-(Vector3Param rhs) const;

		//Binary Vector Comparisons
		bool operator==(Vector3Param rhs) const;
		bool operator!=(Vector3Param rhs) const;

		//Vector component wise multiply and divide
		Vector3 operator*(Vector3Param rhs) const;
		Vector3 operator/(Vector3Param rhs) const;


		///Set all of the values of this Vector at once.
		void Set(float x, float y, float z);

		///Set all of the values of the Vector to the passed in value.
		void Splat(float xyz);

		///Do a component-wise scaling of this Vector with the given Vector.
		void ScaleByVector(Vector3Param rhs);

		///Set all of this Vector's elements to 0.
		void ZeroOut(void);

		///Calculate and return this Vector reflected about the given Vector.
		Vector3 Reflect(Vector3Param reflectionAxis) const;

		///Projects the Vector onto the axis
		Vector3 Project(Vector3Param axis);

		///Add a Vector multiplied by a scalar to this Vector. A commonly done 
		///operation and this reduces temporaries.
		void AddScaledVector(Vector3Param Vector, float scalar);

		///Compute the dot product of this Vector with the given Vector.
		float Dot(Vector3Param rhs) const;

		///Get the length of this Vector.
		float Length(void) const;

		///Get the squared length of this Vector.
		float LengthSq(void) const;

		///Calculate and return a unit-length copy of this Vector.
		Vector3 Normalized(void) const;

		///Make this Vector have a length of 1, returns the original length.
		float Normalize(void);

		///Ceil each component of the Vector.
		void Ceil(void);

		///Floor each component of the Vector.
		void Floor(void);

		///Truncate each component of the Vector.
		void Truncate(void);

		///If positive, it always rounds up, if negative, it always rounds down
		void RoundToExtremes();

		///Round each component of the Vector.
		void Round(void);

		///Attempt to give this Vector a length of 1, but checks if it's possible.
		///Instead of crashing, will return 0 if the Vector was not able to be 
		///normalized.
		float AttemptNormalize(void);

		///Flip this Vector so it's pointing in the opposite direction.
		Vector3Ref Negate(void);

		///Checks to see if the values of this Vector's elements are usable.
		bool Valid(void) const;

		///Compute the cross product of this Vector with the given Vector.
		Vector3 Cross(Vector3Param rhs) const;
		void InvertComponents(void);

		float Distance(const Vector3* rhs);

		///Compute the cross product of the two given Vectors for 2d.
		///The result is only the z axis of the cross product.
		Vector3 Cross2d(const Vector3* rhs);

		///Returns the component-wise minimum Vector of the two Vectors.
		Vector3 Min(const Vector3* rhs);

		///Returns the component-wise maximum Vector of the two Vectors.
		Vector3 Max(const Vector3* rhs);

		///Linearly interpolate between the two Vectors, the t-value is restricted to 
		///the range [0, 1].
		Vector3 Lerp(const Vector3* end, float tValue);

		///Calculates and returns the given Vector with its values clamped to the range
		///[min, max].
		void Clamp(float min, float max);

		///Returns if all values in lhs are less than all values in rhs
		bool AllLess(const Vector3* rhs);

		///Returns if any value in lhs is less than any value in rhs
		bool AnyLess(const Vector3* rhs);

		///Returns if all values in lhs are greater than all values in rhs
		bool AllGreater(const Vector3* rhs);

		///Returns if any value in lhs is greater than any value in rhs
		bool AnyGreater(const Vector3* rhs);

		union
		{
			struct
			{
				float x, y, z;
			};
			float array[3];
		};
		
	};
}

