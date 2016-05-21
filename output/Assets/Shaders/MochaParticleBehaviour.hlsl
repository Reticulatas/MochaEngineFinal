#define RANDOM_IA 16807
#define RANDOM_IM 2147483647
#define RANDOM_AM (1.0f/float(RANDOM_IM))
#define RANDOM_IQ 127773
#define RANDOM_IR 2836
#define RANDOM_MASK 123459876


static int random_x;

float random()
{
    int k;
    float ans;

    random_x ^= RANDOM_MASK;
    k = random_x / RANDOM_IQ;
    random_x = RANDOM_IA * (random_x - k * RANDOM_IQ) - RANDOM_IR * k;

    if (random_x < 0)
        random_x += RANDOM_IM;

    ans = RANDOM_AM * random_x;
    random_x ^= RANDOM_MASK;

    return ans;
}

float random(float low, float high)
{
    float v = random();
    return low * (1.0f - v) + high * v;
}

float2 random(float2 low, float2 high)
{
    float2 v = float2(random(), random());
        return low * (1.0f - v) + high * v;
}

float3 random(float3 low, float3 high)
{
    float3 v = float3(random(), random(), random());
        return low * (1.0f - v) + high * v;
}

void seed(int value)
{
    random_x = value;
    random();
}

struct EmitterProperties
{
    uint mGroupDimX;
    uint mGroupDimY;
    uint mMaxParticles;
    uint mUsedParticles;

    float mMinLifetime;
    float mMaxLifetime;
    float mMinVel;
    float mMaxVel;
    

    float mFadeStartAmt;
    float mFadeEndAmt;
    float mDT;
    float mTotalGameTime;

    float mEmissionAngleVariance;
    float3 mEmissionAngle;

    int mIsEmitting;
    float3 mEmitterPos;

    float3 mCamPos;
    int mRandomDirection;

    float mGravityFactor;
    float mRadius; // CIRCLE EMISSION VARIABLES
    float mStartSize;
    float mEndSize;

    float4 mStartColour;
    float4 mEndColour;

    // ATTRACTOR
    float3 mAttractorPos;
    int    mAttracted;

    int  mAttractorLocalSpace;
    float mAttractorFactor;
    float padding2[2];

    //BOX EMISSION VARIABLES
    float3 mHalfLengths;
    int mType;

    int mDeceleration;
    int mSpawnInGeometry;
    int mDieInGeometry;
    int mWarmCount;

    // PLANE EMISSION
    float3 mNormal;
    int mPlaneEmit;

    // INHERITED VELOCITY
    float3 mInheritedVelocity;
    float mInheritedFactor;

    // MATRICES AND STUFF


};

struct Particle
{
    float3 mPos;
    float3 mVel;
    float mTotalLifeTime;
    float mLifetime;
    float mParticleSize;
    float4 mColor;
    // [0] used for depth
    // [1] unused
    // [2] unused
    //float3 mSpecialData;
};


const static float initial_vel_dampener = 0.005f;
const static float gravity_vel_dampener = 0.0005f;
const static float vel_dampener = 0.999995f;
const static float capFPS = 0.016666666667f;
Particle DefaultBehaviour(EmitterProperties prop, Particle input, uint ID)
{
    Particle p = input;

    uint particleCap;

    if (prop.mUsedParticles < prop.mMaxParticles)
        particleCap = prop.mUsedParticles;
    else
        particleCap = prop.mMaxParticles;

    

    if (ID < particleCap)
    {
        int time = (int)prop.mTotalGameTime;
        seed(ID + time);

        //float mainSeed = random(ID, mTotalGameTime);
        if (prop.mIsEmitting)
        {
            float sign = 1.0f;

            if ((ID % 2) == 0)
                sign *= -1.0f;
            //If alive
            if (p.mLifetime > 0)
            {
                // LIFETIME
                if (prop.mDT > capFPS)
                    p.mLifetime -= capFPS;
                else
                    p.mLifetime -= prop.mDT;
                float fade = p.mLifetime / p.mTotalLifeTime; 
                // VEL
                if (!prop.mAttracted)
                {
                    p.mVel.y -= prop.mGravityFactor * gravity_vel_dampener;

                    // INHERITED VELOCITY
                    p.mVel += prop.mInheritedVelocity * prop.mInheritedFactor;
                    p.mPos += p.mVel;
                    p.mVel *= vel_dampener;
                }
                else
                {
                    float3 randVec;
                    randVec.x = random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);
                    randVec.y = random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);
                    randVec.z = random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);

                    float3 dirVec;

                    if (prop.mAttractorLocalSpace)
                    {
                        //if (prop.mAttractorPos.x == 0.0f &&
                        //    prop.mAttractorPos.y == 0.0f &&
                        //    prop.mAttractorPos.z == 0.0f)
                        //    dirVec = randVec;
                        //else
                            dirVec = prop.mEmitterPos + prop.mAttractorPos - p.mPos;
                    }
                    else
                        dirVec = prop.mAttractorPos - p.mPos; 

                    if (dot(dirVec,dirVec) != 0.0f)
                        dirVec = normalize(dirVec);
                    dirVec += randVec;
                    p.mVel += dirVec * initial_vel_dampener * prop.mAttractorFactor;



                    // INHERITED VELOCITY
                    p.mVel += prop.mInheritedVelocity * prop.mInheritedFactor;
                    p.mPos += p.mVel;
                    p.mVel *= vel_dampener;
                }
                
                // SIZE
                float currSizeAppend = (1.0f - fade) * (prop.mEndSize - prop.mStartSize);
                p.mParticleSize = prop.mStartSize + currSizeAppend;

                // COLOUR
                float red = saturate(0.1f*(1.0f - fade) * (prop.mEndColour.r - prop.mStartColour.r));
                float green = saturate(0.1f*(1.0f - fade) * (prop.mEndColour.g - prop.mStartColour.g));
                float blue = saturate(0.1f*(1.0f - fade) * (prop.mEndColour.b - prop.mStartColour.b));
                p.mColor.r += red;
                p.mColor.g += green;
                p.mColor.b += blue;

                // FADE
                float currFadeAppend = (1.0f - fade) * (prop.mFadeEndAmt - prop.mFadeStartAmt);
                p.mColor.w = prop.mFadeStartAmt + currFadeAppend;
            }
            // Else dead
            else
            {
                // VEL & POS
                p.mPos = prop.mEmitterPos;

                if (prop.mRandomDirection)
                {

                    p.mVel.x += sign * random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);
                    p.mVel.y += sign * random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);
                    p.mVel.z += sign * random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);

                    p.mVel = normalize(p.mVel);
                    p.mVel *= random(prop.mMinVel, prop.mMaxVel) * initial_vel_dampener;

                }
                else
                {
                    p.mVel = prop.mEmissionAngle;

                    p.mVel.x += sign * random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);
                    p.mVel.y += sign * random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);
                    p.mVel.z += sign * random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);

                    p.mVel = normalize(p.mVel);
                    p.mVel *= random(prop.mMinVel, prop.mMaxVel) * initial_vel_dampener;
                    
                    if (prop.mAttracted)
                    {
                        //p.mVel *= prop.mAttractorFactor * 3.0f;
                    }
                }
                // SIZE
                p.mParticleSize = prop.mStartSize;
                p.mColor = prop.mStartColour;
                // FADE
                p.mColor.w = prop.mFadeStartAmt;
                // LIFETIME
                p.mTotalLifeTime = random(prop.mMinLifetime, prop.mMaxLifetime);
                p.mLifetime = p.mTotalLifeTime;

            }

        }
        else
        {
            float sign = 1.0f;

            if ((ID % 2) == 0)
                sign *= -1.0f;

            //If alive
            if (p.mLifetime > 0)
            {
                // LIFETIME
                if (prop.mDT > capFPS)
                    p.mLifetime -= capFPS;
                else
                    p.mLifetime -= prop.mDT;
                float fade = p.mLifetime / p.mTotalLifeTime; 
                // VEL
                if (!prop.mAttracted)
                {
                    p.mVel.y -= prop.mGravityFactor * gravity_vel_dampener;

                    // INHERITED VELOCITY
                    p.mVel += prop.mInheritedVelocity * prop.mInheritedFactor;
                    p.mPos += p.mVel;
                    p.mVel *= vel_dampener;
                }
                else
                {
                    float3 randVec;
                    randVec.x = random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);
                    randVec.y = random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);
                    randVec.z = random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);

                    float3 dirVec;

                    if (prop.mAttractorLocalSpace)
                        dirVec = prop.mEmitterPos + prop.mAttractorPos - p.mPos;
                    else
                        dirVec = prop.mAttractorPos - p.mPos;

                    dirVec += randVec;
                    p.mVel += dirVec * initial_vel_dampener * prop.mAttractorFactor;

                    // INHERITED VELOCITY
                    p.mVel += prop.mInheritedVelocity * prop.mInheritedFactor;
                    p.mPos += p.mVel;
                    p.mVel *= vel_dampener;
                }

                // SIZE
                float currSizeAppend = (1.0f - fade) * (prop.mEndSize - prop.mStartSize);
                p.mParticleSize = prop.mStartSize + currSizeAppend;
                
                // COLOUR
                float red = saturate(0.1f*(1.0f - fade) * (prop.mEndColour.r - prop.mStartColour.r));
                float green = saturate(0.1f*(1.0f - fade) * (prop.mEndColour.g - prop.mStartColour.g));
                float blue = saturate(0.1f*(1.0f - fade) * (prop.mEndColour.b - prop.mStartColour.b));
                p.mColor.r += red;
                p.mColor.g += green;
                p.mColor.b += blue;
                // FADE
                float currFadeAppend = (1.0f - fade) * (prop.mFadeEndAmt - prop.mFadeStartAmt);
                p.mColor.w = prop.mFadeStartAmt + currFadeAppend;
            }
            else
            {
                //p.mColor.x = 0.0f;
                p.mColor.w = 0.0f;
            }
        }

        return p;
    }
    else
    {
        p.mLifetime = -10.f;
        p.mColor.w = 0.0f;
        return p;
    }
}

float3 InsideAABB(float3 halfWidth, float3 pos)
{
    float3 ret;

    ret.x = random(pos.x - halfWidth.x, pos.x + halfWidth.x);
    ret.y = random(pos.y - halfWidth.y, pos.y + halfWidth.y);
    ret.z = random(pos.z - halfWidth.z, pos.z + halfWidth.z);

    return ret;
}

float3 OutsideAABB(float3 halfWidth, float3 pos)
{
    float3 ret = InsideAABB(halfWidth, pos);

    float checkX = (pos.x - ret.x);
    float checkY = (pos.y - ret.y);
    float checkZ = (pos.z - ret.z);
    int temp = (int)random(0.0f, 3.f);

    float res = max(max(checkX, checkY), checkZ);

    if (temp == 0)
    {
        if (checkX > 0.0f)
            ret.x = pos.x + halfWidth.x;
        else
            ret.x = pos.x - halfWidth.x;
    }
    else if (temp == 1)
    {
        if (checkY > 0.0f)
            ret.y = pos.y + halfWidth.y;
        else
            ret.y = pos.y - halfWidth.y;
    }
    else
    {
        if (checkZ > 0.0f)
            ret.z = pos.z + halfWidth.z;
        else
            ret.z = pos.z - halfWidth.z;
    }

    return ret;
}

bool CheckOutOfBounds(float3 halfWidth, float3 pos, float3 particlePos)
{
    float3 max = pos + halfWidth;
    float3 min = pos - halfWidth;

    max += float3(0.5f, 0.5f, 0.5f);
    min -= float3(0.5f, 0.5f, 0.5f);

    return !(particlePos.x > min.x && particlePos.x < max.x &&
             particlePos.y > min.y && particlePos.y < max.y &&
             particlePos.z > min.z && particlePos.z < max.z);
}

Particle BoxBehaviour(EmitterProperties prop, Particle input, uint ID)
{
    Particle p = input;

    uint particleCap;

    if (prop.mUsedParticles < prop.mMaxParticles)
        particleCap = prop.mUsedParticles;
    else
        particleCap = prop.mMaxParticles;

    if (ID < particleCap)
    {
        int time = (int)prop.mTotalGameTime;
        seed(ID + time);

        //float mainSeed = random(ID, mTotalGameTime);
        if (prop.mIsEmitting)
        {
            float sign = 1.0f;

            if ((ID % 2) == 0)
                sign *= -1.0f;

            //If alive
            if (p.mLifetime > 0)
            {
                // LIFETIME
                if (prop.mDT > capFPS)
                    p.mLifetime -= capFPS;
                else
                    p.mLifetime -= prop.mDT;

                float fade = p.mLifetime / p.mTotalLifeTime; 
                // VEL
                if (!prop.mAttracted)
                {
                    p.mVel.y -= prop.mGravityFactor * gravity_vel_dampener;

                    // INHERITED VELOCITY
                    p.mVel += prop.mInheritedVelocity * prop.mInheritedFactor;
                    p.mPos += p.mVel;
                    p.mVel *= vel_dampener;
                }
                else
                {
                    float3 randVec;
                    randVec.x = random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);
                    randVec.y = random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);
                    randVec.z = random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);

                    float3 dirVec = prop.mAttractorPos - p.mPos;

                    if (prop.mAttractorLocalSpace)
                        dirVec = prop.mEmitterPos + prop.mAttractorPos - p.mPos;
                    else
                        dirVec = prop.mAttractorPos - p.mPos;

                    if (dot(dirVec, dirVec) != 0.0f)
                        dirVec = normalize(dirVec);
                    dirVec += randVec;
                    p.mVel += dirVec * initial_vel_dampener * prop.mAttractorFactor;

                    // INHERITED VELOCITY
                    p.mVel += prop.mInheritedVelocity * prop.mInheritedFactor;
                    p.mPos += p.mVel;
                    p.mVel *= vel_dampener;
                }

                // SIZE
                float currSizeAppend = (1.0f - fade) * (prop.mEndSize - prop.mStartSize);
                p.mParticleSize = prop.mStartSize + currSizeAppend;
                // COLOUR
                float red = saturate(0.1f*(1.0f - fade) * (prop.mEndColour.r - prop.mStartColour.r));
                float green = saturate(0.1f*(1.0f - fade) * (prop.mEndColour.g - prop.mStartColour.g));
                float blue = saturate(0.1f*(1.0f - fade) * (prop.mEndColour.b - prop.mStartColour.b));
                p.mColor.r += red;
                p.mColor.g += green;
                p.mColor.b += blue;
                // FADE
                float currFadeAppend = (1.0f - fade) * (prop.mFadeEndAmt - prop.mFadeStartAmt);
                p.mColor.w = prop.mFadeStartAmt + currFadeAppend;

                if (prop.mDieInGeometry && CheckOutOfBounds(prop.mHalfLengths, prop.mEmitterPos, p.mPos))
                {
                    p.mLifetime = -10.f;
                    p.mColor.w = 0.0f;
                }
            }
            // Else dead
            else
            {
                // VEL & POS
                if (prop.mSpawnInGeometry)
                    p.mPos = InsideAABB(prop.mHalfLengths,prop.mEmitterPos);
                else
                    p.mPos = OutsideAABB(prop.mHalfLengths, prop.mEmitterPos);

                if (prop.mRandomDirection)
                {

                    p.mVel.x += sign * random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);
                    p.mVel.y += sign * random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);
                    p.mVel.z += sign * random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);

                    p.mVel = normalize(p.mVel);
                    p.mVel *= random(prop.mMinVel, prop.mMaxVel) * initial_vel_dampener;

                }
                else
                {
                    p.mVel = prop.mEmissionAngle;

                    p.mVel.x += sign * random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);
                    p.mVel.y += sign * random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);
                    p.mVel.z += sign * random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);

                    p.mVel = normalize(p.mVel);
                    p.mVel *= random(prop.mMinVel, prop.mMaxVel) * initial_vel_dampener;
                }
                // SIZE
                p.mParticleSize = prop.mStartSize;
                // COLOUR
                p.mColor = prop.mStartColour;
                // FADE
                p.mColor.w = prop.mFadeStartAmt;
                // LIFETIME
                p.mTotalLifeTime = random(prop.mMinLifetime, prop.mMaxLifetime);
                p.mLifetime = p.mTotalLifeTime;

            }

        }
        else
        {
            float sign = 1.0f;

            if ((ID % 2) == 0)
                sign *= -1.0f;

            //If alive
            if (p.mLifetime > 0)
            {
                // LIFETIME
                if (prop.mDT > capFPS)
                    p.mLifetime -= capFPS;
                else
                    p.mLifetime -= prop.mDT;

                float fade = p.mLifetime / p.mTotalLifeTime; 
                // VEL
                if (!prop.mAttracted)
                {
                    p.mVel.y -= prop.mGravityFactor * gravity_vel_dampener;

                    // INHERITED VELOCITY
                    p.mVel += prop.mInheritedVelocity * prop.mInheritedFactor;
                    p.mPos += p.mVel;
                    //p.mVel *= vel_dampener;
                }
                else
                {
                    float3 randVec;
                    randVec.x = random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);
                    randVec.y = random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);
                    randVec.z = random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);

                    float3 dirVec = prop.mAttractorPos - p.mPos;

                    if (prop.mAttractorLocalSpace)
                        dirVec = prop.mEmitterPos + prop.mAttractorPos - p.mPos;
                    else
                        dirVec = prop.mAttractorPos - p.mPos;

                    dirVec = normalize(dirVec);
                    dirVec += randVec;
                    p.mVel += dirVec * initial_vel_dampener * prop.mAttractorFactor;

                    // INHERITED VELOCITY
                    p.mVel += prop.mInheritedVelocity * prop.mInheritedFactor;
                    p.mPos += p.mVel;
                    //p.mVel *= vel_dampener;
                }

                // SIZE
                float currSizeAppend = (1.0f - fade) * (prop.mEndSize - prop.mStartSize);
                p.mParticleSize = prop.mStartSize + currSizeAppend;
                // COLOUR
                float red = saturate(0.1f*(1.0f - fade) * (prop.mEndColour.r - prop.mStartColour.r));
                float green = saturate(0.1f*(1.0f - fade) * (prop.mEndColour.g - prop.mStartColour.g));
                float blue = saturate(0.1f*(1.0f - fade) * (prop.mEndColour.b - prop.mStartColour.b));
                p.mColor.r += red;
                p.mColor.g += green;
                p.mColor.b += blue;
                // FADE
                float currFadeAppend = (1.0f - fade) * (prop.mFadeEndAmt - prop.mFadeStartAmt);
                p.mColor.w = prop.mFadeStartAmt + currFadeAppend;

                if (prop.mDieInGeometry && CheckOutOfBounds(prop.mHalfLengths, prop.mEmitterPos, p.mPos))
                {
                    p.mLifetime = -10.f;
                    p.mColor.w = 0.0f;
                }
            }
            else
            {
                //p.mColor.x = 0.0f;
                p.mColor.w = 0.0f;
            }
        }

        return p;
    }
    else
    {
        p.mLifetime = -10.f;
        p.mColor.w = 0.0f;
        return p;
    }
}

float3 InsideSphere(float radius, float ID, float3 pos)
{


    float3 ret;

    ret.x = random(-radius , radius );
    ret.y = random(-radius , radius );
    ret.z = random(-radius , radius );

    ret = normalize(ret);

    ret *= random(0.1f, radius);

    return ret;
}

float3 OutsideSphere(float radius, float ID, float3 pos)
{
    float3 ret;

    ret.x = random(-radius - ID, radius + ID);
    ret.y = random(-radius - ID, radius + ID);
    ret.z = random(-radius - ID, radius + ID);

    ret = normalize(ret);

    ret *= radius;

    return ret;
}

bool CheckOutOfSphere(float radius, float3 emitpos, float3 particlePos)
{
    float dist = distance(emitpos, particlePos);

    return (dist > (radius + 0.5f));
}

Particle SphereBehaviour(EmitterProperties prop, Particle input, uint ID)
{
    Particle p = input;

    uint particleCap;

    if (prop.mUsedParticles < prop.mMaxParticles)
        particleCap = prop.mUsedParticles;
    else
        particleCap = prop.mMaxParticles;

    if (ID < particleCap)
    {
        int time = (int)prop.mTotalGameTime;
        seed(ID + time);

        if (prop.mIsEmitting)
        {
            float sign = 1.0f;

            if ((ID % 2) == 0)
                sign *= -1.0f;

            //If alive
            if (p.mLifetime > 0)
            {
                // LIFETIME
                if (prop.mDT > capFPS)
                    p.mLifetime -= capFPS;
                else
                    p.mLifetime -= prop.mDT;

                float fade = p.mLifetime / p.mTotalLifeTime; 
                // VEL
                if (!prop.mAttracted)
                {
                    p.mVel.y -= prop.mGravityFactor * gravity_vel_dampener;

                    // INHERITED VELOCITY
                    p.mVel += prop.mInheritedVelocity * prop.mInheritedFactor;
                    p.mPos += p.mVel;
                    p.mVel *= vel_dampener;
                }
                // IS ATTRACTED
                else
                {
                    float3 randVec;
                    randVec.x = random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);
                    randVec.y = random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);
                    randVec.z = random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);

                    float3 dirVec = prop.mAttractorPos - p.mPos;

                    if (prop.mAttractorLocalSpace)
                        dirVec = prop.mEmitterPos + prop.mAttractorPos - p.mPos;
                    else
                        dirVec = prop.mAttractorPos - p.mPos;

                    dirVec = normalize(dirVec);
                    dirVec += randVec;
                    p.mVel += dirVec * initial_vel_dampener * prop.mAttractorFactor;

                    // INHERITED VELOCITY
                    p.mVel += prop.mInheritedVelocity * prop.mInheritedFactor;
                    p.mPos += p.mVel;
                    p.mVel *= vel_dampener;
                }

                // SIZE
                float currSizeAppend = (1.0f - fade) * (prop.mEndSize - prop.mStartSize);
                p.mParticleSize = prop.mStartSize + currSizeAppend;
                
                // COLOUR
                float red = saturate(0.1f*(1.0f - fade) * (prop.mEndColour.r - prop.mStartColour.r));
                float green = saturate(0.1f*(1.0f - fade) * (prop.mEndColour.g - prop.mStartColour.g));
                float blue = saturate(0.1f*(1.0f - fade) * (prop.mEndColour.b - prop.mStartColour.b));
                p.mColor.r += red;
                p.mColor.g += green;
                p.mColor.b += blue;
                // FADE
                float currFadeAppend = (1.0f - fade) * (prop.mFadeEndAmt - prop.mFadeStartAmt);
                p.mColor.w = prop.mFadeStartAmt + currFadeAppend;

                if (prop.mDieInGeometry && CheckOutOfSphere(prop.mRadius, prop.mEmitterPos, p.mPos))
                {
                    p.mLifetime = -10.f;
                    p.mColor.w = 0.0f;
                }
            }
            // Else dead
            else
            {
                // VEL & POS
                if (prop.mSpawnInGeometry)
                    p.mPos = prop.mEmitterPos + InsideSphere(prop.mRadius, ID, prop.mEmitterPos);
                else
                    p.mPos = prop.mEmitterPos + OutsideSphere(prop.mRadius, ID, prop.mEmitterPos);

                if (prop.mRandomDirection)
                {

                    p.mVel.x += sign * random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);
                    p.mVel.y += sign * random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);
                    p.mVel.z += sign * random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);

                    p.mVel = normalize(p.mVel);
                    p.mVel *= random(prop.mMinVel, prop.mMaxVel) * initial_vel_dampener;

                }
                else
                {
                    p.mVel = prop.mEmissionAngle;

                    p.mVel.x += sign * random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);
                    p.mVel.y += sign * random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);
                    p.mVel.z += sign * random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);

                    p.mVel = normalize(p.mVel);
                    p.mVel *= random(prop.mMinVel, prop.mMaxVel) * initial_vel_dampener;
                }
                // SIZE
                p.mParticleSize = prop.mStartSize;
                
                // COLOUR
                p.mColor = prop.mStartColour;
                // FADE
                p.mColor.w = prop.mFadeStartAmt;
                // LIFETIME
                p.mTotalLifeTime = random(prop.mMinLifetime, prop.mMaxLifetime);
                p.mLifetime = p.mTotalLifeTime;

            }

        }
        else
        {
            float sign = 1.0f;

            if ((ID % 2) == 0)
                sign *= -1.0f;


            //If alive
            if (p.mLifetime > 0)
            {
                // LIFETIME
                if (prop.mDT > capFPS)
                    p.mLifetime -= capFPS;
                else
                    p.mLifetime -= prop.mDT;
                float fade = p.mLifetime / p.mTotalLifeTime; 
                // VEL
                if (!prop.mAttracted)
                {
                    p.mVel.y -= prop.mGravityFactor * gravity_vel_dampener;

                    // INHERITED VELOCITY
                    p.mVel += prop.mInheritedVelocity * prop.mInheritedFactor;
                    p.mPos += p.mVel;
                    p.mVel *= vel_dampener;
                }
                else
                {
                    float3 randVec;
                    randVec.x = random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);
                    randVec.y = random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);
                    randVec.z = random(-prop.mEmissionAngleVariance, prop.mEmissionAngleVariance);

                    float3 dirVec = prop.mAttractorPos - p.mPos;

                    if (prop.mAttractorLocalSpace)
                        dirVec = prop.mEmitterPos + prop.mAttractorPos - p.mPos;
                    else
                        dirVec = prop.mAttractorPos - p.mPos;
                    if (dot(dirVec, dirVec) != 0.0f)
                        dirVec = normalize(dirVec);
                    dirVec += randVec;
                    p.mVel += dirVec * initial_vel_dampener * prop.mAttractorFactor;

                    // INHERITED VELOCITY
                    p.mVel += prop.mInheritedVelocity * prop.mInheritedFactor;
                    p.mPos += p.mVel;
                    p.mVel *= vel_dampener;
                }

                // SIZE
                float currSizeAppend = (1.0f - fade) * (prop.mEndSize - prop.mStartSize);
                p.mParticleSize = prop.mStartSize + currSizeAppend;
                
                // COLOUR
                float red = saturate(0.1f*(1.0f - fade) * (prop.mEndColour.r - prop.mStartColour.r));
                float green = saturate(0.1f*(1.0f - fade) * (prop.mEndColour.g - prop.mStartColour.g));
                float blue = saturate(0.1f*(1.0f - fade) * (prop.mEndColour.b - prop.mStartColour.b));
                p.mColor.r += red;
                p.mColor.g += green;
                p.mColor.b += blue;
                // FADE
                float currFadeAppend = (1.0f - fade) * (prop.mFadeEndAmt - prop.mFadeStartAmt);
                p.mColor.w = prop.mFadeStartAmt + currFadeAppend;

                if (prop.mDieInGeometry && CheckOutOfSphere(prop.mRadius, prop.mEmitterPos, p.mPos))
                {
                    p.mLifetime = -10.f;
                    p.mColor.w = 0.0f;
                }
            }
            else
            {
                //p.mColor.x = 0.0f;
                p.mColor.w = 0.0f;
            }
        }

        return p;
    }
    else
    {
        p.mLifetime = -10.f;
        p.mColor.w = 0.0f;
        return p;
    }
}